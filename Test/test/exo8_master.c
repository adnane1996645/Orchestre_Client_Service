// pour des problèmes de compilation
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "commun.h"
#include "myassert.h"

//-----------------------------------------------------------------
static void usage(const char *exeName, const char *message)
{
    fprintf(stderr, "usage : %s <nbre jetons>\n", exeName);
    fprintf(stderr, "        nombre de workers autorisés à entrer en SC\n");
    fprintf(stderr, "        le programme ne vérifie pas que le paramètre "
            "est un entier correctement constitué.\n");
    if (message != NULL)
        fprintf(stderr, "message : %s\n", message);
    exit(EXIT_FAILURE);
}


//-----------------------------------------------------------------
// Il faut créer le sémaphore avec une clé générée par ftok et le
// le fichier fourni dans exo8_comm.h
// Il faut être rigoureux : si le sémphore existe déjà, c'est une erreur
// et il ne faut pas oublier de l'initialiser en fonction du nombre
// de jetons
int mysemget()
{
  key_t key;
  int semId;
  int ret;

  key = ftok(ORCHESTRE_SERVICE, MY_KEY);
  assert(key != -1);
  semId = semget(key, 1, IPC_CREAT | IPC_EXCL | 0641);
  assert(semId != -1);
  // 2e param : numéro sémaphore ; 4e param : valeur du sémaphore
  ret = semctl(semId, 0, SETVAL, 0);
  assert(ret != -1);

  return semId;
}

void mysem_destroy(int semId)
{
    int ret;

    ret = semctl(semId, -1, IPC_RMID);
    myassert(ret != -1, "Erreur dans la distruction du semophore Orchestre service");
}

//-----------------------------------------------------------------
int main(int argc, char * argv[])
{
    if (argc != 2)
        usage(argv[0], "nombre de paramètres incorrect");

    int nbreTokens;
    int semId;

    nbreTokens = strtol(argv[1], NULL, 10);
    if (nbreTokens < 1)
        usage(argv[0], "nbre tokens incorrect");

    // création du sémaphore
    semId = mysemget();

    printf("Le sémaphore est crée et vous pouvez lancer les workers\n");
    printf("Lorsque tous les workers ont terminé, appuyez sur <Entrée>\n");

    printf("--> ");
    getchar();

    // destruction du sémaphore
    // si vous êtes curieux, faites-le pendant que les workers l'utilisent
    mysem_destroy(semId);

    printf("Fin du master\n");

    return EXIT_SUCCESS;
}
