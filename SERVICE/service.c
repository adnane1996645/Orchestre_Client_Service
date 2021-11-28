/****************************************************************
Bionôme :  - Adnane LAANANI
           -
*****************************************************************/
#define  _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "orchestre_service.h"
#include "client_service.h"
#include "service.h"
#include "service_somme.h"
#include "service_compression.h"
#include "myasset.h"
#include "service_maximum.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s <num_service> <clé_sémaphore> <fd_tube_anonyme> "
            "<nom_tube_service_vers_client> <nom_tube_client_vers_service>\n",
            exeName);
    fprintf(stderr, "        <num_service>     : entre 0 et %d\n", SERVICE_NB - 1);
    fprintf(stderr, "        <clé_sémaphore>   : entre ce service et l'orchestre (clé au sens ftok)\n");
    fprintf(stderr, "        <fd_tube_anonyme> : entre ce service et l'orchestre\n");
    fprintf(stderr, "        <nom_tube_...>    : noms des deux tubes nommés reliés à ce service\n");
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}

/*----------------------------------------------*
 * fonction main
 *----------------------------------------------*/
int main(int argc, char * argv[])
{
    if (argc != 6)
        usage(argv[0], "nombre paramètres incorrect");

    int fdServiceFromOrchestre;
    int fdServiceToClient;
    int fdClientToService;
    int Service = strtol(argv[1], NULL, 10);
    int cleSem = strtol(argv[2], NULL, 10);

    // initialisations diverses : analyse de argv
    fdServiceFromOrchestre = strtol(argv[2], NULL, 10);

    printf("SERVICE... :\n");
    if(Service < SERVICE_ARRET || Service >= SERVICE_NB)
        usage(argv[0], "Le numéro de service est invalide\n");

    if(Service == SERVICE_ARRET)
    {
        printf("||\n||=>Arrêt du Service ...\n||\n||=>Fin!\n");
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        // attente d'un code de l'orchestre (via tube anonyme)
        printf("Reception de l'ordre de orchestre ...:\n");
        Order order = getOrderFromOrchestre(fdServiceFromOrchestre);
        if(!isItOk(order))
        {
          printf("||\n||=>    L'ordre est un ordre de fin.\n");
                break;        //code de fin => sortie de la boucle
        }
        else
        {
            printf("||\n||=>    Reception du mot de passe de l'orchestre ... :\n");

            int passeword = getMotpasse(order);    //réception du mot de passe de l'orchestre

            //ouverture des deux tubes nommés avec le client
            fdServiceToClient = openTubeWrite(argv[3], "Erreur dans l'ouverture du tube Service==>Client\n");
            fdClientToService = openTubeRead(argv[4], "Erreur dans l'ouverture du tube Client==>Service\n");

            bool condition = getPWDFromClient(fdClientToService, passeword);
            if(condition) //    si mot de passe incorrect
            {
              //        envoi au client d'un code d'erreur
                    printf(".\n.\n.\n      Mot de passe incorect : envoi au client un code d'erreur : %s\n", WRONG_PWD);
                    sendReponsePWD(fdServiceToClient, condition);
            }
            else  //    sinon
            {
              //        envoi au client d'un code d'acceptation
                    printf(".\n.\n.\n      Mot de passe correct : envoi au client un code d'acceptation : %s\n", OK_PWD);
                    sendReponsePWD(fdServiceToClient, condition);
                    switch(Service)
                    {
                       // appel de la fonction de communication avec le client :
                       //            une fct par service selon numService (cf. argv[1]) :
                       case SERVICE_SOMME :
                              service_somme(fdClientToService, fdServiceToClient);//. service_somme
                              break;
                       case SERVICE_COMPRESSION :
                              service_compression(fdClientToService, fdServiceToClient);//. service_compression
                              break;
                       case SERVICE_MAXIMUM :
                              service_maximum(fdClientToService, fdServiceToClient)//. service_maximum
                              break;
                       default :
                              printf("        Le numéro de service est incorrect et doit être entre 0 et %d\n", SERVICE_NB - 1);
                              break;
                      }
                       //        attente de l'accusé de réception du client
                       printf(".\n.\n.\n      En attente d'un accusé de réception du client ...: \n");
                       getACR(fdClientToService);
                       //    finsi
              }
              //    fermeture ici des deux tubes nommés avec le client
              close(fdServiceToClient);
              close(fdClientToService);
              //    modification du sémaphore pour prévenir l'orchestre de la fin
              int semId = mysemget(cleSem);
              mysem_descre(semId);
              // finsi
          }
      }
    // libération éventuelle de ressources
    destroy_Order(&order)

  return EXIT_SUCCESS;
}
