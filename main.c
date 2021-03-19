#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

// -------------- Global Values for what is on the table --------
// 0 - not on table 
// 1 - on table 
int     PAPER_TABLE      = 0, 
        MATCH_TABLE      = 0, 
        TOBACCO_TABLE    = 0;
// -----------------------

// agent threads 
pthread_t tobacco_paper_agent, tobacco_match_agent, paper_match_agent; 

// pusher threads 
pthread_t tobacco_pusher, match_pusher, paper_pusher;

// smoker threads 
pthread_t match_man_1, match_man_2, paper_man_1, paper_man_2, tobacco_man_1, tobacco_man_2;

// resource semaphores 
sem_t tobacco, match, paper; 

// man semaphores 
sem_t tobbaco_man_sem, paper_man_sem, match_man_sem;

// misc semaphores 
sem_t agent_sem;
sem_t mutex;

int main(void)
{
    printf("done\n");

    return 0;
}