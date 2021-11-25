#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "orchestre_service.h"
#include "client_service.h"

#include "service_compression.h"

// définition éventuelle de types pour stocker les données
struct DataP
{
    char * chaine;
    char * compression;
};

typedef strcut DataP * Data;
/*----------------------------------------------*
 * fonctions appelables par le service
 *----------------------------------------------*/

// fonction de réception des données
static void receiveData(int fdClientToService, Data data)
{
    int longueur;
    read(fdClientToService, &longueur, sizeof(int));
    data->chaine = malloc(longueur * sizeof(char));
    read(fdClientToService, data->chaine, longueur * sizeof(char));
}

// fonction de traitement des données
static void computeResult(Data data)
{
    data->chaineResult = malloc(3*sizeof(char));
    int i = 0;
    char c = data->chaineResult[i];

    while (c!='\0')
    {
        
    }
}

// fonction d'envoi du résultat
static void sendResult(/* fd_pipe_to_client,*/ /* résultat */)
{
}


/*----------------------------------------------*
 * fonction appelable par le main
 *----------------------------------------------*/
void service_compression(/* fd tubes nommés avec un client */)
{
    // initialisations diverses

    receiveData(/* paramètres */);
    computeResult(/* paramètres */);
    sendResult(/* paramètres */);

    // libération éventuelle de ressources
}
