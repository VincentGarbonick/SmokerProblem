#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>

// -------------- Global Values for what is on the table --------
// 0 - not on table 
// 1 - on table 
int     PAPER_TABLE      = 0, 
        MATCH_TABLE      = 0, 
        TOBACCO_TABLE    = 0;
// -----------------------

// resource semaphores 
sem_t tobacco, match, paper; 

// man semaphores 
sem_t tobacco_man_sem, paper_man_sem, match_man_sem;

// misc semaphores 
sem_t agent_sem;
//sem_t mutex;

pthread_mutex_t mutex;  

//--------- Agents ---------------

void * tobacco_paper_agent(void * arg)
{
    int tobacco_paper_agent_count = 0;

    while(tobacco_paper_agent_count != 6)
    {
        // sleep for random amount of time 0-200 mS
        sleep(  (rand() % 200)  / 100   );

        sem_wait(&agent_sem); // wait on a signal for agent to generate resources 
        
        sem_post(&tobacco);
        sem_post(&paper);

        tobacco_paper_agent_count++;
    }

    printf("Tobacco and paper agent done!\n");         
    return 0;
}

void * tobacco_match_agent(void * arg)
{
    int tobacco_match_agent_count = 0;

    while(tobacco_match_agent_count != 6)
    {
        // sleep for random amount of time 0-200 mS
        sleep(  (rand() % 200)  / 100   );

        sem_wait(&agent_sem); // wait on a signal for agent to generate resources 
        
        sem_post(&tobacco);
        sem_post(&match);

        tobacco_match_agent_count++;

    }

    printf("Tobacco match agent done!\n");  

    return 0;
}

void * match_paper_agent(void * arg)
{
    int match_paper_agent_count = 0;
    
    while(match_paper_agent_count != 6)
    {
        // sleep for random amount of time 0-200 mS
        sleep(  (rand() % 200)  / 100   );

        sem_wait(&agent_sem); // wait on a signal for agent to generate resources 
        
        sem_post(&match);
        sem_post(&paper);

        match_paper_agent_count++;

    }

    printf("Match paper agent done!\n");

    return 0;
}


//-------- Pushers ------------


void * tobacco_pusher(void * arg)
{
    int tobacco_pusher_count = 0;

    while(tobacco_pusher_count != 12)
    {
        // wait for tobacco to be generated 
        sem_wait(&tobacco);
        
        // wait for mutex lock to unlock
        pthread_mutex_lock(&mutex);

        if(PAPER_TABLE == 1)
        {
            // we have tobacco and paper, signal match
            PAPER_TABLE = 0;
            sem_post(&match_man_sem);
        }
        else if (MATCH_TABLE == 1)
        {
            // we have tobacco and match, signal paper
            MATCH_TABLE = 0;
            sem_post(&paper_man_sem);
        }
        else 
        {
            // If this is a first pass, inform global variable that there is tobacco
            // on the table
            TOBACCO_TABLE = 1; 
        }
        
        tobacco_pusher_count++;

        // unlock mutex lock 
        pthread_mutex_unlock(&mutex);

    }

    printf("Tobacco pusher done!\n");

    return 0;

}

void * paper_pusher(void * arg)
{
    int paper_pusher_count = 0;

    while(paper_pusher_count != 12)
    {
        // wait for paper to be generated 
        sem_wait(&paper);
        
        // wait for mutex lock to unlock
        pthread_mutex_lock(&mutex);

        if(TOBACCO_TABLE == 1)
        {
            // we have paper and tobacco, signal match
            TOBACCO_TABLE = 0;
            sem_post(&match_man_sem);
        }
        else if (MATCH_TABLE == 1)
        {
            // we have paper and match, signal tobacco
            MATCH_TABLE = 0;
            sem_post(&tobacco_man_sem);
        }
        else 
        {
            // If this is a first pass, inform global variable that there is tobacco
            // on the table
            PAPER_TABLE = 1; 
        }
        
        // increment count 
        paper_pusher_count++;
        
        // unlock mutex lock 
        pthread_mutex_unlock(&mutex);


    }

    printf("Paper pusher done!\n");

    return 0;
}

void * match_pusher(void * arg)
{
    int match_pusher_count = 0;

    while(match_pusher_count != 12)
    {
        // wait for match to be generated 
        sem_wait(&match);
        
        // wait for mutex lock to unlock
        pthread_mutex_lock(&mutex);

        if(PAPER_TABLE == 1)
        {
            // we have match and paper, signal tobacco man 
            PAPER_TABLE = 0;
            sem_post(&tobacco_man_sem);
        }
        else if (TOBACCO_TABLE == 1)
        {
            // we have match and tobacco, signal paper man
            TOBACCO_TABLE = 0;
            sem_post(&paper_man_sem);
        }
        else 
        {
            // If this is a first pass, inform global variable that there is match
            // on the table
            MATCH_TABLE = 1; 
        }
        
        match_pusher_count++;

        // unlock mutex lock 
        pthread_mutex_unlock(&mutex);

    }

    printf("Match pusher done!\n");
    return 0;
}

// ----------- Smokers ---------------
void * match_man(void * arg)
{
    // number of cigarettes someone has smoked
    int cigarette_count = 0; 

    while(cigarette_count != 3)
    {
        // wait for a pusher to tell tobacco man that the ingredients were there 
        sem_wait(&match_man_sem);

        printf("A match man is making a cigarette!\n"); 
        sleep(  (rand() % 50)  / 100   ); // sleep for up to 50mS 
        
        printf("A match man is smoking a cigarette!\n");
        sleep(  (rand() % 50)  / 100   ); // sleep for up to 50mS 

        sem_post(&agent_sem);

        cigarette_count++;
    }

    printf("\tGreat now I'm hungry!\n");

    return 0;
}

void * paper_man(void * arg)
{
    // number of cigarettes someone has smoked
    int cigarette_count = 0; 

    while(cigarette_count != 3)
    {
        // wait for a pusher to tell tobacco man that the ingredients were there 
        sem_wait(&paper_man_sem);

        printf("A paper man is making a cigarette!\n"); 
        sleep(  (rand() % 50)  / 100   ); // sleep for up to 50mS 
        
        printf("A paper man is smoking a cigarette!\n");
        sleep(  (rand() % 50)  / 100   ); // sleep for up to 50mS 

        sem_post(&agent_sem);

        cigarette_count++;
               
    }

    printf("\tGreat now I'm hungry!\n");

    return 0;
}

void * tobacco_man(void * arg)
{

    // number of cigarettes someone has smoked
    int cigarette_count = 0; 

    while(cigarette_count != 3)
    {
        // wait for a pusher to tell tobacco man that the ingredients were there 
        sem_wait(&tobacco_man_sem);

        printf("A tobacco man is making a cigarette!\n"); 
        sleep(  (rand() % 50)  / 100   ); // sleep for up to 50mS 
        
        printf("A tobacco man is smoking a cigarette!\n");
        sleep(  (rand() % 50)  / 100   ); // sleep for up to 50mS 

        sem_post(&agent_sem);

        cigarette_count++;

    }
    
    printf("\tGreat now I'm hungry!\n");

    return 0;
}

int main(void)
{

    int pshared = 1; // semaphore is shareable 

    // agent thread declaration 
    pthread_t tobacco_paper_agent_t, tobacco_match_agent_t, paper_match_agent_t; 

    // pusher thread declarations 
    pthread_t tobacco_pusher_t, match_pusher_t, paper_pusher_t;

    // smoker therad delcarations 
    pthread_t match_man_1_t, match_man_2_t, paper_man_1_t, paper_man_2_t, tobacco_man_1_t, tobacco_man_2_t;

    // initialize semaphores 
    sem_init(&agent_sem, pshared, 1);
    
    //sem_init(&mutex, pshared, 1);

    // initialize mutex 
    pthread_mutex_init(&mutex, NULL);

    sem_init(&tobacco, pshared, 0);
    sem_init(&match, pshared, 0);
    sem_init(&paper, pshared, 0);

    sem_init(&tobacco_man_sem, pshared, 0);
    sem_init(&paper_man_sem, pshared, 0);
    sem_init(&match_man_sem, pshared, 0);

    // initialize agent 
    if(pthread_create(&tobacco_paper_agent_t, NULL, tobacco_paper_agent, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }

    if(pthread_create(&tobacco_match_agent_t, NULL, tobacco_match_agent, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }

    if(pthread_create(&paper_match_agent_t, NULL, match_paper_agent, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }


    // pusher threads 
    if(pthread_create(&tobacco_pusher_t, NULL, tobacco_pusher, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }


    if(pthread_create(&match_pusher_t, NULL, match_pusher, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }


    if(pthread_create(&paper_pusher_t, NULL, paper_pusher, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }


    // smoker initializations 
    if(pthread_create(&paper_man_1_t, NULL, paper_man, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }

    if(pthread_create(&paper_man_2_t, NULL, paper_man, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }

    if(pthread_create(&match_man_1_t, NULL, match_man, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }

    if(pthread_create(&match_man_2_t, NULL, match_man, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }

    if(pthread_create(&tobacco_man_1_t, NULL, tobacco_man, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }

    if(pthread_create(&tobacco_man_2_t, NULL, tobacco_man, NULL))
    {
        printf("Thread creation failed!");
        exit(1);
    }


    // join threads after finishing 

    // join agents 1
    pthread_join(tobacco_match_agent_t, NULL);
    pthread_join(tobacco_paper_agent_t, NULL);
    pthread_join(paper_match_agent_t, NULL);  

    // join pushers 1
    pthread_join(tobacco_pusher_t, NULL);
    pthread_join(match_pusher_t, NULL);
    pthread_join(paper_pusher_t, NULL);

    // join smokers 1
    pthread_join(match_man_1_t, NULL);
    pthread_join(paper_man_1_t, NULL);
    pthread_join(tobacco_man_1_t, NULL);

    // join smokers 2
    pthread_join(match_man_2_t, NULL);
    pthread_join(paper_man_2_t, NULL);
    pthread_join(tobacco_man_2_t, NULL);

    printf("done\n");

    return 0;
}
