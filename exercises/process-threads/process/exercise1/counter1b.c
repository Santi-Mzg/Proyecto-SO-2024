#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{

    for (int i = 1; i <= 10; i++)
    {
        printf("%d\n", i);
        sleep(2);
    }
    exit(EXIT_SUCCESS);
}
