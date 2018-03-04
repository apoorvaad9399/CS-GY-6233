#include <stdio.h>
#include <unistd.h>
//#include <linux/syscalls.h>

int main () {
	
	char input[] = "";
	int mainPID = getpid();
	int returnValExec;
	char *args[] = {};

	while (strcmp(input,"00000") != 0){
		printf("####################################\n");
		printf("The main program's PID is: %d\n", mainPID);
		printf("Please enter a simple shell command.\n");
		printf("Please enter 00000 (5 zeros) to end.\n");
		scanf("%s", input);
		printf("You typed: %s\n",input);
		int newProc = fork();
		if (newProc==0){
			printf("In Child: %d\n", getpid());
			execvp(input, args); 
		}
	}

	printf("Thank you and good bye!\n");
   
   return(0);
}