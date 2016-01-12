#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "/home/csuser/ezipc.h"
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
		printf ( "Incorrect arguements. Correct usage: ./%s FILENAME\n", argv [ 0 ] );
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
		while ( iBuffer != -1 )
		{
			P ( buff1SemA );
			//P ( buff2SemA );
			//printf ( "%c", *buff1CH );
			//fflush ( stdout );
			iBuffer = *buff1CH;
			V ( buff1SemB );
				
			if ( iBuffer == '*' && mBuffer == '*' )
			{
				mBuffer = '#';
				iBuffer = 0;
			}
			P ( buff2SemA );
			/*
			if ( iBuffer == '*' && *buff2CH == '*' ) 
			{
				//printf ( "Found asterisk!\n" );
				*buff2CH = '#';
				iBuffer = 0;
			}
			*/

			*buff2CH = mBuffer;
			mBuffer = iBuffer;
			//V ( buff1SemB );
			V ( buff2SemB );
		}
		P ( buff2SemA );
		*buff2CH = -1;
		V ( buff2SemB );
		
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
			if ( *buff2CH > 0 )
			{
				printf ( "%c", *buff2CH );
				if ( i % 25 == 0 )
				{
					printf ( "\n" );
				}
				fflush ( stdout );
				i++;
			}
			else if ( *buff2CH < 0 )
			//else
			{
				j = -1;
			}
			V ( buff2SemA );
		}
		exit ( 0 );
	}
	
	ch = getc ( ifp );

	//V ( buff1Sem );
	//printf ( "Start of reading\n" );
	//printf ( "Char: %c\n", ch );
	while ( ch != EOF )
	{
		//printf ( "Char: %c ", ch );
		fflush ( stdout );
		if ( ch != 10 )				// In ASCII 10 is a line feed
		{
			P ( buff1SemB );
			//printf ( "%c", ch );
			//fflush ( stdout );
			*buff1CH = ch;
			V ( buff1SemA );
			//P ( buff1SemB );
			if ( count % CARD_LENGTH == 0 && count != 0)
			{
				//printf ( "%c\n", '|' );
				P ( buff1SemB );
				*buff1CH = '<';             //EOL
				V ( buff1SemA );
				//P ( buff1SemB );
				P ( buff1SemB );
				*buff1CH = 'E';             //EOL
				V ( buff1SemA );
				//P ( buff1SemB );
				P ( buff1SemB );
				*buff1CH = 'O';             //EOL
				V ( buff1SemA );
				//P ( buff1SemB );
				P ( buff1SemB );
				*buff1CH = 'F';             //EOL
				V ( buff1SemA );
				//P ( buff1SemB );
	
				P ( buff1SemB );
				*buff1CH = '>';             //EOL
				V ( buff1SemA );
				//P ( buff1SemB );
			}
			count++;
		}
		ch = getc ( ifp );
	}
	
	//printf ( "Out of reading\n" );
	P ( buff1SemB );
	//*buff1CH = '';
	*buff1CH = -1;
	V ( buff1SemA );
	
	while ( wait() != -1 );

	printf ( "\n" );
	EZIPC_SHM_REMOVE();
	EZIPC_SEM_REMOVE();
	return 0;
}
	
	
