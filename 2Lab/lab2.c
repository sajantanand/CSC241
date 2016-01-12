#include <stdio.h>
#include <stdlib.h>
#define FNAME "lab2.txt"
int main()
{
	char * lineC;
	char * lineP;
	FILE * ifp;
	//ssize_t statusC, statusP;
	size_t  sizeC,  sizeP;
	pid_t id;
	int num;
	
	ifp = fopen( FNAME, "r" );

	if ( ifp == NULL )
	{
		printf("File '%s' not found.\n", FNAME);
		exit(1);
	}

	int procid = fork();

	if ( procid < 0 )
	{
		printf( "Fork was not successful! Please try again. . .\n" );
		exit( 1 );
	}
	else if( procid == 0 )
	{
		//sleep(2);
		num = 0;
		id = getpid();
		//printf ( "ID of child:  %d\n", id );
		while ( getline ( &lineC, &sizeC, ifp ) != EOF )
		{
			num++;
			printf ( "%d child:  %s", id, lineC );
			fflush ( stdout );
		}
		printf ( "Total lines read by child:  %d\n", num );
		//fflush ( stdout );
		exit(0);
	}
	else
	{	
		//sleep(2);
		num = 0;
		id = getpid();
		//printf ( "ID of parent:  %d\n", id );
		while ( getline ( &lineP, &sizeP, ifp ) != EOF )
		{
			num++;
			printf( "%d parent:  %s", id, lineP );
			fflush ( stdout );
		}
		printf ( "Total lines read by parent:  %d\n", num );
		//fflush ( stdout );
	}

	return 0;
}
