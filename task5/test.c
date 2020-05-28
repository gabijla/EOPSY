#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>


#define N1 1	//Women barber
#define N2 1	//Men barber
#define N3 0	//Both barber
#define M 4	//Chairs number
#define C 5	//Customers number

//Semaphores
sem_t waiting_room;
sem_t chair_W;
sem_t chair_M;
sem_t chair_B;
sem_t sleeping_W;
sem_t sleeping_M;
sem_t sleeping_B;
sem_t wait_B;

//Variables
int barberWC[N1];
int barberMC[N2];
int barberBC[N3];
int clients[C];
bool done = false;

//Threads
pthread_t barberThreadW[N1];
pthread_t barberThreadM[N2];
pthread_t barberThreadB[N3];
pthread_t clientThread[C];

void init(){
    //Barber ids arrays
    for (int i = 0; i < N1; i++)
    {
        barberWC[i] = i;
    }

    for (int i = 0; i < N2; i++)
    {
        barberMC[i] = i;
    }

    for (int i = 0; i < N3; i++)
    {
        barberBC[i] = i;
    }

    //Client ids array
    for (int i = 0; i < C; i++)
    {
        clients[i] = i;
    }

    //Semaphores
    sem_init(&waiting_room, 0, M);
    sem_init(&chair_W, 0, N1);
    sem_init(&chair_M, 0, N2);
    sem_init(&chair_B, 0, N3);
    sem_init(&sleeping_W, 0, 0);
    sem_init(&sleeping_M, 0, 0);
    sem_init(&sleeping_B, 0, 0);
    sem_init(&wait_B, 0, 0);
};

void *client(void *id){
    
    //Randomize the customer gender
    char gender;
    if ((rand() % 2) == 0)
    {
        gender = 'w';
    }
    else
    {
        gender = 'm';
    }

    //Free seats in the waiting room
    int room_seats;

    //Free seats by gender
    int freeWseat;
    int freeMseat;
    int freeBseat;

    //Used to know id the 'both barber' is available
    bool both;
    
    sleep(rand() % 10);

    printf("\n> Client[%c] -- %d -- checks the waiting room\n", gender, *(int *) id);
    //Here the number of free seats is stored in room_seats variable
    sem_getvalue(&waiting_room, &room_seats);
   
    //Next the client proceed to check the barber disponiblity regarding the gender, first checks the gender one then the 'both' one
    if (room_seats != 0)
    {
	printf("> Client[%c] %d  seats in the waiting room\n", gender, *(int *) id);
        sem_wait(&waiting_room);

        if (gender == 'w')
        {
            do
            {
                sem_getvalue(&chair_W, &freeWseat);
                if (freeWseat != 0)
                {
                    sem_wait(&chair_W);
                    both = false;
                    sem_post(&waiting_room);
                    printf("> Client[%c] %d  wakes the women barber.\n", gender, *(int *) id);
                    sem_post(&sleeping_W);
                }
                else
                {
                    sem_getvalue(&chair_B, &freeBseat);
                    if (freeBseat != 0)
                    {
                        sem_wait(&chair_B);
                        both = true;
                        sem_post(&waiting_room);
                        printf("> Client[%c] %d  wakes the both gender barber.\n", gender, *(int *) id);
                        sem_post(&sleeping_B);
                    }
                }

            } while (freeWseat == 0 && freeBseat == 0);        

        }

        if (gender == 'm')
        {
            do
            {
                sem_getvalue(&chair_M, &freeMseat);  
                if (freeMseat != 0)
                {
                    sem_wait(&chair_M);
                    both = false;
                    sem_post(&waiting_room);
                    printf("> Client[%c] %d  wakes the men barber.\n", gender, *(int *) id);
                    sem_post(&sleeping_M);
                }
                else
                {
                    sem_getvalue(&chair_B, &freeBseat);
                    if (freeBseat != 0)
                    {
                        sem_wait(&chair_B);
                        both = true;                  
                        sem_post(&waiting_room);
                        printf("> Client[%c] %d  wakes the both gender barber.\n", gender, *(int *) id);
                        sem_post(&sleeping_B);
                    }
                }

            } while (freeMseat == 0 && freeBseat == 0);
        }

        //Here we wait for the barber to end the word
        sem_wait(&wait_B);
        
        if (both)
        {
	    
            sem_post(&chair_B);
        }
        else
        {   
            if (gender == 'w')
            {
                sem_post(&chair_W);
            }
            if (gender == 'm')
            {
                sem_post(&chair_M);
            }
        }
        printf("> Client[%c] %d  leaves with hair done.\n", gender, *(int *) id);
    }
    else
    {   
        printf("> Client[%c] %d  leaves due to no barber available.\n", gender, *(int *) id);
    }
};   

void *barberW(void *id){
    char gen = 'w';

    while (!done)
    {
        printf("> Barber[%c] %d is spleeping.\n", gen, *(int *) id);
        sem_wait(&sleeping_W);

        if (!done)
        {
            printf("> Barber[%c] %d working.\n", gen, *(int *) id);
            sleep(rand() % 10);
            printf("> Barber[%c] %d woman's hair done.\n", gen, *(int *) id);

            sem_post(&wait_B);
        }
    }
    printf("> Barber[%c] %d done cutting hair.\n", gen, *(int *) id);
    sem_post(&sleeping_W);
};

void *barberM(void *id){
    char gen = 'm';

    while (!done)
    {
        printf("> Barber[%c] %d is spleeping.\n", gen, *(int *) id);
        sem_wait(&sleeping_M);

        if (!done)
        {
            printf("> Barber[%c] %d working.\n", gen, *(int *) id);
            sleep(rand() % 10);
            printf("> Barber[%c] %d man's hair done.\n", gen, *(int *) id);

            sem_post(&wait_B);
        }
    }
    printf("> Barber[%c] %d done cutting hair.\n", gen, *(int *) id);
    sem_post(&sleeping_M);
};

void *barberB(void *id){
    char gen = 'b';

   while (!done)
    {
        printf("> Barber[%c] %d is spleeping.\n", gen, *(int *) id);
        sem_wait(&sleeping_B);

        if (!done)
        {
            printf("> Barber[%c] %d working.\n", gen, *(int *) id);
            sleep(rand() % 10);
            printf("> Barber[%c] %d hair done.\n", gen, *(int *) id);

            sem_post(&wait_B);
        }
    }
    printf("> Barber[%c] %d done cutting hair.\n", gen, *(int *) id);
    sem_post(&sleeping_B);
}


int main(){      
    srand(time(NULL));
    
    //Initialization of the semaphores and id arrays
    init();
    

    //Threads creation	
    for (int i = 0; i < N1; i++)
    {
        pthread_create(&barberThreadW[i], NULL, barberW, &barberWC[i]);
    }

    for (int i = 0; i < N2; i++)
    {
        pthread_create(&barberThreadM[i], NULL, barberM, &barberMC[i]);
    }

    for (int i = 0; i < N3; i++)
    {
        pthread_create(&barberThreadB[i], NULL, barberB, &barberBC[i]);
    }

    for (int i = 0; i < C; i++)
    {
        pthread_create(&clientThread[i], NULL, client, &clients[i]);
    }

    //Wait the created threads to complete execution
    for (int i = 0; i < C; i++)
    {
        pthread_join(clientThread[i], NULL);
    }

    //Flag used to know when there are no more customers
    done = true;
 
    //Wake up the barbers and wait the threads as before
    for (int i = 0; i < N1; i++)
    {
        sem_post(&sleeping_W);
        pthread_join(barberThreadW[i], NULL);
    }

    for (int i = 0; i < N2; i++)
    {
        sem_post(&sleeping_M);
        pthread_join(barberThreadM[i], NULL);
    }

    for (int i = 0; i < N3; i++)
    {
        sem_post(&sleeping_B);
        pthread_join(barberThreadB[i], NULL);
    }
}




