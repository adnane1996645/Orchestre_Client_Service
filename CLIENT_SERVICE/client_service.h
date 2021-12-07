#ifndef CLIENT_SERVICE_H
#define CLIENT_SERVICE_H

// Ici toutes les communications entre les services et les clients :
// - les deux tubes nommés pour la communication bidirectionnelle

#define OK_PWD "CORRECT PASSWORD"
#define WRONG_PWD "INCORRECT PASSWORD"
#define ACR "RESULT OK"

typedef struct Service_ComP * Service_Com;

Service_Com init_Service(int s);
void destroy_service(Service_Com * pself);
char * create_pipeS2C(Service_Com service);
char * create_pipeC2S(Service_Com Service);
int openTubeWrite(char * nomTube, char * Err);
int openTubeRead(char * nomTube, char * Err);
bool getPWDFromClient(int fdRead, int PwD);
void sendReponsePWD(int fdWrite, bool isOK);
void sendChaineToService(char * chaine, int fdWrite);
char * getChainefromService(int fdRead);
void sendACR(int fdWrite);
void getACR(int fdRead);

/*
bool getPWDFromClient(int fdClientToService);
void sendReponsePWD(int fdSeviceToClient, bool isOK);*/


#endif
