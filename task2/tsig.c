#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_CHILD  10
#define WITH_SIGNALS

#ifdef WITH_SIGNALS
int interr = 0;
void int_handler();
void term_handler();
#endif

int terminate(int i,int* ids);

int child_count=0;
pid_t child_pids[NUM_CHILD];

int main()
{
	#ifdef WITH_SIGNALS
	for(int i=1; i<NSIG; i++)
		//Ignoring all signals
		signal(i, SIG_IGN);
	//Restoring SIGCHLD to default handler
	signal(SIGCHLD, SIG_DFL);
	//Setting int_handler to SIGINT
	signal(SIGINT, int_handler);
	#endif

	int status;
	int cpid;
	int finished=0;

	for(int i=0;i<NUM_CHILD;i++) // loop will run NUM_CHILD times
	{
		#ifdef WITH_SIGNALS
		//Checking interrupt value
		if(interr != 0)
		{
			//When the process is interrupted, exits the loop
			terminate(i, child_pids);
			printf("parent[%d]: son creation interrupted.\n", getpid());
			break;
		}
		#endif

		//Son process creation
		cpid = fork();

		//Unsuccessful creation
		if(cpid < 0)
		{
			//Return value: 1 and sending SIGTERM to all created processes
			return terminate(i,child_pids);
		}

		//Parent process
		if(cpid > 0)
		{
			printf("parent[%d]: created son with pid [%d]\n", getpid(),cpid);
			child_pids[i]=cpid;
			child_count++;
			sleep(1);
			continue;
		}

		//Son process
        	if(cpid == 0)
        	{
			#ifdef WITH_SIGNALS
			signal(SIGINT, SIG_IGN);
			signal(SIGTERM, term_handler); 
			#endif
			printf("child[%d]: My parent process is: [%d]\n",getpid(),getppid());
			sleep(10);
			printf("child[%d]: Execution complete!\n",getpid());
        		return 0;
        	}
		#ifdef WITH_SIGNALS
		// Return signal handlers
		for(int i=1; i<NSIG; i++)
		signal(i, SIG_DFL);
		#endif
		return 0;

	}

	int exit_codes[child_count];

	for(finished;finished<child_count;finished++)
	{
		//Waits for son processes to finish and save the exit code
		wait(&status);
		exit_codes[finished]=WEXITSTATUS(status);
	}
	printf ("parent[%d]: All %d son processes terminated\n",getpid(),finished);
	for(int i=0;i<child_count;i++)
	{
		printf("Exit code from process: [%d] --> [%d]\n", (int)child_pids[i], exit_codes[i]); 
	}

	 #ifdef WITH_SIGNALS
        for(int i=1; i<NSIG; i++)
		//Restore default signal handlers
                signal(i, SIG_DFL);
        #endif
        return 0;
}

#ifdef WITH_SIGNALS
void int_handler()
{
	printf("parent[%d]: keyboard interrupt\n", getpid());
	interr = 1;
}

void term_handler()
{
	printf("child[%d]: SIGTERM signal.\n", getpid());
}
#endif

int terminate (int i, int* ids){
	for(int j=0;j<i;j++)
	{
		printf("parent[%d]: sending SIGTERM to PID [%d]\n",getpid(), (int)child_pids[j]);
		kill(ids[j], SIGTERM);
	}
	return 1;
}

