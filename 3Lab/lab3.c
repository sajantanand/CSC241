#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char * line;
	char * portion;
	char ** commands;
	int * status_code;
	unsigned long n, r;

	line = portion = NULL;
	commands = NULL;
	status_code = NULL;
	int arguements;
	pid_t child;
	
	do {
		n = r = 0;
		arguements = 0;
		child = 0;
		printf ( "sajant%% " );
		fflush ( stdout );
		r = getline ( &line, &n, stdin );

		portion = strtok( line, " \n" );
		
		while ( portion != NULL )
		{
			commands = realloc ( commands, sizeof ( char * ) * ( arguements + 2 ));
			commands [ arguements++ ] = portion;
			portion = strtok( NULL, " \n" );
		}
		commands [ arguements ] = NULL;
		if ( arguements > 0 && strcmp ( commands [ arguements - 1 ], "&" ) == 0 )
		{
			commands [ arguements - 1 ] = NULL;
				
			child = fork();
			if ( child == 0 )
			{
				int status = execvp ( commands [ 0 ], commands );
				perror ( "exec failure " );
				exit ( status  );
			}
		}
		else if ( arguements > 0 )
		{
			child = fork();
			if ( child == 0 )
			{
				//printf ( "child id: %d\n", child );
				int status = execvp ( commands [ 0 ], commands );
				perror ( "exec failure " );
				exit ( status );
			}

			waitpid ( child, status_code, 0 );
		}
	} while ( r != EOF ) ;

	free ( line );
	free ( portion );
	free ( commands );
	return 0;
}
