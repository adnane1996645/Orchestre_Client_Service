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
#include "../UTILS/memory.h"
#include "../UTILS/myassert.h"
#include "client_orchestre.h"
//#include <errno.h> //utile pour débugger


struct ComP{//pour la communication client_service
  int pwd; //motdepass envoyé par l'orchestre au client pour qu'il puisse valider ses communications avec le service
  char *pipe1, *pipe2; //nom des tubes par les quels vont transiter les comms entre le client et le service
};

void creat_named_pipe(){
  int ret = mkfifo("pipe_o2c",0641); //chemin relatif
  //printf("%s\n",strerror(errno));
  myassert(ret != -1, "Erreur dans la création du tube nommé o2c");
  
  ret = mkfifo("pipe_c2o",0641);
  //printf("%s 2\n",strerror(errno));
  myassert(ret != -1, "Erreur dans la création du tube nommé c2o");
}

int * open_pipes_c(){ //coté client
  int *fd;
  
  MY_MALLOC(fd, int, 2);
  fd[1] = open("pipe_c2o", O_WRONLY);//ecriture
  myassert(fd[1] != -1, "Erreur dans l'ouverture en ecriture coté client du tube nommé c2o");
  
  fd[0]= open("pipe_o2c", O_RDONLY);//lecture
  myassert(fd[0] != -1, "Erreur dans l'ouverture en lecture coté client du tube nommé o2c ");
  
  

  return fd;
}

int * open_pipes_o(){ //coté orchestre
  int *fd;
  
  MY_MALLOC(fd, int, 2);
  fd[0] = open("pipe_c2o", O_RDONLY);
  myassert(fd[0] != -1, "Erreur dans l'ouverture en lecture coté orchestre du tube nommé c2o");
  
  fd[1] = open("pipe_o2c", O_WRONLY);
  myassert(fd[1] != -1, "Erreur dans l'ouverture en ecriture coté orchestre du tube nommé o2c ");
  

  return fd;
}

void close_pipes(int *fd){ // un tableau de 2 entier dois etre passé en parametre
  int ret = close(fd[0]);
  myassert(ret != -1, "Erreur dans la fermeture du tube en lecture");
  ret = close(fd[1]);
  myassert(ret != -1, "Erreur dans la fermeture du tube en ecriture");
}


/////COMMUNICATION d'une structure/////

Com init_com(int num_service, int mdp){//initialisation communication services-client(o2c)(crée les tubes en meme temps)
  Com c;
  char *a;
  int ret;

  MY_MALLOC(c, struct ComP, 1);
  MY_MALLOC(a, char, 1);
  c->pwd = mdp;

  int len = strlen("../pipe_s2c_1");
  MY_MALLOC(c->pipe1, char, len+1);//len + 1 pour le \0
  MY_MALLOC(c->pipe2, char, len+1);
  c->pipe1 = "../pipe_c2s_";//on laisse une place de libre a la fin de la chaine pour le numéro du service
  c->pipe2 = "../pipe_s2c_";

  ret = sprintf(a, "%d", num_service);//convertion int en chaine
  myassert(ret >= 0, "Erreur lors de la convertion en chaine de caractere du numéro du service");

  strcat(c->pipe1, a);//on concatène pour former le nom du tube en entier
  strcat(c->pipe2, a);

  ret = mkfifo(c->pipe1, 0641);
  myassert(ret != -1, "Erreur dans la création du tube nommé pipe_c2s_<num>");
  ret = mkfifo(c->pipe2, 0641);
  myassert(ret != -1, "Erreur dans la création du tube nommé pipe_s2c_<num>");

  return c;
}


void send_com(int fdWrite, constCom c){
  int ret = write(fdWrite, &c, sizeof(int));
  myassert(ret != -1, "Erreur dans l'envoi des tubes et du mot de pass via la structure com");
}

Com rcv_com(int fdRead){//recevoir le nom des tube et mdp pour la com service client
  Com c;
  int ret = read(fdRead,&c,sizeof(int));
  myassert(ret != -1, "Erreur dans la réception des tubes et du mot de pass via la structure com");

  return c;
}

void destroy_com(Com *pself){
  Com self = *pself;

  MY_FREE(self);
  MY_FREE(self->pipe1);
  MY_FREE(self->pipe2);
}

int getPwd(constCom c){
  return c->pwd;
}

char * getPipe(constCom c, int n){
  if(n == 1)
    return c->pipe1;
  else if(n == 2)
    return c->pipe2;
  else return NULL;//exit();
}



////////requete et réponse pour la demande d'un service/////////

void send_request(int fdWrite, int service){ // coté client
  int ret = write(fdWrite, &service, sizeof(int));
  myassert(ret != -1, "Erreur dans l'envoi de la demande du client");
}

int rcv_request(int fdRead){ // coté orchestre
  int ask;
  int ret = read(fdRead,&ask,sizeof(int));
  myassert(ret != -1, "Erreur dans la réception de la demande du client ");

  return ask;
}

void send_reply(int fdWrite, bool r){ // coté orchestre
  int ret = write(fdWrite, &r, sizeof(bool));
  myassert(ret != -1, "Erreur dans l'envoi de la réponse de l'orchestre");
}

bool rcv_reply(int fdRead){ // coté client
  bool r;
  int ret = read(fdRead,&r,sizeof(bool));
  myassert(ret != -1, "Erreur dans la réception de la réponse de l'orchestre");

  return r;
}

void send_adc(int fdWrite){ // coté client
  int r = ADC;
  int ret = write(fdWrite, &r, sizeof(int));
  myassert(ret != -1, "Erreur dans l'envoi de l'accusé de réception");
}

void rcv_adc(int fdRead){ // coté orchestre
  int r;
  int ret = read(fdRead,&r,sizeof(int));
  myassert(ret != -1, "Erreur dans la réception de l'accusé de réception");


}



//////////mutex//////////

void creat_mutex(){//on ne retourne pas le semid car l'orchestre ne l'utilise pas
  key_t key = ftok(CLIENT_ORCHESTRE, PROJ_ID);
  //printf("%s\n",strerror(errno));
  myassert(key != -1, "Erreur dans la création de la clé pour la création du mutex ");

  int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0641);
  myassert(semid != -1, "Erreur dans la création du mutex");

  int ret = semctl(semid, 0, SETVAL, 1);//1 pour imiter un mutex
  myassert(ret != -1, "Erreur dans l'initialisation du mutex");

}

int recup_mutex(){//c'est une erreur d'appeler cette fonction si le mutex n'a pas été créé préalablement
  key_t key = ftok(CLIENT_ORCHESTRE, PROJ_ID);
  //printf("%s\n",strerror(errno));
  myassert(key != -1, "Erreur dans la création de la clé pour récuperer le mutex");

  int semid = semget(key, 1, 0); // 0 pour récupérer l'id d'un semaphore existant
  myassert(semid != -1, "Erreur dans la récupération du semid du mutex");

  return semid;
}

void v_mutex(int semid){
  struct sembuf op;
  op.sem_num = 0;
  op.sem_flg = 0;
  op.sem_op = 1;
  int ret = semop(semid, &op, 1);
  myassert(ret != -1, "Erreur lors du +1 sur le mutex");
}

void p_mutex(int semid){
  struct sembuf op;
  op.sem_num = 0;
  op.sem_flg = 0;
  op.sem_op = -1;
  int ret = semop(semid, &op, 1);
  myassert(ret != -1, "Erreur lors du -1 sur le mutex");
}

void destroy_mutex(int semid){
  int ret = semctl(semid, 0, IPC_RMID, 0);
  myassert(ret != -1, "Erreur dans la destruction du mutex");
}
