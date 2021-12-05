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


static int startServices(int **pipe_OtoS,int semid[]){
  
  

  //boucle de création des i services
  for(int i = 0; i < 3; i++){
    
    // Initialise le tube anonyme
    int ret = pipe(pipe_OtoS[i]);
    assert(ret != -1);
    
    int resFork = fork();
    assert(resFork != -1);

    
    if (resFork == 0) // on est dans un service général
    {
      char fd[20];
      
      ret = close(pipe_OtoS[i][1]);//on ferme le tube anonyme en écriture du coté du service
      assert(ret != -1);

      ret = sprintf(fd,"%d",pipe_OtoS[i][0]);
      assert(ret >= 0);
      
      if(i == 0){// en position 2 ("1") c'est le projid pour la clé de semaphore
	char *const parmList[] = {"Service", "0", "1", fd, "../pipe_s2c_0", "../pipe_c2s_0", NULL};
	semid[i] = mysemget(1);
      }
      
      else if(i == 1){
	char *const parmList[] = {"Service", "1", "2", fd, "../pipe_s2c_1", "../pipe_c2s_1", NULL};
	semid[i] = mysemget(2);
      }
      
      else if(i == 2){
	char *const parmList[] = {"Service", "2", "3", fd, "../pipe_s2c_2", "../pipe_c2s_2", NULL};
	semid[i] = mysemget(3);
      }
      
      execv("Service", parmList);
      
      printf("Probleme dans le exec\n");//on est pas sensé se retrouver ici
    }
    else{
      ret = close(pipe_OtoS[i][0]);//on ferme le tube anonyme en lecture du coté du orchestre
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
  int semidS[3];
  int unnamed_pipe_OtoS[3][2];
  Order ord;
  Com c;
  
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
  startServices(unnamed_pipe_OtoS, semidS);
  
  while (! fin)
    {
      // ouverture ici des tubes nommés avec un client
      fd = open_pipes_o(); //fd[0]->lecture  /  fd[1]->ecriture
      
      // attente d'une demande de service du client
      service = rcv_request(fd[0]);
      
      // détecter la fin des traitements lancés précédemment via
      // les sémaphores dédiés (attention on n'attend pas la
      // fin des traitement, on note juste ceux qui sont finis)
      /*une fonction dans orchestre_service pourrait etre utile, i.e: bool * getserv_fin(int * servId); */
      if(semctl(semidS[service], 0, GETVAL, 0) == 0)
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
      else if(serv_fini[service] == false)
	send_reply(fd[1], false);
      
      // sinon
      else{
      //     envoi au client d'un code d'acceptation (via le tube nommé)
	send_reply(fd[1], true);
	
      //     génération d'un mot de passe
	pwd = genPwd();
	
      //     envoi d'un code de travail au service (via le tube anonyme)
      //     envoi du mot de passe au service (via le tube anonyme)
	ord = init_Order(true, pwd);
	OrderOrchestreToService(unnamed_pipe_OtoS[service][1], ord);
	
      //     envoi du mot de passe au client (via le tube nommé)
      //     envoi des noms des tubes nommés au client (via le tube nommé)
	c = init_com(service , pwd);
	send_com(fd[1], c);
	
      // finsi
      }
      
      // attente d'un accusé de réception du client
      rcv_adc(fd[0]);

      // fermer les tubes vers le client
      close_pipes(fd);
      
      // il peut y avoir un problème si l'orchestre revient en haut de la
      // boucle avant que le client ait eu le temps de fermer les tubes
      // il faudrait régler cela avec un sémaphore, mais on va se contenter
      // d'une attente de 1 seconde (solution non satisfaisante mais simple)
      sleep(1);
      if(c != NULL)//peut importe que ce code soit avant ou après le sleep
	destroy_com(&c);//on libere l'espace alloué par les mallocs fais dans les init ici
      if(ord != NULL)//car on peux pas le faire en dehors de la boucle sans perdre les adresses des précédentes allocations
	destroy_Order(&ord);
    }
  
  
  // attente de la fin des traitements en cours (via les sémaphores)
  
  //on pourrais ptt attendre pour 1 service puis lui envoyé le code de fin puis attendre sa terminaison,puis service suivant(ect)
  
  for(int i = 0; i < 3; i++)
    mysem_attente(semidS[i]);
  
  
  // envoi à chaque service d'un code de fin
  ord = init_Order(false, pwd);
  
  for(int i = 0; i < 3; i++)
    OrderOrchestreToService(unnamed_pipe_OtoS[i][1], ord);
  
  // attente de la terminaison des processus services
  for(int i = 0; i < 3; i++)
    wait(NULL);
  
  // libération des ressources
  destroy_Order(&ord);
  
  return EXIT_SUCCESS;
}
