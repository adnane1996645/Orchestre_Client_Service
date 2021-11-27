#include "memory.h"
#include "myassert.h"
#include "client_service.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



int openTubeWrite(char * nomTube, char * Err)
{
    int fd;
    fd = open(nomTube, O_WRONLY, 0644);
    myasset(fd != -1, Err);
    return fd;
}

int openTubeRead(char * nomTube, char * Err)
{
    int fd;
    fd = open(nomTube, O_RDONLY, 0644);
    myasset(fd != -1, Err);
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
     if(isOk)
          write(fdWrite, &OK_PWD, sizeof(int));
     else
          write(fdWrite, &WRONG_PWD, sizeof(int));
}

void sendChaineToService(char * chaine, int fdWrite)
{
    int length = strlen(chaine)+1;
    write(fdWritre, &length, sizeof(int));
    write(fdWrite, chaine, length * sizeof(char));
}

char * getChainefromService(int fdRead)
{
    int length;
    read(fdRead, &length, sizeof(int));
    char * chaineResult = (char *) malloc(length * sizeof(char));
    read(fdWrite, chaineResult, length * sizeof(char));
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
    myassert(diff == 0; "Il y a une erreur quelque part, le client n'a pas envoyé l'accusé\n");
    printf("||\n||=>Accusé de réception est bien recus de la part du client.");
    free(acr);
}
