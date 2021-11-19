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


struct OrderP
{
    bool isOk;
    int motpasse;
};

Order init_Order(bool isok, const int motpasse)
{
    Order ret;

    ret=NULL;
    MY_MALLOC(ret, struct OrderP, 1);
    ret->isOk = isok;
    if(ret->isOk)
        ret->motpasse=motpasse;
    return ret;
}

bool isItOk(Order self)
{
    return self->isOk;
}

int getMotpasse(Order self)
{
    return self->motpasse;
}

void destroy_Order(Order * pself)
{
    MY_FREE(*pself);
}

void OrderOrchestreToService(int fdWrite,  Order order) // ##true : ordre favorable - #false : ordre defavorable
{
    write(fdWrite, &(order->isOk), sizeof(int));
    if(order->isOk)
        write(fdWrite, order->motpasse, sizeof(int));
    close(fdWrite);
}

Order getOrderFromOrchestre(int fdRead)
{
    Order order = NULL;

    MY_MALLOC(order, struct OrderP, 1);
    read(fdRead, &(order->isOk), sizeof(bool));
    if(order->isOk)
        read(fdRead, &(order->motpasse), sizeof(int));
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
