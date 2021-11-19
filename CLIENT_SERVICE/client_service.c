#include "memory.h"
#include "myassert.h"
#include "client_service.h"

#define OK_PWD "CORRECT PASSWORD"
#define WRONG_PWD "INCORRECT PASSWORD"



int getPWDFromClient(int fdclientToService)
{
      int pwd;
      read(fdclientToService, &pwd, sizeof(int));
}
