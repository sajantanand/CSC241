#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

//#include "/home/csuser/ezipc.h"
#include "ezipc.h"

#define TRUE 1
#define MAX_RAND_CUST 5
#define MAX_RAND_BARB 10

void USR1Handler ( int );
int randUser( int limit );

int chairSem, barberSem, cashierSem, countSem, printSem1, printSem2, printSem3;
char * cutsCH, *chairsCH, *count;

int main()
{
	signal ( SIGUSR1, USR1Handler );

	time_t t;
	srand ( ( unsigned ) time ( NULL ) );

	SETUP();
	
	barberSem = SEMAPHORE ( SEM_CNT, 0 );
	cashierSem = SEMAPHORE ( SEM_BIN, 0 );
	chairSem = SEMAPHORE ( SEM_BIN, 1 );
	countSem = SEMAPHORE ( SEM_BIN, 1 );
	printSem1 = SEMAPHORE ( SEM_BIN, 0 );
	printSem2 = SEMAPHORE ( SEM_BIN, 0 );
	printSem3 = SEMAPHORE ( SEM_BIN, 0 );

	int chairs, customers;
	char temp;	
	pid_t customerPID, cashierPID;

	printf ( "Barber process pid is: %d\n", getpid() );
	printf( "Enter number of chars and maximum number of customers: " );
	fflush( stdout );
	scanf ( "%d", &chairs );
	scanf ( "%d", &customers );
	printf ( "Number of seats: %d\nMaximum number of cutomers: %d\n\n", chairs, customers );
	
	chairsCH = SHARED_MEMORY ( 1 );
	*chairsCH = chairs;
	
	cutsCH = SHARED_MEMORY ( 1 );
	*cutsCH = 0;
	
	count = SHARED_MEMORY ( customers );
	*count = 0;

	customerPID = fork();											//Spawn process to handle customer processes
	if ( customerPID == 0 )	
	{
		P ( countSem );
		while ( *count < customers )									//Create a process for each customer
		{
			V ( countSem );
			customerPID = fork();
			if ( customerPID == 0)
			{
				//P ( chairSem );
				P ( countSem );
				(*count)++;
				int number = *count;
				V ( countSem );
				if ( number == customers )
				{
					printf ( "#############Reached maximum customers: %d\n", customers );
				}
				P ( chairSem );
				if ( *chairsCH > 0 )
				{
					( *chairsCH )--;
					printf ( "Customer %d arrived. There are %d seats available.\n", number,  *chairsCH );
					V ( chairSem );
					V ( barberSem );
					P ( printSem1 );
					
					printf ( "HHAAAIIIRRR CCCCUUUUTTTT. Customer %d is getting a haircut.\n", number );
					V ( printSem2 );
					P ( printSem3 );
					printf ( "@@@Customer %d pays for haircut and leaves.\n", number);
				}
				else
				{
					printf ( "*************Customer %d leaves, no chairs available.\n", number );
					V ( chairSem );
				}
				exit ( 0 );
			}
			sleep ( randUser ( MAX_RAND_CUST ) );
			P ( countSem );
		}
		exit ( 0 );
	}

	cashierPID = fork();											//Spawn process to emulate cashier
	if ( cashierPID == 0 )
	{
		while ( TRUE )
		{
			P ( cashierSem );
			V ( printSem3 );
		}
		exit ( 0 );
	}

	fork();												//If two barbers are requested.
	while ( TRUE )												//Barber is main process
	{
		P ( barberSem );
		P( chairSem );
		printf ( "Barber %d awakened or there is a customer waiting. There are %d seats available.\n", getpid(), ++( *chairsCH ) );
		V ( chairSem );
		V ( printSem1 );
		P ( printSem2 );
		sleep ( randUser ( MAX_RAND_BARB ) );
		printf ( "### %d Finished giving haircut to customer. That is haircut number %d today. . . a busy day.\n", getpid(), ++(*cutsCH) );
		V ( cashierSem );
	}

	return 0;
}

void USR1Handler ( int theUSR1 )
{
	printf ( "****** Barber received USR1 smoke signal ******\n" );
	printf ( "******    TOTAL NUMBER OF HAIRCUTS: %d    ******\n", *cutsCH );
	printf ( "******       CLOSE SHOP AND GO HOME      ******\n" );
	EZIPC_SHM_REMOVE();
	EZIPC_SEM_REMOVE();
	exit ( 0 );
}

int randUser ( int limit)
{
	int x = rand() % limit;
	return x;
}
