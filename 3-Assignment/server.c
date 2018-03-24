/*
Joshua Lane
UserID: JAL987
CS-GY 6233 Intro to Operating Systems
Assignment #3, server.c

Summary:  	This program operates by connecting to shared memory already created
			by receiver.c.  It watched the first character in that shared memory
			to determine what it should do next.  This first character acts as 
			a control signal, with all necessary data following it.  
			Upon receiving a non-wait signal, this program will either clean up 
			and then quit or it will process the secret from shared memory into
			the file secrets.out.  It will the return to a wait condition. 

NOTE:  		If this program were more complex communications between the two 
			processes would be handled using semaphores, rather than a character
			byte at the start of the shared memory buffer, but since it is only 
			two, non-threaded processes with very simple interaction rules, the
			character byte approach seems sufficient.  
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define QUIT "QUIT"

//The following characters will determine the behavior of the two processes
//One of them will always be present in the first space of the buffer

//# tells processor.c to wait for input and to trigger receiver.c to continue
//! tells processor.c process the string that follows.  
//A second ! also tells processor.c where the string ends
//@ tells processor.c that receiver.c has received a request to quit
#define WAIT '#'
#define STRING '!'
#define QUITSIG '@'

//This is the filename that is used to let the processor know that the receiver is alive and active
//A second filename isn't needed because once both processes are active, they can communicate via the 
//shared memory channel.
#define REC_AWAKE ".SM_R_alive"

#define TEXT_BUFFER_KEY 5555
#define IPC_BUFFER_SIZE 1024 

//The main menu block that drives user interaction
//Alerts the user to how many secrets have been processed. 
void printMenu(int secretNumber){
	printf("\n\n########  SHARED MEMORY PROCESSOR START  ###########\n");
	printf("%d secrets shared.\n",secretNumber);
}

void main(){

	char *shm_text_ptr, *temp; 
	char secretPhrase[IPC_BUFFER_SIZE];
	char recPid[10];
	bool quit = false;
	int shm_text, secretLength, i;
	int secretNumber=0;
	FILE *secrets;
	FILE *rAlive;

	rAlive = fopen(REC_AWAKE,"r");
	if (rAlive == NULL){
		printf("The reciever is not running.  Please type ./rec before running this process.\n");
	}
	else{
		fclose(rAlive);
		//Connect to the shared memory communications channel already created by receiver.c
		//the shm_text channel will serve to pass text back and forth
		shm_text = shmget(TEXT_BUFFER_KEY,IPC_BUFFER_SIZE, IPC_CREAT | 0666);

		if (shm_text < 0){
			printf("ERROR: shm_text shmget failure\n");
			quit = true;
		}
		shm_text_ptr = shmat(shm_text, NULL, 0);

		if (shm_text_ptr == "-1"){
			printf("ERROR: shm_text shmat failure\n");
			quit = true;
		}

		//change the signal buffer to the WAIT flag so that the receiver sees that the 
		//processor is now up and running.  
		shm_text_ptr[0] = WAIT;

		//open secrets.out for writing
		secrets = fopen("secrets.out","w");

		if (secrets == NULL){
			printf("ERROR:  secrets.out failed to open.");
			printf("ERROR:  exiting...");
			quit = true;
		}

		//This if checks to see if we had any errors above.  If so, it skips execution and ends. 
		if (quit == false){
			while(quit == false){
				//reset the length of our current secret
				secretLength = 0;

				//print the menu to terminal
				printMenu(secretNumber);	

				//alert user to wait condition and then wait for a signal from receiver
				printf("Waiting for a secret to be passed.\n");
				while (shm_text_ptr[0] == WAIT){
					sleep(1);
				}

				//Once the wait signal has changed, check if the new signal is a quit
				if ((shm_text_ptr[0]) == QUITSIG){

					printf("Processor.c has received a quit signal from the receiver.\n");
					printf("Cleaning up and quiting now.\n");
					
					//close our secrets file
					fclose(secrets);

					//kill our infinite loop
					quit = true;

				}
				else {
					//if the signal is not a quit, it must be a secret to process
					printf("Processing secret information received from the receiver.\n");

					//temporary pointer so we don't mess with shm_text_ptr
					temp = shm_text_ptr;

					//use temp to process the secret from shared memory
					//calculate the length of the secret while also 
					//copying it to a temporary variable
					while(temp[secretLength + 1] != STRING){
						secretPhrase[secretLength] = temp[secretLength + 1];
						secretLength++;
					}

					//print the length of the current secret to file
					fprintf(secrets, "%d|",secretLength);

					//print the secret one character at a time to the file
					for(i=0; i<secretLength;i++){
						fprintf(secrets, "%c",secretPhrase[i]);
					}

					//move the file to a new line to be ready for the next secret
					fprintf(secrets, "\n");

					//set the first character of the communications buffer to the wait signal
					//This tells the receiver that the processor is done with it's function and 
					//is returning to a wait status.  Receiver can exit wait and continue.
					shm_text_ptr[0] = WAIT;

					//increment the number of secrets passed
					secretNumber++;
				}

			}

			//clean up the shared memory. 
			shmdt(shm_text_ptr);
		}
	}
	return;
}