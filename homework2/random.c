#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "ezipc.h"

#define smax 10

int monitor;

struct test
{
	int x;
};

struct test test ( int value )
{
	struct test temp;
	temp.x = value;
	return temp;
}

void PB ( struct test * sem )
{
	P ( monitor );

	while ( sem->x <= 0 )
		{}
	sem->x--;

	V ( monitor );
}

void VB( struct test * sem )
{
	P ( monitor );

	while ( sem->x >= smax)
		{}
	sem->x++;

	V ( monitor );
}

int main()
{
	SETUP();
	
	monitor = SEMAPHORE ( SEM_BIN, 1 );	

	struct test try = test ( 5 );

	printf("%d\n", try.x);
	
	VB( &try );

	printf("%d\n", try.x);

	exit ( 0 );
}
