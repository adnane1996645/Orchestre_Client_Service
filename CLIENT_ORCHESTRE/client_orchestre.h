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
Com init_Com(int num_service, int mdp);

AskServices init_Ask(bool isok, const int service)
void AskClientToOrchestre(int fdWrite, int service);
int getAskFromClient(int fdRead);
bool getOk(AskServices self);
int getService(AskServices self);
void destroy_Ask(AskServices *pself);

int creat_mutex();
void v_mutex(int semid);
void p_mutex(int semid);

#endif
