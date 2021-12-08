#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "client_service.h"
#include "client_compression.h"
#include <string.h>
#include <unistd.h>
#include "../UTILS/memory.h"
#include "../UTILS/io.h"
#include "../UTILS/myassert.h"



/*----------------------------------------------*
 * usage pour le client compression
 *----------------------------------------------*/

static void usage(const char *exeName, const char *numService, const char *message)
{
    fprintf(stderr, "Client compression de chaîne\n");
    fprintf(stderr, "usage : %s %s <chaîne>\n", exeName, numService);
    fprintf(stderr, "        %s      : numéro du service\n", numService);
    fprintf(stderr, "        <chaine> : chaîne à compresser\n");
    fprintf(stderr, "exemple d'appel :\n");
    fprintf(stderr, "    %s %s \"aaabbcdddd\"\n", exeName, numService);
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}


/*----------------------------------------------*
 * fonction de vérification des paramètres
 *----------------------------------------------*/

 struct DataP
 {
     char * str;
     int lstr;
     char * dest;
     int ldest;
 };

 typedef struct DataP * Data;

void client_compression_verifArgs(int argc, char * argv[])
{
    if (argc != 3)
        usage(argv[0], argv[1], "nombre d'arguments");
    // éventuellement d'autres tests
    int service = io_strToInt(argv[1]);
    myassert(service == 1, "Le numéro de service n'est pas le service de compression");
}


/*----------------------------------------------*
 * fonctions de communication avec le service
 *----------------------------------------------*/

// ---------------------------------------------
// fonction d'envoi des données du client au service
// Les paramètres sont
// - le file descriptor du tube de communication vers le service
// - la chaîne devant être compressée
static void sendData(int fdWrite,  Data data)
{
      write(fdWrite, &(data->lstr), sizeof(int));
      write(fdWrite, data->str, sizeof(char) * data->lstr);
}

// ---------------------------------------------
// fonction de réception des résultats en provenance du service et affichage
// Les paramètres sont
// - le file descriptor du tube de communication en provenance du service
// - autre chose si nécessaire
static void receiveResult(int fdRead,  Data data)
{
    read(fdRead, &(data->ldest), sizeof(int));
    data->dest = (char *)malloc(sizeof(char) * data->ldest);
    read(fdRead, data->dest, sizeof(char) * data->ldest);
}

// ---------------------------------------------
// Fonction appelée par le main pour gérer la communications avec le service
// Les paramètres sont
// - les deux file descriptors des tubes nommés avec le service
// - argc et argv fournis en ligne de commande
// Cette fonction analyse argv et en déduit les données à envoyer
//    - argv[2] : la chaîne à compresser
void client_compression(int fdWrite, int fdRead, int argc, char * argv[])
{
    // variables locales éventuelles
    Data data = malloc(sizeof(struct DataP));
    int l;

    data->lstr = strlen(argv[2])-1;
    l = data->lstr;
    data->str = malloc(sizeof(char) * data->lstr);
    for(int i = 0; i<l-1; i++)
        data->str[i-1] = argv[2][i+1];
    data->str[l-1] = '\0';
    if (argc != 3)
        usage(argv[0], argv[1], "nombre d'arguments");
    sendData(fdWrite, data);
    receiveResult(fdRead, data);
}
