#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BADPIPE "|"
#define BADREDIRECTR ">"
#define BADREDIRECTL "<"
#define QUIT "QUIT"

void printMenu(mainPID){
	printf("####################################\n");
	printf("The main program's PID is: %d\n", mainPID);
	printf("Please enter a simple shell command.\n");
	printf("Piping and redirects are not support.\n");
	printf("Please type quit to end.\n");	
}

void printContinue(){
	char *dummyValue = calloc(32, sizeof(char*));
	printf("Please press any key to continue\n");
	gets(dummyValue);
}

void main () {

		
	char *argv[32];
	char *tempChar;
	char *userCommand;
	int i=0;	
	int mainPID = getpid();
	int MAX_ARGS = 32;
	int fileDescriptors[2];
	bool badInput = false;
	//char *input = calloc(32, sizeof(char*));
	char *input[50];
	char parsedCommand[50] = 'go';
	

	while (strcmp(parsedCommand,QUIT) != 0){
		printf("Start Loop\n");
		i = 0;
		printMenu(mainPID);
		gets(input);
		printf("You Types: %s\n",input);
			while ((tempChar = strsep(&input," "))!= NULL){
				printf("%s\n", tempChar);
				argv[i] = calloc(strlen(tempChar) + 1, sizeof(char*));
				strncat(argv[i],tempChar,strlen(tempChar));
				if ((strcmp(argv[i],BADPIPE)) || (strcmp(argv[i],BADREDIRECTL)) || (strcmp(argv[i],BADREDIRECTR))){
					badInput = true;
				}
				printf("Arg %d: %s\n", i,argv[i]);
				printf("Input: %s\n", input);
				i++;
			}

			if (badInput == true){
				printf("This program doesn't support piping or redirects\n");
			}

			pipe(fileDescriptors);

			int newProc = fork();
			if (newProc==0){
				printf("In Child: %d\n", getpid());
				dup2(fileDescriptors[1], STDOUT_FILENO);
				close(fileDescriptors[1]);
				close(fileDescriptors[0]);
				execvp(argv[0], argv); 
			}
			close(fileDescriptors[1]);

			char buffer[4096];
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
			printf("In PID: %d\n", getpid());
			//printContinue();
			free(input);
			char *input = calloc(32, sizeof(char*));
			input = "";
			printf("EndLoop\n");
	}

	printf("Thank you and good bye!\n");
   
}