#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <assert.h>
#include "../UTILS/memory.h"
#include "client_orchestre.h"

/*
struct AskServicesP{//demande effectué par un client à l'orchestre pour le service souhaité
  bool isOk;  //reponse de l'orchestre (true si l'orchestre accepte)
  int service; //numéro du service souhaité
};
*/
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

int * open_pipes_c(){ //coté client
  int *fd;
  MY_MALLOC(fd, int, 2);
  fd[0]= open("../pipe_o2c", O_RDONLY);//lecture
  assert(fd[0] != -1);
  fd[1] = open("../pipe_c2o", O_WRONLY);//ecriture
  assert(fd[1] != -1);

  return fd;
}

int * open_pipes_o(){ //coté orchestre
  int *fd;
  MY_MALLOC(fd, int, 2);
  fd[0] = open("../pipe_c2o", O_RDONLY);
  assert(fd[0] != -1);
  fd[1] = open("../pipe_o2c", O_WRONLY);
  assert(fd[1] != -1);

  return fd;
}

void close_pipes(int *fd){ // un tableau de 2 entier dois etre passé en parametre
  int ret = close(fd[0]);
  assert(ret != -1);
  ret = close(fd[1]);
  assert(ret != -1);
}


Com init_com(int num_service, int mdp){//initialisation communication services-client(o2c)(crée les tubes en meme temps)
  Com c;
  char *a;
  int ret;
  
  MY_MALLOC(c, struct ComP, 1);
  MY_MALLOC(a, char, 1);
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

Com rcv_com(int fdRead){//recevoir le nom des tube et mdp pour la com service client
  Com c;
  int ret = read(fdRead,&c,sizeof(int));
  assert(ret != -1);

  return c;
}

void destroy_com(Com *pself){
  Com self = *pself;
  
  MY_FREE(self);
  MY_FREE(self->tube1);
  MY_FREE(self->tube2);
}

int getPwd(constCom c){
  return c->mdp;
}

char * getPipe(constCom c, int n){
  if(n == 1)
    return c->tube1;
  else if(n == 2)
    return c->tube2;
  else return NULL;//exit();
}


////////requete et réponse pour la demande d'un service/////////

void send_request(int fdWrite, int service){ // coté client
  int ret = write(fdWrite, &(service), sizeof(int));
  assert(ret != -1);
}

int rcv_request(int fdRead){ // coté orchestre
  int ask;
  int ret = read(fdRead,&ask,sizeof(int));
  assert(ret != -1);

  return ask;
}

void send_reply(int fdWrite, bool r){ // coté orchestre
  int ret = write(fdWrite, &(r), sizeof(bool));
  assert(ret != -1);
}

bool rcv_reply(int fdRead){ // coté client
  int r;
  int ret = read(fdRead,&r,sizeof(bool));
  assert(ret != -1);

  return r;
}

//probablement inutile donc voir pour suppression
/*
void send_Ask(int fdWrite, int service){
  AskServices ask;
  MY_MALLOC(ask, struct AskServicesP, 1);
  ask->isOk = false;
  ask->service = service;
  int ret = write(fdWrite, &(ask), sizeof(int));
  assert(ret != -1);
}

AskServices getAskFromClient(int fdRead){
  AskServices ask_services;
  int ret = read(fd,&ask_services,sizeof(int));
  assert(ret != -1);

  return ask_services;
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
*/

//////////mutex//////////

int creat_mutex(){
  key_t key = ftok(CLIENT_ORCHESTRE, PROJ_ID);
  assert(key != -1);
  
  int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0641);
  assert(semid != -1);
  
  int ret = semctl(semid, 0, SETVAL, 1);//1 pour imiter un mutex
  assert(ret != -1);
  
  return semid;
}

int recup_mutex(){//c'est une erreur d'appeler cette fonction si le mutex n'a pas été créé préalablement
  key_t key = ftok(CLIENT_ORCHESTRE, PROJ_ID);
  assert(key != -1);
  
  int semid = semget(key, 1, 0); // 0 pour récupérer l'id d'un semaphore existant
  assert(semid != -1);
  
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
