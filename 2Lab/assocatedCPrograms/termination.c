#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	int save_status, i, status;
	pid_t child_pid;
	for ( i = 0; i < 3; i++ )
	{
		if ( fork() == 0 )
		{
			sleep ( 1 );
			exit ( i );
		}
	}
	
	while (( child_pid = wait( &save_status )) != -1 )
	{
		status = save_status >> 8;
		printf ( "Child pid:  %d with status %d\n", child_pid, status );
	}
	return 0;	
}
