#include <stdio.h>
#include <stdlib.h>

int main()
{
	pid_t forkID;
	int * x = malloc(sizeof(int));
	*x = 5;
	printf ( "outside fork %d\n", x );
	forkID = fork();

	if ( forkID == 0 )
	{
		sleep(2);
		printf ( "child\n" );
		(*x)++;
		printf ( "%d\n", *x );
		printf( "%d\n", x );
		exit( 0 );
	}
	else
	{
		printf ( "parent\n" );
		*x = 30;
		printf ( "%d\n", *x );
		printf ( "%d\n", x );
	}
	wait ( forkID );
	return 0;
}
	


