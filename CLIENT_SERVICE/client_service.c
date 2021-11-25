#include "memory.h"
#include "myassert.h"
#include "client_service.h"



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
