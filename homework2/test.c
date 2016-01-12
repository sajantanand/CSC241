#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "ezipc.h"

#define smax 10

int sema;

struct monitor
{
	int x;
};

struct monitor monitor ( int value )
{
	struct monitor temp;
	temp.x = value;
	return temp;
}

void PB ( struct monitor * sem )
{
	P ( sema );

	while ( sem->x <= 0 )
		{}
	sem->x--;

	V ( sema );
}

void VB( struct monitor * sem )
{
	P ( sema );

	while ( sem->x >= smax)
		{}
	sem->x++;

	V ( sema );
}

int main()
{
	SETUP();
	
	sema = SEMAPHORE ( SEM_BIN, 1 );	

	struct monitor try = monitor ( 5 );

	printf("%d\n", try.x);
	
	VB( &try );

	printf("%d\n", try.x);

	exit ( 0 );
}