//
//  pipes.c
//  Created by Swetul R. Patel on 2020-03-06.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int state = SIGUSR2;
char temp[50];
int pos = 0;

/*
 As the name suggests man in the middle attack involves intercepting and possibly altering a message not meant to be
 received by anyother person other than the recepient, here the parent process is the man in the middle.
 MAN IN THE MIDDLE- changes every instance of cat with dog and dog with cat when the correct signals are sent to it, it will not change the message if the incorect signal has been sent i.e change cat to dog signal when input was dog,

 */


void middleMan(char input[], int state)
{
    char in[50];
    if(state == SIGUSR2)//if signal is SIGUSR1 convert any instance of cat to dog in the input
    {
        for(int i = 0; i < pos-2; i++)
        {
            //("%d, %c %d\n", state, input[i], pos-2);

            char cat[3] = {'c','a','t'};
            strncpy(in, &input[i], strlen(input));
            for(int j = 0; j< pos-2; j++)
            {
                if(cat[0] == in[j])
                {
                    if(cat[1] == in[j+1])
                    {
                        if(cat[2] == in[j+2])
                        {
                            input[j] = 100;
                            input[j+1] = 111;
                            input[j+2] = 103;
                            i+=2;
                        }
                    }
                }
            }

        }
    }//end of state


    if(state == SIGUSR1)//if signal is SIGUSR1 convert any instance of dog to cat in the input
    {
        for(int i = 0; i < pos-2; i++)
        {
         //comparing string to find dog
            char cat[3] = {'d','o','g'};
            strncpy(in, &input[i], strlen(input));
            for(int j = 0; j< pos-2; j++)
            {
                if(cat[0] == in[j])
                {
                    if(cat[1] == in[j+1])
                    {
                        if(cat[2] == in[j+2])
                        {
                            input[j] = 99;
                            input[j+1] = 97;
                            input[j+2] = 116;
                            i+=2;
                        }
                    }
                }
            }
        }
    }// end of state
    if(state == SIGALRM)
    {
        //doing nothing and since string is already in the pipe no need to change it.
    }
}

/*
 handler function, changes state of the parent process to be able to be the middle man. sign is the signal value itself
 RETURN- no return value
 */
void myHandler(int sign)
{
    if(sign == SIGUSR1)
    {
        state = SIGUSR1;
    }
    else if(sign == SIGUSR2){
        state = SIGUSR2;

    }
    else if(sign == SIGALRM){
        state = SIGALRM;
    }
    else{
        perror("Error in signal");
    }
}


int main(int argc, char** argv)
{

    //char defaultMessage[3];
    int pipes[2];
    char buffer[3];
    pid_t pid;
    ssize_t reaad = argc;
    int pipeFlag = pipe(pipes);

    if(pipeFlag == -1)                  //error creating pipe
    {
        perror("ERROR creating PIPE try again");
        exit(EXIT_FAILURE);
    }
    pid = fork();

    if(pid == -1)                       //errror creating fork
    {
        perror("ERROR creating fork try again");
        exit(EXIT_FAILURE);
    }

    if(pid == 0)                //enter child process
    {
        close(pipes[0]);            //close read end
        dup2(pipes[1], STDOUT_FILENO);      //connect stdout to write end of pipe

        char *myargs[2];
        myargs[0] = argv[1];
        myargs[1] = argv[2];            //copy argv into child variable

        execvp(myargs[0], myargs);          //execute cat
        write(pipes[1], argv[1], strlen(argv[1]));         //write input from stdin to pipe

        close(pipes[1]);
        exit(EXIT_SUCCESS);

    }
    else{
        /*
         Parent process:
            copys the values from the pipe put by the child process,
            depending on state it will change every instance of cat and dog interchangeably.

         */

        signal(SIGUSR1, myHandler);
        signal(SIGUSR2, myHandler);
        signal(SIGALRM, myHandler);                 //check for signal

        close(pipes[1]);                //close write end
        reaad = read(pipes[0], &buffer, 1);             //read contents of pipe
        while (reaad > 0)
        {
            if(strcmp(buffer, "\n") != 0)           //if enter is not pressed on CMD, i.e user not done typing
            {
                temp[pos++] = buffer[0];
                //write(STDOUT_FILENO, &temp[pos-1], 3);
            }

            if(strcmp(buffer, "\n") == 0)               //user hit enter and wants to process input
            {

                middleMan(temp, state);                 //main in the middle ;)
                write(STDOUT_FILENO, temp, pos);        //print altered string to pipe i.e std out
                write(STDOUT_FILENO, "\n", 1);
                pos = 0;                                //reset position of array
            }
            reaad = read(pipes[0], &buffer,1);

        }

        close(pipes[0]);            //close read end
        wait(NULL);                 //wait for baby boy
        exit(EXIT_SUCCESS);
    }
    return EXIT_SUCCESS;
}
//end of file
