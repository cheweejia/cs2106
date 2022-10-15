#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int userInput, childPid, childResult;
    //Since largest number is 10 digits, a 12 characters string is more
    //than enough
    char cStringExample[12];
    printf("\nEnter a number:");
    scanf("%d", &userInput);

    int pid = fork();
    if (pid != 0) {
        childPid = fork();
        // Parent process
        if (childPid != 0 ) {
            // Wait for the return result from the child process and store in childResult
            wait( &childResult);
            printf("%d has %d prime factors\n", userInput, childResult >> 8);
        } // Child process
        else {
            //Easy way to convert a number into a string
            sprintf(cStringExample, "%d", userInput);
            execl("./PF", "PF", cStringExample, NULL);
        }
    } else {
        if (userInput > 0) {
            execl("./a.out", "a.out", NULL, NULL);
        }
    }

}
