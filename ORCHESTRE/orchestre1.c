/************************************************************************
 * Projet CC2
 * Programmation avancée en C
 *
 * Auteurs: Esteban Mauricio & Adnane
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#include "config.h"
#include "client_orchestre.h"
#include "orchestre_service.h"
#include "service.h"


static int genPwd(){
  srand(time(0));
  int rd = rand() % 100000 + 100000;

  return rd;
}

static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s <fichier config>\n", exeName);
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}

static void Pere(int semid, ) //orchestre originale
{

}

static void son(int semid) //ou on va laner un service
{
    int ret;

    ret = semget();
}

static void startServices(int  * pipe_O2S){

  // Initialise le tube anonyme
  int ret = pipe(pipe_O2S);
  assert(ret != -1);

  //boucle de création des i services
  for(int i = 0; i < 3; i++){
    int resFork = fork();
    assert(resFork != -1);


    if (resFork == 0) // on est dans un service général
    {
      char fd[20];

      ret = close(pipe_O2S[1]);//on ferme le tube anonyme en écriture du coté du service
      assert(ret != -1);

      ret = sprintf(fd,"%d",pipe_O2S[0]);
      assert(ret >= 0);

      char * const parmList[7];
      if(i == 0)// en position 2 ("1") c'est le projid pour la clé de semaphore
      {
          parmList[0] = "../SERVICE/Service";
          parmList[1] = "0";
          parmList[2] = "1";
          parmList[3] = fd;
          parmList[4] = "../pipe_s2c_0";
          parmList[5] = "../pipe_c2s_0";
          parmList[6] = NULL;
      }
      else if(i == 1)
      {
          parmList[0] = "../SERVICE/Service";
          parmList[1] = "1";
          parmList[2] = "2";
          parmList[3] = fd;
          parmList[4] = "../pipe_s2c_1";
          parmList[5] = "../pipe_c2s_1";
          parmList[6] = NULL;
      }
      else
	    {
          parmList[0] = "../SERVICE/Service";
          parmList[1] = "2";
          parmList[2] = "3";
          parmList[3] = fd;
          parmList[4] = "../pipe_s2c_2";
          parmList[5] = "../pipe_c2s_2";
          parmList[6] = NULL;
      }

      execv("../SERVICE/Service", parmList);
      //on est pas sensé se retrouver ici
    }
    else{
      ret = close(w_pipe_OtoS);//on ferme le tube anonyme en lecture du coté du orchestre
      assert(ret != -1);
    }
  }
}


int main(int argc, char * argv[])
{
  if (argc != 2)
    usage(argv[0], "nombre paramètres incorrect");

  bool fin = false;
  int *fd;
  int service, pwd;
  bool serv_fini[3];
  int unnamed_pipe_OtoS[2];

  // lecture du fichier de configuration
  config_init(argv[1]);

  // Pour la communication avec les clients
  // - création de 2 tubes nommés pour converser avec les clients
  // - création d'un sémaphore pour que deux clients ne communiquent pas en même temps avec l'orchestre
  creat_named_pipe();
  int semid = creat_mutex();

  // lancement des services, avec pour chaque service :
  // - création d'un tube anonyme pour converser (orchestre vers service)
  // - un sémaphore pour que le service préviene l'orchestre de la fin d'un traitement
  // - création de deux tubes nommés (pour chaque service) pour les communications entre les clients et les services

  startServices(unnamed_pipe_OtoS[0], unnamed_pipe_OtoS[1]);

  while (! fin)
    {
      // ouverture ici des tubes nommés avec un client
      fd = open_pipes_o(); //fd[0]->lecture  /  fd[1]->ecriture

      // attente d'une demande de service du client
      service = rcv_request(fd[0]);

      // détecter la fin des traitements lancés précédemment via
      // les sémaphores dédiés (attention on n'attend pas la
      // fin des traitement, on note juste ceux qui sont finis)

      /*besoin de faire une fonction dans orchestre_service, i.e: bool * getserv_fin(int * servId); */
      if(semctl(semid[service], 0, GETVAL, 0) == 0)
	serv_fini[service] = true;
      else serv_fini[service] = false;


      // analyse de la demande du client
      // si ordre de fin
      //     envoi au client d'un code d'acceptation (via le tube nommé)
      //     marquer le booléen de fin de la boucle
      if(service == -1){
	send_reply(fd[1], true);
	fin = true;
      }

      // sinon si service non ouvert
      //     envoi au client d'un code d'erreur (via le tube nommé)
      else if(config_isServiceOpen(service) == false)
	send_reply(fd[1], false);

      // sinon si service est déjà en cours de traitement
      //     envoi au client d'un code d'erreur (via le tube nommé)
      else if(serv_fini == false)
	send_reply(fd[1], false);

      // sinon
      else{
      //     envoi au client d'un code d'acceptation (via le tube nommé)
	send_reply(fd[1], true);

      //     génération d'un mot de passe
	pwd = genPwd();

      //     envoi d'un code de travail au service (via le tube anonyme)


      //     envoi du mot de passe au service (via le tube anonyme)
      //     envoi du mot de passe au client (via le tube nommé)
      //     envoi des noms des tubes nommés au client (via le tube nommé)
      // finsi

      // attente d'un accusé de réception du client
	rcv_adc(fd[0]);

      // fermer les tubes vers le client


      // il peut y avoir un problème si l'orchestre revient en haut de la
      // boucle avant que le client ait eu le temps de fermer les tubes
      // il faudrait régler cela avec un sémaphore, mais on va se contenter
      // d'une attente de 1 seconde (solution non satisfaisante mais simple)
	sleep(1);
      }
    }
  // attente de la fin des traitements en cours (via les sémaphores)

  // envoi à chaque service d'un code de fin

  // attente de la terminaison des processus services
  wait(NULL);
  wait(NULL);
  wait(NULL);
  // libération des ressources

  return EXIT_SUCCESS;
}
