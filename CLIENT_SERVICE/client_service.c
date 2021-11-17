#include "memory.h"
#include "myassert.h"
#include "client_service.h"

#define OK_PWD "CORRECT PASSWORD"
#define WRONG_PWD "INCORRECT PASSWORD"



char * getPWDFromClient(int fdclientToService)
{
      char * mot[100];
      read(fdclientToService, mot, )
}
