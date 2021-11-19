#ifndef CLIENT_SERVICE_H
#define CLIENT_SERVICE_H

// Ici toutes les communications entre les services et les clients :
// - les deux tubes nommés pour la communication bidirectionnelle

#define OK_PWD "CORRECT PASSWORD"
#define WRONG_PWD "INCORRECT PASSWORD"

bool getPWDFromClient(int fdClientToService);
void sendReponsePWD(int fdSeviceToClient, bool isOK);


#endif
