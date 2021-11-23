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

struct ComP{//pour la communication client_service
  int mdp; //motdepass envoyé par l'orchestre au client pour qu'il puisse valider ses communications avec le service
  char *tube1, *tube2; //nom des tubes par les quels vont transiter les comms entre le client et le service
};

void creat_named_pipes(){
  int ret = mkfifo("../pipe_o2c",0641); //chemin relatif
  assert(ret != -1);
  ret = mkfifo("../pipe_c2o",0641);
  assert(ret != -1);
}

void open_pipes_c(){ //coté client
  int fd = open("../pipe_o2c", O_RDONLY);
  assert(fd != -1);
  fd = open("../pipe_c2o", O_WRONLY);
  assert(fd != -1);
}

void open_pipes_o(){ //coté orchestre
  int fd = open("../pipe_c2o", O_RDONLY);
  assert(fd != -1);
  fd = open("../pipe_o2c", O_WRONLY);
  assert(fd != -1);
}


Com init_com(int num_service, int mdp){ //initialisation communication services-client (o2c)
  Com c;
  char a[1];
  int ret;
  
  MY_MALLOC(c, struct ComP, 1);
  c->mdp = mdp;
  
  int len = strlen("../pipe_s2c_1");
  MY_MALLOC(c->tube1, char, len+1);//len + 1 pour le \0
  MY_MALLOC(c->tube2, char, len+1);
  c->tube1 = "../pipe_c2s_";//on laisse une place de libre a la fin de la chaine pour le numéro du service
  c->tube2 = "../pipe_s2c_";

  sprintf(a, "%d", num_service);//convertion int en chaine
  
  strcat(c->tube1, a);//on concatène pour former le nom du tube en entier
  strcat(c->tube2, a);
  
  ret = mkfifo(c->tube1, 0641);
  assert(ret != -1);
  ret = mkfifo(c->tube2, 0641);
  assert(ret != -1);

  return c;
}


void send_com(int fdWrite, constCom c){
  int ret = write(fdWrite, &(c), sizeof(int));
  assert(ret != -1);
}

void destroy_com(Com *pself){
  Com self = *pself;
  
  MY_FREE(self);
  MY_FREE(self->tube1);
  MY_FREE(self->tube2);
  *pself = NULL;
}


void AskClientToOrchestre(int fdWrite, int service){
  AskServices ask;
  MY_MALLOC(ask, struct AskServicesP, 1);
  ask->isOk = false;
  ask->service = service;
  int ret = write(fdWrite, &(ask), sizeof(int));
  assert(ret != -1);
}

int getAskFromClient(int fdRead){
  int ask_services;
  int ret = read(fd,&ask_services,sizeof(int));
  assert(ret != -1);
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


////mutex////

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

void destroy_mutex(int semid){
  int ret = semctl(semid, 0, IPC_RMID, 0);
  assert(ret != -1);
}
