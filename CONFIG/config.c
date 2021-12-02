/*
 * Indications (à respecter) :
 * - Les erreurs sont gérées avec des assert ; les erreurs traitées sont :
 *    . appel trop tôt ou trop tard d'une méthode (cf. config.h)
 *    . fichier de configuration inaccessible
 *    . une position erronée
 * - Le fichier (si on arrive à l'ouvrir) est considéré comme bien
 *   formé sans qu'il soit nécessaire de le vérifier
 *
 * Un code minimal est fourni et permet d'utiliser le module "config" dès
 * le début du projet ; il faudra le remplacer par l'utilisation du fichier
 * de configuration.
 * Il est inutile de faire plus que ce qui est demandé
 *
 * Dans cette partie vous avez le droit d'utiliser les entrées-sorties
 * de haut niveau (fopen, fgets, ...)
 */


// TODO include des .h système
#include <stdlib.h>
#include <stdio.h>
#include "myassert.h"
#include "config.h"
#include "memory.h"
#include <stdbool.h>
#include <string.h>

#define LONGUEUR_exeName 16



// TODO Définition des données ici
struct Config
{
    int NB_Services;
    char * exeName;
    bool * open;
    //int * Num_services;
    bool configInit_is_notCalled;
    bool configExit_is_notCalled;

};

struct Config config = {0,NULL,NULL,true,true};

void config_init(const char *filename)
{
     int nb_services;
     FILE * fd;

    // TODO erreur si la fonction est appelée deux fois
    myassert(config.configInit_is_notCalled,"La fonction config_init est appelée plus qu'une fois!");
    config.configInit_is_notCalled=false;
    // TODO code vide par défaut, à remplacer
    //      il faut lire le fichier et stocker toutes les informations en
    //      mémoire
    fd = fopen(filename, "r");
    fscanf(fd,"%d\n",&(config.NB_Services));
    nb_services=config.NB_Services;
    MY_MALLOC(config.exeName, char , LONGUEUR_exeName);
    fscanf(fd,"%s\n",config.exeName);
    MY_MALLOC(config.open, bool, nb_services); //allocation pour un tableau d'etat d'un service
    //MY_MALLOC(config.Num_services, int , nb_services);
    for(int i =0; i< nb_services; i++)
    {
          char service[10];
          int s;
          fscanf(fd,"%d ",&s);
          fscanf(fd,"%s\n",service);
          config.open[i] = (strcmp(service,"ouvert") == 0);
    }
    fclose(fd);
}

void config_exit()
{
    config.configExit_is_notCalled=false;
    // TODO erreur si la fonction est appelée avant config_init
    myassert(!config.configInit_is_notCalled,"La fonction config_exit est appelée avant config_init!");
    // TODO code vide par défaut, à remplacer
    //      libération des ressources
    free(config.open);
    //free(config.Num_services);
    free(config.exeName);
}

int config_getNbServices()
{
    // erreur si la fonction est appelée avant config_init
    myassert(!config.configInit_is_notCalled,"La fonction config_getNbServices est appelée avant config_init!");
    // erreur si la fonction est appelée après config_exit
    myassert(config.configExit_is_notCalled,"La fonction config_getNbServices est appelée après config_exit!");
    // code par défaut, à remplacer
    return config.NB_Services;
}

const char * config_getExeName()
{
    // TODO erreur si la fonction est appelée avant config_init
     myassert(!config.configInit_is_notCalled,"La fonction config_getExeName est appelée avant config_init!");
    // TODO erreur si la fonction est appelée après config_exit
    myassert(config.configExit_is_notCalled,"La fonction config_getExeName est appelée après config_exit!");
    // TODO code par défaut, à remplacer
    return (const char *)config.exeName;
}

bool config_isServiceOpen(int pos)
{
    int nb = config_getNbServices();
    // TODO erreur si la fonction est appelée avant config_init
    myassert(!config.configInit_is_notCalled,"La fonction config_isServiceOpen est appelée avant config_init!");
    // TODO erreur si la fonction est appelée après config_exit
    myassert(config.configExit_is_notCalled,"La fonction config_isServiceOpen est appelée après config_exit!");
    // TODO erreur si "pos" est incorrect
    myassert(pos>=0 && pos<nb,"Numéro du service est invalide!");
    // TODO code par défaut, à remplacer
    return config.open[pos];
}
