#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <assert.h>
#include "memory.h"
#include "client_orchestre.h"


struct AskServicesP{
  bool isOk;
  int Service;
};

void creat_named_pipes(){
  int ret = mkfifo("../pipe_o2c",0641); //chemin relatif
  assert(ret != -1);
  ret = mkfifo("../pipe_c2o",0641);
  assert(ret != -1);
}

int creat_mutex(){
  key_t key = ftok(CLIENT_ORCHESTRE, PROJ_ID);
  assert(key != -1);

  int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0641);
  assert(semid != -1);

  int ret = semctl(semid, 0, SETVAL, 1);
  assert(ret != -1);

  return semid;
}

int ask_services;
int ar = read(fd,&ask_services,sizeof(int));
assert(ar != -1);
