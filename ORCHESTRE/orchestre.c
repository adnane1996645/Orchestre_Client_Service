/************************************************************************
 * Projet CC2
 * Programmation avancée en C
 *
 * Auteurs: Esteban Mauricio & Adnane LAANANI
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>

#include "../UTILS/myassert.h"
#include "../CONFIG/config.h"
#include "../CLIENT_ORCHESTRE/client_orchestre.h"
#include "../ORCHESTRE_SERVICE/orchestre_service.h"
#include "../SERVICE/service.h"


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


// il n'y à pas d'allocation dynamique car peux importe le nb de service, on passera tjr en param un tableau a 2 colonnes
static void startServices(int (*pipe_OtoS)[2],int semid[]){//2 colonnes(donc une ligne) pour chaque pipe

  for(int i = 0; i < 3; i++){//création des semaphore pour les services
    semid[i] = mysemget_create(i+1);
  }
  
  int resFork = fork();
  myassert(resFork != -1, "Erreur dans l'exécution du fork exterieur a la boucle for");
  
  //boucle de création des i services
  for(int i = 0; i < 3; i++){
    
    // Initialise le tube anonyme
    int ret = pipe(pipe_OtoS[i]);
    myassert(ret != -1, "Erreur dans l'initialisation du tube anonyme OtoS ");

    if (resFork != 0 && i != 0){
      resFork = fork();
      myassert(resFork != -1, "Erreur du fork interieur a la boucle for");      
    }
    
    if (resFork == 0) // on est dans un service général
    {
      char fd[20];
      char numService[20];
      
      ret = close(pipe_OtoS[i][1]);//on ferme le tube anonyme en écriture du coté du service
      myassert(ret != -1, "Erreur dans la fermeture du tube anonyme en ecriture");

      
      ret = sprintf(fd,"%d",pipe_OtoS[i][0]);
      myassert(ret >= 0, "Erreur lors de la convertion en chaine de caractere du file descriptor du tube");

      ret = sprintf(numService,"%d",i);
      myassert(ret >= 0, "Erreur lors de la convertion en chaine de caractere du numéro de service");

      
      if(i == 0){// en position 2 ("1") c'est le projid pour la clé de semaphore
	char *const parmList[] = {"SERVICE/service", numService, "1", fd, "../pipe_s2c_0", "../pipe_c2s_0", NULL};
	ret = execv("SERVICE/service", parmList);
	myassert(ret != -1, "Erreur du execv"); 
      }
      
      else if(i == 1){
	char *const parmList[] = {"SERVICE/service", numService, "2", fd, "../pipe_s2c_1", "../pipe_c2s_1", NULL};
	ret = execv("SERVICE/service", parmList);
	myassert(ret != -1, "Erreur du execv"); 
      }
      
      else if(i == 2){
	char *const parmList[] = {"SERVICE/service", numService, "3", fd, "../pipe_s2c_2", "../pipe_c2s_2", NULL};
	ret = execv("SERVICE/service", parmList);
	myassert(ret != -1, "Erreur du execv"); 
      }
    }
    else{
      ret = close(pipe_OtoS[i][0]);//on ferme le tube anonyme en lecture du coté du orchestre
      myassert(ret != -1, "Erreur dans la fermeture du tube anonyme en lecture");
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
  int unnamed_pipe_OtoS[3][2];//pas d'allocation dynamique (voir fonction startServices)
  Order ord = NULL;
  Com c = NULL;
  
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
      printf("attente d'une demande du client \n");
      service = rcv_request(fd[0]);
      
      
      // détecter la fin des traitements lancés précédemment via
      // les sémaphores dédiés (attention on n'attend pas la
      // fin des traitement, on note juste ceux qui sont finis)
      /*une fonction dans orchestre_service pourrait etre utile, i.e: bool * getserv_fin(int * servId); */
      if(semctl(semidS[service], 0, GETVAL, 0) == 1)
	serv_fini[service] = true;
      else serv_fini[service] = false;

      
      // analyse de la demande du client
      // si ordre de fin
      //     envoi au client d'un code d'acceptation (via le tube nommé)
      //     marquer le booléen de fin de la boucle
      printf("analyse demande de client... \n\n");
      if(service == -1){
	send_reply(fd[1], true);
	fin = true;
      }
	
      // sinon si service non ouvert
      //     envoi au client d'un code d'erreur (via le tube nommé)
      else if(config_isServiceOpen(service) == false){
	send_reply(fd[1], false);
	printf("service non ouvert  \n\n");
      }
      
      // sinon si service est déjà en cours de traitement
      //     envoi au client d'un code d'erreur (via le tube nommé)
      else if(serv_fini[service] == false){
	send_reply(fd[1], false);
	printf("service en cour d'execution, retour un code d'erreur au client  \n\n");
      }
      
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
	printf("init com  \n\n");
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
  
  printf(" envoi aux service d'un code de fin... \n\n");
  // envoi à chaque service d'un code de fin
  ord = init_Order(false, pwd);
  
  for(int i = 0; i < 3; i++)
    OrderOrchestreToService(unnamed_pipe_OtoS[i][1], ord);
  
  // attente de la terminaison des processus services
  for(int i = 0; i < 3; i++){
    wait(NULL);
    mysem_destroy(semidS[i]);
  }
  
  // libération des ressources
  destroy_Order(&ord);
  destroy_mutex(semid);
  printf("le programme c'est terminer avec succés! \n\n");
  return EXIT_SUCCESS;
}
