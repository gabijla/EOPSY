#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define RUNTIME 30
#define N 5 
#define LEFT ( i + N - 1 ) % N 
#define RIGHT ( i + 1 ) % N

void grab_forks(int i);
void put_away_forks(int i);
void test(int i); 
void *philosopher(void *arg); 		//philosopher thread start routine
void think(int i); 			
void eat(int i); 			

int state[N]; 				//pilosophers states
pthread_mutex_t m;			//critical section mutex
pthread_mutex_t s[N];			//mutex array for philosopher sync
pthread_t p_tid[N];			//philosopers thread IDs
			
enum state 				//enumeration  of states
{
    	THINKING,
    	EATING,
    	HUNGRY
}; 

int main()
{
	int p_id[N];
	//initializing IDs and states as thinking
	for(int i=0; i < N; i++)
	{
		p_id[i] = i; 
		state[i] = THINKING;

		
        	//initializing synchronization thread mutexes
        	if (pthread_mutex_init(&s[i], NULL) != 0)
        	{
            		perror("error while initializing mutex");
            		exit(1);
        	}
        	//locking mutexes
        	pthread_mutex_lock(&s[i]);
	}
	
	


	for (int i = 0; i < N; i++)
   	{
		//creating philosophers threads
        	if (pthread_create(&p_tid[i], NULL, philosopher, (void *)&p_id[i]) != 0) 
        	{
            		perror("error while creating thread");
            		exit(1);
        	}
    	}

    	sleep(RUNTIME);
	
    	for (int i = 0; i < N; i++)
    	{
		//cancelating threads and waiting them to terminate
        	pthread_cancel(p_tid[i]); 
        	pthread_join(p_tid[i], NULL); 
		pthread_mutex_destroy(&s[i]);
    	}
	
	//destroying mutexes    
   	pthread_mutex_destroy(&m); 
}

void *philosopher(void *arg) 
{
   	//this cacelability state is needed since we need to cancel them at any time immediately
    	if (pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) != 0)
    	{
        	perror("error whyle setting cancel type to asynchronous");
        	exit(EXIT_FAILURE);
    	}
    	int i = *((int *)arg);
	
	//infinite while loop "main problem"
    	while (true) 
    	{	
    	    	think(i);
        	grab_forks(i);
        	eat(i);
        	put_away_forks(i);
    	}

    	return NULL;
}

void grab_forks(int i)
{
    	pthread_mutex_lock(&m);
    	state[i] = HUNGRY;
    	test(i);
    	pthread_mutex_unlock(&m);
    	pthread_mutex_lock(&s[i]);
}

void put_away_forks(int i)
{
    	pthread_mutex_lock(&m);
    	state[i] = THINKING;
    	test(LEFT);
    	test(RIGHT);
    	pthread_mutex_unlock(&m);
}

void test(int i)
{
    	if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    	{
        	state[i] = EATING;
        	pthread_mutex_unlock(&s[i]);
    	}
}

void think(int i)
{
    	printf("PHILOSOPHER[%d]: THINKING\n", i);
    	sleep(2);
}

void eat(int i)
{
    	printf("PHILOSOPHER[%d]: EATING\n", i);
    	sleep(2);
}
