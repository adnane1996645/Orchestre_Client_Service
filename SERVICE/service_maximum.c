#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <float.h>

#include "orchestre_service.h"
#include "client_service.h"
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "service_maximum.h"

//#include "service_maximum.h"

#define NB_THREADS 3

// définition éventuelle de types pour stocker les données

struct DataP{
    float* TAB;
    int length;
    float max;
};


typedef struct DataP * Data;

/******* GESTION THREAD ********/

// structure pour passer les paramètres aux pthread
typedef struct
{
    float * max; // la valeur du max d'un des tableau
    float *arr; // la tableau i occupé par un thread
    int start; //indice de première case du tableau occupé par par le thread i
    int end;   //indice de la derniere case du tableau occupé par par le thread i
    pthread_mutex_t mux;
} ThreadData;


/***********************************************/
// Fonction support d'un thread
// Tous les threads lanceront cette fonction
void * routine(void * arg)
{
    ThreadData *data = (ThreadData *) arg;
    float max = -FLT_MAX;

    for (int i = data->start; i <= data->end; i++){
        if(data->arr[i] > max){
            max = data->arr[i];
        }
    }

    // utilisation d'un mutex pour éviter que deux
    // threads écrivent en même temps
    pthread_mutex_lock(&(data->mux));
    if (*(data->max) < max){
        (*(data->max)) = max;
    }
    pthread_mutex_unlock(&(data->mux));

    return NULL;
}

ThreadData* createThreadDatas(Data d)
{
    pthread_mutex_t mux = PTHREAD_MUTEX_INITIALIZER;
    ThreadData* datas = (ThreadData*)malloc(sizeof(ThreadData) * NB_THREADS);
    int intervalLength = d->length/NB_THREADS;
    for (int i = 0; i < NB_THREADS ; i++){
        datas[i].max = &(d->max) ;
        datas[i].arr = d->TAB;
        datas[i].start = i * intervalLength;
        printf("start %d\n", datas[i].start);
        datas[i].end = (i+1) * intervalLength -1;
        datas[i].mux = mux;
    }
    datas[NB_THREADS - 1].end += (d->length % NB_THREADS);
    printf("%d\n", datas[NB_THREADS - 1].end);
    return datas;
}

/***********************************************/

// fonction de traitement des données
static void computeResult(Data d)
{
    pthread_t tabId[NB_THREADS];
    ThreadData* datas = createThreadDatas(d);
    // lancement des threads
    for (int i = 0; i < NB_THREADS; i++)
    {
        int ret = pthread_create(&(tabId[i]), NULL, routine, &(datas[i]));
        assert(ret == 0);
    }
    // attente de la fin des threads
    for (int i = 0; i < NB_THREADS; i++)
    {
	      int ret = pthread_join(tabId[i], NULL);
        assert(ret == 0);
    }
    free(datas);
}

// fonction de réception des données
static void receiveData(int fdRead, Data data)
{
    data->max = data->TAB[0];
    read(fdRead, &(data->length), sizeof(float));
    data->TAB = malloc(sizeof(float)*data->length);
    for(int i = 0; i < data->length; i++)
          read(fdRead, data->TAB+i, sizeof(float));
}

// fonction d'envoi du résultat
static void sendResult(int fdWrite, float result)
{
    write(fdWrite, &result, sizeof(float));
}


/*----------------------------------------------*
 * fonction appelable par le main
 *----------------------------------------------*/
void service_maximum(int fdClientToService, int fdServiceToClient)
{
    // initialisations diverses
    Data data;
    printf("        Service Maximum ... :\n");
    MY_MALLOC(data, struct DataP, 1);
    receiveData(fdClientToService, data);
    computeResult(data);
    sendResult(fdServiceToClient, data->max);

    // libération éventuelle de ressources
    free(data);
}
