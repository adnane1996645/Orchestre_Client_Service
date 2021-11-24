#ifndef CLIENT_ORCHESTRE_H
#define CLIENT_ORCHESTRE_H

// Ici toutes les communications entre l'orchestre et les clients :
// - le sémaphore pour que 2 clients ne conversent pas en même temps avec l'orchestre
// - les deux tubes nommés pour la communication bidirectionnelle
#include <stdbool.h>
// fichier choisi pour l'identification du sémaphore
#define CLIENT_ORCHESTRE "client_orchestre.h"

// identifiant pour le deuxième paramètre de ftok
#define PROJ_ID 5

struct AskServicesP;
typedef struct AskServicesP *AskServices;
typedef const struct AskServicesP *constAskServices;

struct ComP;
typedef struct ComP *Com;
typedef const struct ComP *constCom;

void creat_named_pipe();//c'est l'orchestre qui creer
int * open_pipes_c();//coté client
int * open_pipes_o();//coté orchestre
void close_pipes(int *fd);

Com init_com(int num_service, int mdp);//crée la structure contenant le nom des tube et le mdp à envoyer au client 
void send_com(constCom c);
Com rcv_com(int fdRead);
void destroy_com(Com *pself);
int getPwd(constCom c);
char * getPipe(constCom c, int n);

void AskClientToOrchestre(int fdWrite, int service);
AskServices getAskFromClient(int fdRead);
bool getOk(AskServices self);
int getService(AskServices self);
void destroy_Ask(AskServices *pself);

int creat_mutex();
void v_mutex(int semid);
void p_mutex(int semid);
void destroy_mutex(int semid);

#endif
