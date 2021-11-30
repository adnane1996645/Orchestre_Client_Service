#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "io.h"
#include "memory.h"
#include "myassert.h"

#include "service.h"
#include "client_orchestre.h"
#include "client_service.h"

#include "client_arret.h"
#include "client_somme.h"
#include "client_maximum.h"
#include "client_compression.h"


static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s <num_service> ...\n", exeName);
    fprintf(stderr, "        <num_service> : entre -1 et %d\n", SERVICE_NB - 1);
    fprintf(stderr, "                        -1 signifie l'arrêt de l'orchestre\n");
    fprintf(stderr, "        ...           : les paramètres propres au service\n");
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}

int main(int argc, char * argv[])
{
    if (argc < 2)
        usage(argv[0], "nombre paramètres incorrect");

    int numService = io_strToInt(argv[1]);
    if (numService < -1 || numService >= SERVICE_NB)
        usage(argv[0], "numéro service incorrect");

    // appeler la fonction de vérification des arguments
    //     une fct par service selon numService
    //            . client_arret_verifArgs
    //         ou . client_somme_verifArgs
    //         ou . client_compression_verifArgs
    //         ou . client_maximum_verifArgs
    else if(numService == -1)
      client_arret_verifArgs(argc, argv[]);
    else if(numService == 0)
      client_somme_verifArgs(argc, argv[]);
    else if(numService == 1)
      client_compression_verifArgs(argc, argv[]);
    else if(numService == 2)
      client_maximum_verifArgs(argc, argv[]);

    // initialisations diverses s'il y a lieu
    int semid = recup_mutex();
    int *fd;
    bool rp;
    Com com;

    // entrée en section critique pour communiquer avec l'orchestre
    p_mutex(semid);
    
    // ouverture des tubes avec l'orchestre
    fd = open_pipes_c(); //fd[0]->lecture  /  fd[1]->ecriture

    // envoi à l'orchestre du numéro du service
    send_request(fd[1], numService);

    // attente code de retour
    rp = rcv_reply(fd[0]);
    
    // si code d'erreur
    //     afficher un message erreur
    if(rp == false)
      printf("erreur: l'orchestre n'a pas accepter votre demande \n");
    
    // sinon si demande d'arrêt (i.e. numService == -1)
    //     afficher un message
    else if(numService == -1)
      printf("l'orchestre va s'arrêter \n");
      
    // sinon
    //     récupération du mot de passe et des noms des 2 tubes
    else
      com = rcv_com(fd[0]);
    
    // finsi
    //
    
    // envoi d'un accusé de réception à l'orchestre
    send_adc(fd[1]);
    
    // fermeture des tubes avec l'orchestre
    close_pipes(fd);
    
    // sortie de la section critique
    v_mutex(semid);
    
    //
    // si pas d'erreur et service normal
    ///////****je vois pas à quoi çà correspond****////////
    if(/*condition?? */){  
    
    //     ouverture des tubes avec le service
      int fdW = openTubeWrite(getPipe(com, 1), "erreur d'ouverture en écriture du tube entre le client et les service");
      int fdR = openTubeRead(getPipe(com, 2), "erreur d'ouverture en lecture du tube entre le client et les service");
      
    //     envoi du mot de passe au service
      sendPWD(fd[1], getPwd(com));
      
    //     attente de l'accusé de réception du service
      getACR(fd[0]);
    //     si mot de passe non accepté
    //         message d'erreur
      if(getReponsePWD(fd[0]) == WRONG_PWD)
	printf("le mot de passe n'a pas été accepté\n");
    //     sinon
    //         appel de la fonction de communication avec le service :
    //             une fct par service selon numService :
    //                    . client_somme
    //                 ou . client_compression
    //                 ou . client_maximum
      else {
	if(numService == 0)
	  client_somme(fd[0], fd[1], argc, argv[]);
	else if(numService == 1)
	  client_compression(fd[0], fd[1], argc, argv[]);
	else if(numService == 2)
	  client_maximum(fd[0], fd[1], argc, argv[]);
	
    //         envoi d'un accusé de réception au service
	sendACR(fd[1]);
	
    //     finsi
      }
      
    //     fermeture des tubes avec le service
      closeTubeWrite(fdW, "erreur de fermeture en écriture du tube entre le client et les service");
      closeTubeRead(fdR, "erreur de fermeture en lecture du tube entre le client et les service");
    // finsi
    }
    
    // libération éventuelle de ressources
    
    
    return EXIT_SUCCESS;
}
