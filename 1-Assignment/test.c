#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main () {
	
	char input[] = "";
	int mainPID = getpid();
	char *args[5];
	char *tempChar;
	int numArgs = 0;
	char *options;
	int fileDescriptors[2];

	printf("####################################\n");
	printf("The main program's PID is: %d\n", mainPID);
	printf("Please enter a simple shell command.\n");
	printf("Please enter 00000 (5 zeros) to end.\n");
	gets(input);
	printf("You typed: %s\n",input);

	tempChar = strtok (input, " ");
	args[numArgs] = tempChar;
	numArgs = numArgs + 1;
	while (tempChar != NULL)
	{
		tempChar = strtok (input, " ");
		args[numArgs] = tempChar;
		printf("OUTPUT: %s\n",tempChar );
		numArgs = numArgs + 1;

	}

	printf("OUTPUT: %s\n",options);

	pipe(fileDescriptors);

	int newProc = fork();
	if (newProc==0){
		printf("In Child: %d\n", getpid());
		dup2(fileDescriptors[1], STDOUT_FILENO);
		close(fileDescriptors[1]);
		close(fileDescriptors[0]);
		//execvp(input[0], input); 
	}
	close(fileDescriptors[1]);

	char buffer[4096];
	while (1){
		ssize_t count = read(fileDescriptors[0], buffer, sizeof(buffer));
		if (count == 0){
			break;
		}
		else{
			break;
			printf("%s",buffer);
		}
	}
	close(fileDescriptors[0]);
	wait(0);
	printf("In PID: %d\n", getpid());


	printf("Thank you and good bye!\n");
   
   return(0);
}