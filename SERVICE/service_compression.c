#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
/*#include "orchestre_service.h"
#include "client_service.h"

#include "service_compression.h"*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
// définition éventuelle de types pour stocker les données
struct DataP
{
    char * str;
    int lstr;
    char * dest;
    int ldest;
};

typedef struct DataP * Data;
/*----------------------------------------------*
 * fonctions appelables par le service
 *----------------------------------------------*/

// fonction de réception des données
static void receiveData(int fdRead, Data data)
{
    read(fdRead, &(data->lstr), sizeof(int));
    data->str = malloc((data->lstr) * sizeof(char));
    for(int i =0; i< data->lstr; i++)
      read(fdRead, data->str+i, sizeof(char));
    printf("receiveData");
}

// fonction de traitement des données
static void computeResult(Data data)
{
    int longueur = strlen(data->str)+1;
    int occChar = 1;
    char currChar = data->str[0];
    int length = 1;
    int debut = 1;
    int iter = 0;

    data->dest =malloc(sizeof(char));
    while(currChar != '\0')
    {
        for(int i=debut; i<longueur; i++)
        {
            if(currChar==data->str[i])
                  occChar++;
            else
            {
                length+=2;
                data->dest = realloc(data->dest, length * sizeof(char));
                data->dest[iter] = occChar + '0';
                data->dest[iter+1] = data->str[i-1];
                data->dest[iter+2] = '\0';
                iter+=2;
                currChar = data->str[i];
                debut = i+1;
                break;
            }
        }
        occChar=1;
    }
    data->ldest = strlen(data->dest)+1;
}

// fonction d'envoi du résultat
static void sendResult(int fdWrite, Data data)
{
    write(fdWrite, &(data->ldest), sizeof(int));
    for(int i =0; i<data->ldest; i++)
        write(fdWrite, data->dest+i, sizeof(char));
}


/*----------------------------------------------*
 * fonction appelable par le main
 *----------------------------------------------*/
void service_compression(int fdRead, int fdWrite)
{
    // initialisations diverses
    Data data = malloc(sizeof(struct DataP));
    receiveData(fdRead, data);
    computeResult(data);
    sendResult(fdWrite, data);
    // libération éventuelle de ressources
    printf("%s\n",data->dest);
    free(data->str);
    free(data->dest);
    free(data);
}
/*int main()
{
    char * chaine = "cccccchhhhttatttttt";
    int fd = open("info", O_CREAT | O_RDWR, 0644);
    int ret=0;
    int l = strlen(chaine)+1;
    assert(fd !=-1);
    write(fd, &l, sizeof(char));
    for(int i =0; i<l; i++)
    {
         write(fd, chaine+i, sizeof(char));
    }
    close(fd);
    int fd1 = open("info", O_RDONLY, 0644);
    Data data = malloc(sizeof(struct DataP));
    receiveData(fd1, data);
    printf("%s\n",data->str);
    close(fd1);
    computeResult(data);
    int fd2 = open("info", O_WRONLY, 0644);
    sendResult(fd2, data);
    close(fd2);
    // libération éventuelle de ressources
    //printf("%s\n",data->dest);
    free(data->str);
    free(data->dest);
    free(data);*/

    /*fd = open("info", O_CREAT | O_RDWR, 0644);
    int longueur;
    read(fd, &longueur, sizeof(int));
    char * c = malloc(longueur*sizeof(char));
    for(int i =0; i<longueur; i++)
      read(fd, c+i, sizeof(char));
    printf("%s\n", c);
    close(fd);*/

    /*Data data = malloc(sizeof(struct DataP));
    data->str = "cccccchhhhttatttttt";
    data->lstr = strlen(data->str)+1;
    computeResult(data);
    printf("%s %d\n", data->dest, data->ldest);*/

    return EXIT_SUCCESS;
}
