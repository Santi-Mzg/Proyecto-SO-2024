#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>

void *list_directory(void *arg) 
{
    int *fd = (int *) arg;
    
    struct dirent *dir;
    DIR *d = opendir("/etc");

    if (d == NULL)
    {
        perror("Error opening directory");
    }else{
        int i = 0;

        while ((dir = readdir(d)) != NULL && i < 10)
        {
            sleep(1);
            printf("%s\n", dir->d_name);
            i++;
        }
        closedir(d);
    }

    pthread_exit(NULL); 
}


void *count_to_ten(void *arg) {

    for (int i = 1; i <= 10; i++) {
        printf("%d\n", i);
        sleep(1);  
    }
    pthread_exit(NULL);  
}

int main() {
    pthread_t thread1, thread2;

    if (pthread_create(&thread1, NULL, list_directory, NULL) != 0) {
        perror("Error creating thread for listing directory");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&thread2, NULL, count_to_ten, NULL) != 0) {
        perror("Error creating thread for counting");
        exit(EXIT_FAILURE);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Main: Both threads have completed.\n\n");

    return 0;
}
