#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "client_service.h"
#include "client_maximum.h"


/*----------------------------------------------*
 * usage pour le client maximum
 *----------------------------------------------*/

static void usage(const char *exeName, const char *numService, const char *message)
{
    fprintf(stderr, "Client maximum de float\n");
    fprintf(stderr, "usage : %s %s <nbThreads> <f1> <f2> .. <fn>\n", exeName, numService);
    fprintf(stderr, "        %s           : numéro du service\n", numService);
    fprintf(stderr, "        <nbThreads>   : nombre de threads\n");
    fprintf(stderr, "        <f1> ... <fn> : les nombres à tester (au moins un)\n");
    fprintf(stderr, "exemple d'appel :\n");
    fprintf(stderr, "    %s %s 2 6.3 5.8 -2.33 0.0 8.34 12.98\n", exeName, numService);
    fprintf(stderr, "    -> 6 nombres à tester avec 2 threads\n");
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}

/*----------------------------------------------*
 * fonction de vérification des paramètres
 *----------------------------------------------*/

void client_maximum_verifArgs(int argc, char * argv[])
{
    if (argc < 4)
        usage(argv[0], argv[1], "nombre d'arguments");
    
}


/*----------------------------------------------*
 * fonctions de communication avec le service
 *----------------------------------------------*/

// ---------------------------------------------
// fonction d'envoi des données du client au service
// Les paramètres sont
// - le file descriptor du tube de communication vers le service
// - le nombre de threads que doit utiliser le service
// - le tableau de float dont on veut le maximum
static void sendData(int fd_pipe_to_service , int nbre_threads, float * tab_float)
{
  // envoi du nombre de threads et du tableau de float
  int ret = write(fd_pipe_to_service, &nbre_threads, sizeof(int));
  assert(ret != -1);
  ret = write(fd_pipe_to_service, &tab_float, sizeof(float *));
  assert(ret != -1);
}

// ---------------------------------------------
// fonction de réception des résultats en provenance du service et affichage
// Les paramètres sont
// - le file descriptor du tube de communication en provenance du service
// - autre chose si nécessaire
static void receiveResult(int fd /* ,autres paramètres si nécessaire */)
{
    // récupération du maximum
  float res;
  int ret = read(fd,&res,sizeof(float));
  assert(ret != -1);
    // affichage du résultat
  printf("resultat: %f\n",res);
}


// ---------------------------------------------
// Fonction appelée par le main pour gérer la communications avec le service
// Les paramètres sont
// - les deux file descriptors des tubes nommés avec le service
// - argc et argv fournis en ligne de commande
// Cette fonction analyse argv et en déduit les données à envoyer
//    - argv[2] : nombre de threads
//    - argv[3] à argv[argc-1]: les nombres flottants
void client_maximum(int fd1, int fd2, int argc, char * argv[])
{
  // variables locales éventuelles
  float tab[argc-4];
  for(int i = 3; i <= argc-1; i++)
    tab[i-3] = argv[i];

  
  sendData(fd1, argv[2], tab);
  receiveResult(fd2);
}

