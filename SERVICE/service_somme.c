#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "orchestre_service.h"
#include "client_service.h"

#include "service_somme.h"

// définition éventuelle de types pour stocker les données
struct DataP{
    float a;
    float b;
    float somme;
};


typedef struct DataP * Data;

/*----------------------------------------------*
 * fonctions appelables par le service
 *----------------------------------------------*/

// fonction de réception des données
static void receiveData(int fdRead, Data data)
{
    read(fdRead, &(data->a), sizeof(float));
    read(fdRead, &(data->b), sizeof(float));
}

// fonction de traitement des données
static void computeResult(Data data)
{
    data->somme = data->a + data->b;
}

// fonction d'envoi du résultat
static void sendResult(int fdWrite, Data data)
{
    write(fdWrite, &(data->somme), sizeof(float));
}


/*----------------------------------------------*
 * fonction appelable par le main
 *----------------------------------------------*/
void service_somme(int fdClientToService, int fdServiceToClient)
{
    // initialisations diverses
    Data data = malloc(sizeof(struct DataP));

    receiveData(fdClientToService, data);
    computeResult(data);
    sendResult(fdServiceToClient);

    // libération éventuelle de ressources
    free(data);
}
