//
//  signals.c
//
//
//  Created by Swetul R. Patel on 2020-03-08.
//
//

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

//int handlerAvail = true;
int currentSIGNAL;

void myHandler(int sig)
{
    if(sig == SIGINT)
    {
        printf("Signal SIGINT received");
        for(int i = 0; i < 5; i++)
        {
            printf("%d - ", i);
        }
    }

    if(sig == SIGUSR2)
    {
        printf("Signal SIGUSR2 received");
    }

    else{
            printf("Handler unavailable, Signal waiting");
        }
    exit(EXIT_SUCCESS);

}


int main()
{

    struct sigaction temp;
    temp.sa_handler = myHandler;


    pid_t pid;


    pid = fork();

    if(pid == 0)
    {

        write(STDOUT_FILENO, "c", 1);
        sigaction(SIGINT, &temp, NULL);
        while(1)
        {

            printf("CH -");
            fflush(stdout);
            sleep(1);
        }


    }
    else{


        printf("PA - ");



        sleep(4);
        kill(pid, SIGINT);
        kill(pid, SIGINT);
        sleep(10);
        for(int j= 0; j < 10; j++)
        {
            kill(pid, SIGINT);
            printf("*signal sent by me* \n");
        }
    }


    return EXIT_SUCCESS;


}




