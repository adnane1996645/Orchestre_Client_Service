#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "orchestre_service.h"
#include "client_service.h"

#include "service_compression.h"

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
    data->str = (char *)malloc(sizeof(char) * data->lstr);
      read(fdRead, data->str, sizeof(char) * data->lstr);
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

    /*for(int j=0; j<data->lstr-1; j++)
        printf("%d|%c\n", j, data->str[j]);*/
    data->dest =malloc(sizeof(char));
    while(currChar != '\0')
    {
        for(int i=debut; i<longueur; i++)
        {
            if(currChar==data->str[i])
            {
                occChar++;
            }
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
    write(fdWrite, data->dest, sizeof(char) * data->ldest);
}


/*----------------------------------------------*
 * fonction appelable par le main
 *----------------------------------------------*/
void service_compression(int fdRead, int fdWrite)
{
    // initialisations diverses
    Data data = malloc(sizeof(struct DataP));
    printf("        Service Compression ... :\n");
    receiveData(fdRead, data);
    printf("%s\n", data->str);
    computeResult(data);
    printf("%s\n", data->dest);
    sendResult(fdWrite, data);
    // libération éventuelle de ressources
    //printf("%s\n",data->dest);
    free(data->str);
    free(data->dest);
    free(data);
}
/*int main()
{*/
    /*char * chaine = "cccchhhhttatttttt";
    int fd = open("info", O_CREAT | O_WRONLY, 0644);
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
    int fd2 = open("info2", O_CREAT | O_WRONLY, 0644);
    service_compression(fd1, fd2);
    close(fd1);
    close(fd2);

    int fd3 = open("info2", O_RDONLY, 0644);
    int l1;
    read(fd3, &l1, sizeof(int));
    char * chaine1 = malloc(sizeof(char) * l1);
    read(fd3, chaine1, sizeof(char) * l1);
    close(fd3);

    printf("%s  %s\n",chaine, chaine1);*/
    //Data data = malloc(sizeof(struct DataP));


    /*
    char * chaine = "cchhhhttatttttt";
    int fd = open("info", O_CREAT | O_WRONLY, 0644);
    int l = strlen(chaine)+1;
    assert(fd !=-1);
    write(fd, &l, sizeof(int));
    write(fd, chaine, sizeof(char) * l);
    close(fd);
    int fd1 = open("info", O_RDONLY, 0644);
    int fd2 = open("info2", O_CREAT | O_WRONLY, 0644);
    service_compression(fd1, fd2);
    // libération éventuelle de ressources
    //printf("%s\n",data->dest);
    close(fd1);
    close(fd2);
    int fd3 = open("info2", O_RDONLY, 0644);
    int l3;
    read(fd3, &l3, sizeof(int));
    char * c3 = malloc(sizeof(char) * l3);
    read(fd3, c3, sizeof(char) * l3);
    printf("%s %d\n", c3, l3);
    free(c3);
    return EXIT_SUCCESS;
}*/
