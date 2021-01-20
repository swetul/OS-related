/*
	Swetul Patel
	Simulating the use of a semaphore in C
*/

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int SEMPAHORE = 2;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int tID = 0;
int threadV = 0;

void Resource()
{
	printf("DOING SOME WORK by THREAD %d \n",threadV);
	sleep(3);
	//pthread_exit(0);
}

void *Resource1()
{
	pthread_mutex_lock(&lock);
	printf("Thread %d is getting access to the resource \n", threadV);
	printf("DOING SOME WORK by THREAD %d \n", threadV);
	sleep(5);
	pthread_mutex_unlock(&lock);
	printf("Thread %d is releasing access to the resource \n \n", threadV);
	threadV++;
	pthread_exit(0);

}

//function that tries to mimic a semaphor but it does not work since mutex is not really a sempahor unless the value of the semaphor is 1.
void *requestResource()
{

	if(SEMPAHORE > 0)
	{
		pthread_mutex_lock(&lock);
		SEMPAHORE--;
		printf("Thread %d is getting access to the resource \n", threadV);
		Resource();
		pthread_mutex_unlock(&lock);
		SEMPAHORE++;
		printf("Thread %d is releasing access to the resource \n \n", threadV);
	}threadV++;
	pthread_exit(0);
}


int main()
{

	pthread_t Thread;
	pthread_t Thread1;

	//attempting to mimic a semaphor
	for(int i = 0; i < 5; i++)
	{
		pthread_create(&Thread, NULL, requestResource, NULL);
		tID++;
		//pthread_join(Thread, NULL);
		printf("Thread %d has been created. \n", i);
	}
	pthread_join(Thread, NULL);
	pthread_join(Thread, NULL);
	pthread_join(Thread, NULL);
	pthread_join(Thread, NULL);
	pthread_join(Thread, NULL);

	//testing to see normal operation of a lock
	threadV = 0;
	tID = 0;
	for(int i = 0; i < 5; i++)
	{
		pthread_create(&Thread1, NULL, Resource1, NULL);
		tID++;
		printf("Thread %d has been created. \n", i);
	}

	pthread_join(Thread1, NULL);
	return 0;
}






