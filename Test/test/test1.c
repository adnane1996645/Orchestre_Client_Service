#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       #include <unistd.h>


void lire(int * var, int fd)
{
    read(fd, var, sizeof(int));
}

int main()
{
    int * var=NULL;
    int b = 3;

    int fd = open("toto", O_CREAT | O_RDWR);
    write(fd, &b, 4);
    lire(var, fd);
    printf("%d\n", *var);
    close(fd);
    return EXIT_SUCCESS;
}
