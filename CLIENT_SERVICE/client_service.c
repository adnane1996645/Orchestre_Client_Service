#define _XOPEN_SOURCE 700
#include "../UTILS/memory.h"
#include "../UTILS/myassert.h"
#include "client_service.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*struct Com
{
    int fdServiceToClient;
    int fdClientToService;
    //int Service;
};

Com com = {-2,-1,-1};

void init_com(int service)
{
    com.service = service;
}*/

struct Service_ComP
{
    int Service;
    char * pipeS2C;
    char * pipeC2S;
};

Service_Com init_Service(int s)
{
    Service_Com service;

    char * name1 = "../pipe_s2c_";
    char * name2 = "../pipe_c2s_";
    MY_MALLOC(service, struct Service_ComP, 1);
    MY_MALLOC(service->pipeS2C, char, strlen(name1)+2);
    MY_MALLOC(service->pipeC2S, char, strlen(name2)+2);
    service->Service = s;
    sprintf(service->pipeS2C, "%s%d", name1, s);
    sprintf(service->pipeC2S, "%s%d", name2, s);
    return service;
}

void destroy_service(Service_Com * pself)
{
    free((*pself)->pipeS2C);
    free((*pself)->pipeC2S);
    free(*pself);
    *pself = NULL;
}

char * create_pipeS2C(Service_Com service)
{
    int ret;

    ret = mkfifo(service->pipeS2C, 0641);
    myassert(ret == 0, "Pas de probleme dans la creation du tube");
    return service->pipeS2C;
}

char * create_pipeC2S(Service_Com Service)
{
    int ret;

    ret = mkfifo(Service->pipeC2S, 0641);
    myassert(ret == 0, "Pas de probleme dans la creation du tube");
    return Service->pipeC2S;
}

int openTubeWrite(char * nomTube, char * Err)
{
    int fd;

    fd = open(nomTube, O_WRONLY, 0644);
    myassert(fd != -1, Err);
    return fd;
}

int openTubeRead(char * nomTube, char * Err)
{
    int fd;

    fd = open(nomTube, O_RDONLY, 0644);
    myassert(fd != -1, Err);
    return fd;
}

bool getPWDFromClient(int fdRead, int PwD)
{
      int pwd;
      read(fdRead, &pwd, sizeof(int));
      return (pwd==PwD);
}

void sendReponsePWD(int fdWrite, bool isOK)
{
     if(isOK)
          write(fdWrite, &OK_PWD, sizeof(int));
     else
          write(fdWrite, &WRONG_PWD, sizeof(int));
}

void sendChaineToService(char * chaine, int fdWrite)
{
    int length = strlen(chaine)+1;
    write(fdWrite, &length, sizeof(int));
    write(fdWrite, chaine, length * sizeof(char));
}

char * getChainefromService(int fdRead)
{
    int length;
    read(fdRead, &length, sizeof(int));
    char * chaineResult = (char *) malloc(length * sizeof(char));
    read(fdRead, chaineResult, length * sizeof(char));
    return chaineResult;
}

void sendACR(int fdWrite)
{
    int l = strlen(ACR)+1;
    write(fdWrite, &l, sizeof(int));
    write(fdWrite, ACR, sizeof(char) * l);
}

void getACR(int fdRead)
{
    int l;
    char * acr;
    int diff;
    read(fdRead, &l, sizeof(int));
    acr = malloc(sizeof(char) * l);
    read(fdRead, acr, sizeof(char) * l);
    diff = strcmp(acr, ACR);
    myassert(diff == 0, "Il y a une erreur quelque part, le client n'a pas envoyé l'accusé\n");
    printf("||\n||=>Accusé de réception est bien recus au sens du client.");
    free(acr);
}
