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

void sendPWD(int fdWrite, int pwd)
{
    read(fdWrite, &pwd, sizeof(int));
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
     {
        int l = strlen(OK_PWD)+1;
        write(fdWrite, &l, sizeof(int));
        write(fdWrite, OK_PWD, sizeof(char) * l);
     }
     else
     {
        int l = strlen(WRONG_PWD) +1;
        write(fdWrite, &l, sizeof(int));
        write(fdWrite, WRONG_PWD, sizeof(char) * l);
     }
}

char * getReponsePWD(int fdWrite)
{
    int l;
    char * rsp;
    read(fdWrite, &l, sizeof(int));
    rsp = malloc(sizeof(char) * l);
    read(fdWrite, rsp, sizeof(char) * l);
    return rsp;
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
