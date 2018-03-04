/*
Joshua Lane
UserID: JAL987
CS-GY 6233 Intro to Operating Systems
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BADPIPE "|"
#define BADREDIRECTR ">"
#define BADREDIRECTL "<"
#define QUIT "QUIT"


//The main menu block that drives user interaction
void printMenu(mainPID){
	printf("############  PROGRAM START  ###############\n");
	printf("The main program's PID is: %d\n", mainPID);
	printf("Please enter a simple shell command.\n");
	printf("Piping and redirects are not supported.\n");
	printf("Please type QUIT to end.\n");	
}

void main () {
		
	char *argv[64]; //for building the input to execvp
	char buffer[4096]; //for capturing the child process's output
	char *tempChar; //temporary variable for moving characters around
	int i=0; //iterator
	int j=0; //iterator
	int mainPID = getpid(); 
	int fileDescriptors[2]; 
	bool noFork = false; //variable for controlling program flow
	bool noQuit = true;
	char *input = calloc(64, sizeof(char*)); //capture user input
	char *parsedInput = calloc(64, sizeof(char*)); //parse input
	
	while (noQuit){

		//Print menu, instructions and capture user input
		printMenu(mainPID);
		gets(input);

		//Parse the input into the command and options
		//build the argv which will be used to pass to exec in a later step
		parsedInput = input;
		i = 0;
		noFork = false;
		while (((tempChar = strsep(&parsedInput," "))!= NULL) ) {

			argv[i] = calloc(strlen(tempChar) + 1, sizeof(char*));
			strncat(argv[i],tempChar,strlen(tempChar));


			//Program control flow checks and 
			//error handling in case the user tries to use piping or redirects or quit.  
			//Capture the input, warn user about usage, and return to the menu.
			//In either case skip the rest of execution, evaluate the while loop and prompt again or exit.  
			
			if (strcmp(argv[i],QUIT) == 0){
				noQuit = false;
				break;
			}
			else if ((strcmp(argv[i],BADPIPE) == 0) || (strcmp(argv[i],BADREDIRECTL) == 0) || (strcmp(argv[i],BADREDIRECTR) == 0) ){
				noFork = true;
				printf("ERROR:  This program doesn't support piping or redirects\n");
				break;
			}
			i++;
		}

		if ((noFork == false) && (noQuit == true)){
			//This is the meat of the program.  Fork the process.
			//Enter the child process and adjust file descriptors to pipe the 
			//child process's output into the parent process so that the parent 
			//process can read and ultimately display what the child process would otherwise
			//send to stdout.
			//Use the argv prepped above to run execvp
			pipe(fileDescriptors);
			int newProc = fork();
			if (newProc==0){
				//Used for tracking which process the program
				//is currently running in
				printf("##################################\n");
				printf("##        Child PID: %d      ##\n", getpid());
				printf("## Child Process Output Follows ##\n");
				printf("##################################\n");
				dup2(fileDescriptors[1], STDOUT_FILENO);
				close(fileDescriptors[1]);
				close(fileDescriptors[0]);
				execvp(argv[0], argv); 
			}
			close(fileDescriptors[1]);

			//grabbing the output of the child process from the appropriate FD,
			//reading it into a defined buffer and then printing the buffer to the screen
			while (1){
				ssize_t count = read(fileDescriptors[0], buffer, sizeof(buffer));
				if (count == 0){
					break;
				}
				else{
					printf("%s",buffer);
				}
			}
			close(fileDescriptors[0]);
			wait(0);

			//Used for tracking which process the program
			//is currently running in
			printf("##################################\n");
			printf("##       Current PID: %d     ##\n", getpid());
			printf("## Child Process Output Ends    ##\n");
			printf("##################################\n");

		}
		
		//###############################################
		//  Variable Cleanup Section  
		//  Necessary to reset certain variables for 
		//  clean looping of the program
		//###############################################

		memset(input,0,sizeof(input));
		memset(argv,0,sizeof(argv));			
		memset(buffer,0,4096);
	}

	printf("Thank you and good bye!\n");
   
}