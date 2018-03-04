/*
Joshua Lane
UserID: JAL987
CS-GY 6233 Intro to Operating Systems
Assignment #2
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define SECRET_KEY "COOL"
#define QUIT "QUIT"
#define IN_BUFFER_SIZE 512
#define DONE_WRITE "%" //The character that the receiver will write to l
#define DONE_READ "#"



//The main menu block that drives user interaction
void printMenu(){
	printf("\n\n########  SHARED MEMORY PROGRAM START  ###########\n\n");
	printf("Please enter a single phrase no more than %d bytes.\n", IN_BUFFER_SIZE);
	printf("Make sure your phrase includes \"COOL\" if you want it to be shared.\n");
	printf("Please type QUIT to end.\n");
	printf("Secret Phrase: ");

}

void main(){

	char *input = malloc(sizeof(char)*IN_BUFFER_SIZE);
	char *temp;
	bool quit = false;

	while(quit == false){
		printMenu();	
		fgets(input,IN_BUFFER_SIZE,stdin);

		if (strncmp(input,QUIT,4) == 0){

			//logic flow output used to test while building program
			printf("YOU JUST ASKED TO QUIT\n");
			

			quit = true;
		}

		if (strstr(input,SECRET_KEY)!= NULL){
			//logic flow output used to test while building program
			temp = input;
			temp += strlen(input);
			*temp = DONE_WRITE;
			printf("YOU ENTERED THE SECRET KEY AS PART OF YOUR PHRASE %s \n", input);

		}
		else{
			//logic flow output used to test while building program
			printf("YOU DIDNOT ENTER THE SECRET KEY AS PART OF YOUR PHRASE\n");			
		}
	}

	return;
}