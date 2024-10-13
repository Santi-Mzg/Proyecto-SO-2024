#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

int main()
{
    struct dirent *dir;
    DIR *d = opendir("/etc");

    if (d == NULL)
    {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    if (d)
    {
        int i = 0;

        while ((dir = readdir(d)) != NULL && i < 10)
        {
            sleep(1);
            printf("%s\n", dir->d_name);
            i++;
        }
        closedir(d);
    }

    return (0);
}
