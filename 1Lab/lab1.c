#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void alarmHandler(int);
void quitHandler(int);

int main() {
	signal ( SIGALRM, alarmHandler);
	signal ( SIGINT, quitHandler);

	const size_t SIZE = 20;
	char * userName;
	userName = (char *) malloc (SIZE);

	while(1) {

		printf("Please enter your username: ");
		alarm(5);
		fscanf(stdin, "%s", userName);

		if (strcmp(userName, "exit") == 0)
		{
			exit(0);
		}
		printf("\nYour username is %s\n\n", userName);
		fflush(stdout);
	}

	return 0;
}

void alarmHandler ( int theAlarm) {
	printf("\n\nTime's up!\n");
	printf("\nPlease enter your username: ");
	fflush(stdout);
	alarm(5);
	return;
}

void quitHandler ( int theInt) {
	printf("\n\nPlease type 'exit' to quit. \n");
	printf("\nPlease enter your username: ");
	fflush(stdout);
	alarm(5);
	return;
}
