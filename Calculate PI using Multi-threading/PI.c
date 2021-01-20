/*--------------------------------------------------------------------
  < SWETUL PATEL>

--------------------------------------------------------------------*/
#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "gmp.h"

#define PR 256 //bits available for init
#define WORKERS 1024
#define MAX_WORKERS 32

int minK = 1;
int maxK = WORKERS/MAX_WORKERS;

mpf_t finalPI;

static void* calcPI(void* k)        //fucntion that does the work, i.e calculate PI
{
    int* mK = (int *) k;
    mpf_t X,L,K,M,T,C, PI;

    mpf_init2(X, PR);
    mpf_set_si(X, 1);

    mpf_init2(L, PR);
    mpf_set_si(L, 13591409);

    mpf_init2(K, PR);
    mpf_set_si(K, 6);
    mpf_init2(T, PR);
    mpf_init2(PI, PR);

    long double c = 426880 * (sqrt(10005));
    mpf_init_set_si(M, 1);
    mpf_set_si(T, 13591409);

    mpf_init2(C, PR);
    mpf_set_si(C, c);
    mpf_set_si(PI, 1);

    //int i;
    mpf_t xConst, lConst, kConst, kCo, power, one;
    mpf_init_set_si(xConst, -262537412640768000);

    mpf_init_set_si(lConst, 545140134);
    mpf_init_set_si(kConst, 12);
    mpf_init_set_si(kCo, 16);
    mpf_init_set_si(power, 3);
    mpf_init_set_si(one, 1);
    mpf_t tempI, temp, tem1;

    mpf_init2(tempI, PR);
    mpf_set_si(tempI, 5);

    mpf_init2(temp, PR);
    mpf_set_si(temp,5);

    mpf_init2(tem1, PR);
    mpf_init_set_si(tem1,0);
    int i;
    for(i = minK; i < *mK; i++)           //this conditions are present to allow threads to work on only one value of k
    {
        mpf_set_si(tempI, i);    // k = i

        mpf_pow_ui(tem1, tempI, 3);  // tem1 = k^3

        mpf_mul(temp, tempI, kCo); //temp = k*16

        mpf_sub(temp, tem1, temp); // temp = k^3 - 16k

        mpf_add(tempI, tempI, one); // tempI = k+1;

        mpf_pow_ui(tem1, tempI, 3); // tem1 = (k^3)

        mpf_div(temp, temp, tem1); // temp = ((k^3-16k)/(k+1)^3

        mpf_mul(M, M, temp); //m = m * temp


        mpf_set_si(temp, 0);
        mpf_add(K,K,kConst); //K += 12
        mpf_pow_ui(temp , xConst, i);
        mpf_add(L,L,lConst); //L += 545140134
        mpf_mul(X, X, temp); //X *= -262537412640768000

        mpf_set_si(temp, 0);
        mpf_set_si(tem1, 0);

        mpf_mul(temp, M, L);
        mpf_div(tem1, temp, X);
        mpf_add(T, T, tem1);

        mpf_set_si(temp, 0);
        mpf_set_si(tem1, 0);
    }

    mpf_div(PI, C, T);
    mpf_set(finalPI, PI);

    return NULL;
}
struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;

	if ( (end.tv_nsec-start.tv_nsec) < 0)
	{
		temp.tv_sec = end.tv_sec-start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	}
	else
	{
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}

	return temp;
}


#ifdef USE_PROCS
#define worker_t pid_t

#define create_worker(work) make_fork(work);
#define stop_worker(worker) kill_fork(worker);
#define task_division(work) task_fork(work);

static pid_t make_fork(int *work)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Failed to fork new process.");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        calcPI(work);
        exit(EXIT_SUCCESS);
    }
    return pid;
}
static pid_t task_fork(int* work)
{
    return make_fork(work);
}
static void kill_fork(pid_t pid)
{
    waitpid(pid, NULL, 0);
}
#endif

#ifdef USE_THREADS
#define worker_t pthread_t *

#define create_worker(work) make_thread(work);
#define stop_worker(worker) kill_thread(worker);
#define task_division(work) task_thread(work);



static pthread_t *make_thread(int *work)
{
    pthread_t *thread = malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, calcPI, work);
    return thread;
}
static pthread_t *task_thread(int* work)
{

    *work = minK++;
    return make_thread(work);
}
static void kill_thread(pthread_t *thread)
{
    pthread_join(*thread, NULL);
    free(thread);
}
#endif

int main(void)
{
    mpf_init2(finalPI, PR);
    mpf_set_si(finalPI, 0);             //PI global

    int count = 0;
    int work = WORKERS / MAX_WORKERS;   // value of k to calculate PI
    unsigned int current_workers = 0;
    worker_t workers[MAX_WORKERS];
	struct timespec start, end, total;

	clock_gettime(CLOCK_REALTIME, &start);
    for (int i = 0; i < WORKERS; i++)
    {
        workers[current_workers] = task_division(&work);
        current_workers++;
        count ++;
        if (current_workers == MAX_WORKERS)
        {
            for (int j = 0; j < MAX_WORKERS; j++)
            {
                stop_worker(workers[j]);
            }
            current_workers = 0;
        }
    }
	clock_gettime(CLOCK_REALTIME, &end);
    total = diff(start, end);

    printf("\n Finished experiment with final PI value as : \n ");
    mpf_out_str(stdout,0,PR, finalPI);

    printf("\n This value was generated using %i iterations of the series.", work);

    printf("\n Finished experiment with timing %lds, %ldns\n", total.tv_sec, total.tv_nsec);
    printf("Created %d total workers in that time.\n", count);

    return EXIT_SUCCESS;
}
