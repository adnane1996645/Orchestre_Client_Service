#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../UTILS/memory.h"
#include "../UTILS/io.h"
#include "../UTILS/myassert.h"

#include "client_service.h"
#include "client_maximum.h"
#include <assert.h>
#include <limits.h>
#include <unistd.h>


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
 struct DataP{
     int NB_THREADS;
     float* TAB;
     int length;
     float max;
 };

typedef struct DataP * Data;

void client_maximum_verifArgs(int argc, char * argv[])
{
    if (argc < 4)
        usage(argv[0], argv[1], "nombre d'arguments");

    int thread = io_strToInt(argv[2]);
    myassert(thread >= INT_MIN && thread <= INT_MAX , "Erreur overflow int");
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
static void sendData(int fdWrite , Data data)
{
  // envoi du nombre de threads et du tableau de float
  write(fdWrite, &(data->NB_THREADS), sizeof(int));
  //assert(ret != -1);
  write(fdWrite, &(data->length), sizeof(int));
  for(int i = 0; i < data->length; i++)
        write(fdWrite, data->TAB+i, sizeof(float));
}

// ---------------------------------------------
// fonction de réception des résultats en provenance du service et affichage
// Les paramètres sont
// - le file descriptor du tube de communication en provenance du service
// - autre chose si nécessaire
static void receiveResult(int fd, Data data)
{
    // récupération du maximum
    read(fd, &(data->max), sizeof(float));
    // affichage du résultat
    printf("resultat: %f\n", data->max);
}


// ---------------------------------------------
// Fonction appelée par le main pour gérer la communications avec le service
// Les paramètres sont
// - les deux file descriptors des tubes nommés avec le service
// - argc et argv fournis en ligne de commande
// Cette fonction analyse argv et en déduit les données à envoyer
//    - argv[2] : nombre de threads
//    - argv[3] à argv[argc-1]: les nombres flottants
void client_maximum(int fdWrite, int fdRead, int argc, char * argv[])
{
  // variables locales éventuelles
  Data data = malloc(sizeof(struct DataP));
  data->NB_THREADS = io_strToInt(argv[2]);

  data->length = argc-3;

  data->TAB = malloc(sizeof(float) * data->length);
  for(int i = 3; i < argc; i++)
     data->TAB[i-3] = io_strToFloat(argv[i]);

  sendData(fdWrite, data);
  free(data->TAB);
  free(data);
  receiveResult(fdRead, data);
}
