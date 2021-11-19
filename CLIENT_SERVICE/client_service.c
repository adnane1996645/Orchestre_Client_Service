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
