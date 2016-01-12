#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//#include "/home/csuser/ezipc.h"
#include "ezipc.h"

#define CARD_LENGTH 20
#define EOL "<EOL>"
#define TRUE 1

int buff1SemA, buff1SemB, buff2SemA, buff2SemB;

int main ( int argc, char ** argv )
{
	SETUP ();

	char * buff1CH, * buff2CH;
	int ch, count;
	FILE * ifp;
	pid_t squash, print;

	buff1SemA = SEMAPHORE ( SEM_BIN, 0 );
	buff1SemB = SEMAPHORE ( SEM_BIN, 1 );
	buff2SemA = SEMAPHORE ( SEM_BIN, 1 );
	buff2SemB = SEMAPHORE ( SEM_BIN, 0 );
	buff1CH = SHARED_MEMORY ( 1 );
	buff2CH = SHARED_MEMORY ( 1 );
	
	ch = count = 1;

	if ( argc != 2 )
	{
		printf ( "Incorrect arguements. Correct usage: %s FILENAME\n", argv [ 0 ] );
		exit ( 1 );
	}
	
	ifp = fopen ( argv [ 1 ], "r" );
	if ( ifp == NULL )
	{
		printf ( "File '%s' not found.\n", argv [ 1 ] );
		exit ( 1 );
	}

	squash = fork ();
	if ( squash == 0 )
	{
		char iBuffer = 0;
		char mBuffer = 0;
		int noQuit = 1;
		while ( noQuit )
		{
			P ( buff1SemA );
			iBuffer = *buff1CH;
			noQuit = iBuffer;
			V ( buff1SemB );
				
			if ( iBuffer == '*' && mBuffer == '*' )
			{
				mBuffer = '#';
				iBuffer = 0;
			}
			P ( buff2SemA );

			*buff2CH = mBuffer;
			mBuffer = iBuffer;
			V ( buff2SemB );
		}
		P ( buff2SemA );
		*buff2CH = 10;					//10 is ASCII for Line Feed. A line feed will never be found in the file by squash, as the reader process removes all line feeds.
		V ( buff2SemB );				//Thus I can use a line feed to signal that all input has been read.
		
		exit ( 0 );
	}
	
	print = fork();
	if ( print == 0 )
	{
		int i = 1;
		int j = 1;
		while ( j != -1 )
		{
			P ( buff2SemB );
			if ( *buff2CH != 10 && *buff2CH != 0)
			{
				printf ( "%c", *buff2CH );
				if ( i % 25 == 0 )
				{
					printf ( "\n" );
				}
				fflush ( stdout );
				i++;
			}
			else if ( *buff2CH == 10 )
			{
				j = -1;
			}
			V ( buff2SemA );
		}
		exit ( 0 );
	}
	
	ch = getc ( ifp );

	while ( ch != EOF )
	{
		fflush ( stdout );
		if ( ch != 10 )				// In ASCII 10 is a line feed
		{
			P ( buff1SemB );
			*buff1CH = ch;
			V ( buff1SemA );
			if ( count % CARD_LENGTH == 0 && count != 0)
			{
				P ( buff1SemB );
				*buff1CH = '<';             //EOL
				V ( buff1SemA );

				P ( buff1SemB );
				*buff1CH = 'E';             //EOL
				V ( buff1SemA );

				P ( buff1SemB );
				*buff1CH = 'O';             //EOL
				V ( buff1SemA );

				P ( buff1SemB );
				*buff1CH = 'L';             //EOL
				V ( buff1SemA );

				P ( buff1SemB );
				*buff1CH = '>';             //EOL
				V ( buff1SemA );
			}
			count++;
		}
		ch = getc ( ifp );
	}
	
	P ( buff1SemB );
	*buff1CH = 0;
	V ( buff1SemA );
	
	while ( wait() != -1 );

	printf ( "\n" );
	EZIPC_SHM_REMOVE();
	EZIPC_SEM_REMOVE();
	return 0;
}
	
	
