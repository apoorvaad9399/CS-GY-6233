/*
Joshua Lane
UserID: JAL987
CS-GY 6233 Intro to Operating Systems
Assignment #3, client.c

Summary:  	This program operates by creating shared memory for communications
			between it and processor.c.  The first character in that shared memory
			determines how both programs behave.  This first character acts as 
			a control signal, with all necessary data following it in all cases.  
			The receiver takes text input from the user, scans it for the key word
			anywhere in the input and then reacts accordingly.  If the key word is
			found, the program places the user input into the buffer and signals 
			that the processor should execute it's functions.  The receiver then
			enters a wait status until it sees a signal in the shared memory that
			the processor is done and has returned to a wait status.  At that 
			point the receiver prompts for more user input.  This continues until 
			a QUIT is received, upon which the receiver signals the process to 
			quit and then quits itself.  

NOTE:  		If this program were more complex communications between the two 
			processes would be handled using semaphores, rather than a character
			byte at the start of the shared memory buffer, but since it is only 
			two, non-threaded processes with very simple interaction rules, the
			character byte approach seems sufficient.  

*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SECRET_KEY "C00L"
#define QUIT "QUIT"

#define IN_BUFFER_SIZE 512

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
#define IPC_BUFFER_SIZE 1024 //large than the input buffer to ensure that input doesn't overflow the text buffer

//The main menu block that drives user interaction
void printMenu(){
	printf("\n\n########  SHARED MEMORY PROGRAM START  ###########\n\n");
	printf("Please enter a single phrase no more than %d bytes.\n", IN_BUFFER_SIZE);
	printf("Make sure your phrase includes \"COOL\" if you want it to be shared.\n");
	printf("The '!', '@', and '#' characters are reserved and cannot be used in secrets.\n");
	printf("Please type QUIT to end.\n");
	printf("Secret Phrase: ");
}

void main(){

	char *input = malloc(sizeof(char)*IN_BUFFER_SIZE);
	char *temp, *shm_text_ptr;
	bool quit = false;
	int shm_text;
	FILE *alive;


	//using files to make sure both processes are running
	//and to alert the user if they are not
	alive = fopen(REC_AWAKE,"w");
	fprintf(alive,"%d",getpid());
	fclose(alive);

	//Set up the shared memory communications channel
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


	//this if-statement checks to see if we had any errors above.  If so, skip execution and end. 
	if (quit == false){
		//Set the signal buffer so rec waits until proc responds before continuing.  
		//Proc will change this first value to its own WAIT signal to let the 
		//receiver know that the processor is awake and the receiver can continue. 
		shm_text_ptr[0] = STRING;
		printf("Waiting for the processor to run.  Please type ./procsm in another window\n");
		while (shm_text_ptr[0] == STRING){
			sleep(1);
		}

		while(quit == false){
			printMenu();	
			fgets(input,IN_BUFFER_SIZE,stdin);

			if (strncmp(input,QUIT,4) == 0){

				//User feedback
				printf("Quitting both programs now.\n");
				
				//set the first character of the channel to the quit signal
				shm_text_ptr[0] = QUITSIG;
				
				//clean up the file that is used for telling the processor that the receiver is active.
				remove(REC_AWAKE);

				//cancel the infinite loop
				quit = true;
			}
			else if ((strstr(input,"!") != NULL) || (strstr(input,"@") != NULL) || (strstr(input,"#") != NULL)){
				//Error check to ensure control characters aren't used in input
				printf("\n ERROR: Input skipped. Please avoid using the !, @, or # symbols.\n");
			}
			else if (strstr(input,SECRET_KEY) != NULL){
				
				//Skip loading the signal character to the first slot of the buffer.  
				//This avoid the processor executing before the receiver has finished. 
				//Receiver will put the signal character in after it has prepared and loaded
				//all necessary data into the shared memory buffer.  
				temp = shm_text_ptr;
				temp++;

				//Move the input, temporarily including the newline character, to the communication buffer
				memcpy(temp,input, strlen(input));

				//Add the string termination character in place of the newline character
				//The minus one facilitates the overwrite of the newline character
				temp += strlen(input) - 1;
				*temp = STRING;

				//The last act is to put the signal character into the buffer
				shm_text_ptr[0] = STRING;
				printf("Please wait while the processor handles the secret data.\n");
			}
			else{
				
				//This is the feedback for the case where the user did not enter the secret phrase
				printf("This phrase did not contain the secret key and will not be shared. \n");			
			}

			//wait until the processor has done it's work
			//in which time it will send a signal via the first character string in the shared memory 
			while (shm_text_ptr[0] == STRING){
				sleep(1);
			}
			
		}

		//clean up and destroy the shared memory. 
		shmdt(shm_text_ptr);
		
		shmctl(shm_text,IPC_RMID,0);
		
	}
	return;
}