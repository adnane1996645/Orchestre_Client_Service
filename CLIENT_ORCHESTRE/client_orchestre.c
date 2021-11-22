#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <assert.h>
#include "memory.h"
#include "client_orchestre.h"


struct AskServicesP{//demande effectué par un client à l'orchestre pour le service souhaité
  bool isOk;  //reponse de l'orchestre (true si l'orchestre accepte)
  int service; //numéro du service souhaité
};

struct ComP{//communication client_service
  int mdp; //motdepass envoyé par l'orchestre au client pour qu'il puisse valider ses communications avec le service
  char *tube1, *tube2; //nom des tubes par les quels vont transiter les comms entre le client et le service
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
  
  int ret = semctl(semid, 0, SETVAL, 1);//1 pour imiter un mutex
  assert(ret != -1);
  
  return semid;
}

void v_mutex(int semid){
  struct sembuf op;
  op.sem_num = 0;
  op.sem_flg = 0;
  op.sem_op = 1;
  int ret = semop(semid, &op, 1);
  assert(ret != -1);
}
  
void p_mutex(int semid){
  struct sembuf op;
  op.sem_num = 0;
  op.sem_flg = 0;
  op.sem_op = -1;
  int ret = semop(semid, &op, 1);
  assert(ret != -1);
}

AskServices init_Ask(bool isok, const int service){
  int ask_services;
  int ar = read(fd,&ask_services,sizeof(int));
  assert(ar != -1);
}

void AskClientToOrchestre(int fdWrite, int service){
  int ask_services;
  int ar = read(fd,&ask_services,sizeof(int));
  assert(ar != -1);
}

int getAskFromClient(int fdRead){
  int ask_services;
  int ar = read(fd,&ask_services,sizeof(int));
  assert(ar != -1);
}

int getService(AskServices self){
  return self->service;
}

bool getOk(AskServices self){
  return self->isOk;
}

void destroy_Ask(AskServices *pself){
  MY_FREE(*pself);
}


