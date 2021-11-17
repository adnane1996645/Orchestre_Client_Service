#include "myassert.h"
#include <string.h>
#include "orchestre_service.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdbool.h>
#include "memory.h"
#define _XOPEN_SOURCE 700

//CECI UN TEST
struct OrderP
{
    bool isOk;
    int size;
    char * motpasse;
};

Order init_Order(bool isok, const char * motpasse)
{
    Order ret;

    ret=NULL;
    MY_MALLOC(ret, struct OrderP, 1);
    ret->isOk = isok;
    ret->size = strlen(motpasse)+1;
    if(ret->isOk)
    {
        MY_MALLOC(ret->motpasse, char, ret->size);
        strcpy(ret->motpasse, motpasse);
    }
    return ret;
}

bool isItOk(Order self)
{
    return self->isOk;
}

char * getMotpasse(Order self)
{
    return self->motpasse;
}

void destroy_Order(Order * pself)
{
    if((*pself)->isOk)
        MY_FREE((*pself)->motpasse);
    MY_FREE(*pself);
}

void OrderOrchestreToService(int fdWrite,  Order order) // ##true : ordre favorable - #false : ordre defavorable
{
    write(fdWrite, &(order->isOk), sizeof(int));
    if(order->isOk)
    {
        int size;

        size = order->size;
        write(fdWrite, &size, sizeof(int));
        write(fdWrite, order->motpasse, size*sizeof(char));
    }
    close(fdWrite);
}

Order getOrderFromOrchestre(int fdRead)
{
    Order order = NULL;

    MY_MALLOC(order, struct OrderP, 1);
    read(fdRead, &(order->isOk), sizeof(bool));
    if(order->isOk)
    {
        read(fdRead, &(order->size), sizeof(int));
        MY_MALLOC(order->motpasse, char, order->size);
        read(fdRead, &(order->motpasse), (order->size) * sizeof(char));
    }
    close(fdRead);
    return order;
}

int mysemget(int numService)
{
    int ret;
    key_t key;
    int pojid;

    pojid=1+numService;
    key = ftok(ORCHESTRE_SERVICE, pojid);
    myassert(key!=-1, "Erreur dans la création de la clé dans la fonction mysemget");
    ret = semget(key, 1, IPC_CREAT | IPC_EXCL | 0641);
    myassert(key!=-1, "Erreur dans la création du semaphore dans la fonction mysemget");
    semctl(ret, 0, SETVAL, 1);
    return ret;
}

void mysem_descre(int semId)
{
    int ret;

    struct sembuf opp = {0,-1,0};
    ret = semop(semId, &opp, 1);
    myassert(ret != -1, "Erreur dans la desc du semophore Orchestre service");
}

void mysem_incre(int semId)
{
    int ret;

    struct sembuf opp = {0,1,0};
    ret = semop(semId, &opp, 1);
    myassert(ret != -1, "Erreur dans la incre du semophore Orchestre service");
}

void mysem_destroy(int semId)
{
    int ret;

    ret = semctl(semId, -1, IPC_RMID);
    myassert(ret != -1, "Erreur dans la distruction du semophore Orchestre service");
}
