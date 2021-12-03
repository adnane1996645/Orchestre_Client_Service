#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "client_service.h"
#include "client_somme.h"
#include <limits.h>


/*----------------------------------------------*
 * usage pour le client somme
 *----------------------------------------------*/

static void usage(const char *exeName, const char *numService, const char *message)
{
    fprintf(stderr, "Client somme de deux nombres\n");
    fprintf(stderr, "usage : %s %s <n1> <n2> <prefixe>\n", exeName, numService);
    fprintf(stderr, "        %s         : numéro du service\n", numService);
    fprintf(stderr, "        <n1>      : premier nombre à sommer\n");
    fprintf(stderr, "        <n2>      : deuxième nombre à sommer\n");
    fprintf(stderr, "        <prefixe> : chaîne à afficher avant le résultat\n");
    fprintf(stderr, "exemple d'appel :\n");
    fprintf(stderr, "    %s %s 22 33 \"le résultat est : \"\n", exeName, numService);
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}

/*----------------------------------------------*
 * fonction de vérification des paramètres
 *----------------------------------------------*/

struct DataP
{
    int a;
    int b;
    int somme;
};

typedef struct DataP * Data;

void client_somme_verifArgs(int argc, char * argv[])
{
    long int tmp1 = strtol(argv[2]);
    long int tmp2 = strtol(argv[3]);

    if (argc != 5)
        usage(argv[0], argv[1], "nombre d'arguments");
    // éventuellement d'autres tests
    int service = io_strToInt(argv[1]);
    myassert(service == 0, "Le numéro de service n'est pas le service de somme");
    myassert(tmp1 >= INT_MIN && tmp1 <= INT_MAX && tmp2 >= INT_MIN && tmp2 <= INT_MAX , "Erreur overflow int");
}


/*----------------------------------------------*
 * fonctions de communication avec le service
 *----------------------------------------------*/

// ---------------------------------------------
// fonction d'envoi des données du client au service
// Les paramètres sont
// - le file descriptor du tube de communication vers le service
// - les deux float dont on veut la somme
static void sendData(int fdWrite,  int entier1, int entier2)
{
    // envoi des deux nombres
    write(fdWrite, &entier1, sizeof(int));
    write(fdWrite, &entier2, sizeof(int));
}

// ---------------------------------------------
// fonction de réception des résultats en provenance du service et affichage
// Les paramètres sont
// - le file descriptor du tube de communication en provenance du service
// - le prefixe
// - autre chose si nécessaire
static void receiveResult(int fdRead, char * prefixe, Data data)
{
    // récupération de la somme
    // affichage du préfixe et du résultat
    read(fdRead, &(data->somme), sizeof(int));
    printf("%s %d", prefixe, data->somme);
    printf("\n");
}


// ---------------------------------------------
// Fonction appelée par le main pour gérer la communications avec le service
// Les paramètres sont
// - les deux file descriptors des tubes nommés avec le service
// - argc et argv fournis en ligne de commande
// Cette fonction analyse argv et en déduit les données à envoyer
//    - argv[2] : premier nombre
//    - argv[3] : deuxième nombre
//    - argv[4] : chaîne à afficher avant le résultat
void client_somme(int fdWrite, int fdRead, int argc, char * argv[])
{
    // variables locales éventuelles
    Data data;

    if (argc != 5)
        usage(argv[0], argv[1], "nombre d'arguments");
    MY_MALLOC(data, struct DataP, 1);
    data->a = io_strToInt(argv[2]);
    data->b = io_strToInt(argv[3]);
    sendData(fdWrite, data->a, data->b);
    receiveResult(fdRead, argv[4], data);
    MY_FREE(data);
}
