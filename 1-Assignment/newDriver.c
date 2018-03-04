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
	char *input = calloc(32, sizeof(char*));
	char *parsedInput = calloc(32, sizeof(char*));
	

	while (strcmp(input,QUIT) != 0){
		printf("Start Loop\n");
		i = 0;
		printMenu(mainPID);
		gets(input);
		parsedInput = input;
		printf("You Types: %s\n",input);
		printf("You Types: %s\n",parsedInput);
			while ((tempChar = strsep(&parsedInput," "))!= NULL){
				printf("%s\n", tempChar);
				argv[i] = calloc(strlen(tempChar) + 1, sizeof(char*));
				strncat(argv[i],tempChar,strlen(tempChar));
				if ((strcmp(argv[i],BADPIPE)) || (strcmp(argv[i],BADREDIRECTL)) || (strcmp(argv[i],BADREDIRECTR))){
					badInput = true;
				}
				printf("INPUTST: %s\n", input);
				printf("Arg %d: %s\n", i,argv[i]);
				i++;
			}

			if (badInput == true){
				printf("This program doesn't support piping or redirects\n");
			}

			printf("Input End: %s\n", input);
			printf("EndLoop\n");
	}

	printf("Thank you and good bye!\n");
   
}