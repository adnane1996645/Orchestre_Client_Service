#define  _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "orchestre_service.h"
#include "client_service.h"
#include "service.h"
#include "service_somme.h"
#include "service_compression.h"
#include "service_maximum.h"
#include <string.h>



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

    // initialisations diverses : analyse de argv
    fdServiceFromOrchestre = strtol(argv[2], NULL, 10);

    while (true)
    {
        // attente d'un code de l'orchestre (via tube anonyme)
        Order order = getOrderFromOrchestre(fdServiceFromOrchestre);

        if(!isItOk(order))
              break;            //code de fin => sortie de la boucle
        else
        {
            int passeword = getMotpasse(order);    //réception du mot de passe de l'orchestre

            fdServiceToClient = open(argv[3], O_WRONLY, 0644);     //ouverture des deux tubes nommés avec le client
            fdClientToService = open(argv[4], O_RDONLY, 0644);
            bool condition = getPWDFromClient(fdClientToService, passeword);
            //    si mot de passe incorrect
            if(condition)
                  sendReponsePWD(fdServiceToClient, condition);
            else
            {
              //        envoi au client d'un code d'erreur
              //    sinon
              //        envoi au client d'un code d'acceptation
              //        appel de la fonction de communication avec le client :
              //            une fct par service selon numService (cf. argv[1]) :
              //                   . service_somme
              //                ou . service_compression
              //                ou . service_maximum
              //        attente de l'accusé de réception du client
              //    finsi
              //    fermeture ici des deux tubes nommés avec le client
              //    modification du sémaphore pour prévenir l'orchestre de la fin
              // finsi
            }
        }

    }

    // libération éventuelle de ressources

    return EXIT_SUCCESS;
}
