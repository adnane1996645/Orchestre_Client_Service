#ifndef ORCHESTRE_SERVICE_H
#define ORCHESTRE_SERVICE_H

// Ici toutes les communications entre l'orchestre et les services :
// - le tube anonyme pour que l'orchestre envoie des données au service
// - le sémaphore pour que  le service indique à l'orchestre la fin
//   d'un traitement
#include <stdbool.h>

#define ORCHESTRE_SERVICE "orchestre_service.h"

typedef struct OrderP * Order;
int mysemget();
void mysem_descre(int semId); // desincremente la valeur du semaphore par 1
void mysem_incre(int semId); // incremente la valeur du semaphore par 1
void mysem_destroy(int semId); // détruire le sémaphore
Order init_Order(bool isok, const int motpasse);
bool isItOk(Order self);
int getMotpasse(Order self);
void destroy_Order(Order * pself);
void OrderOrchestreToService(int fdWrite,  Order order);
Order getOrderFromOrchestre(int fdRead);


#endif
