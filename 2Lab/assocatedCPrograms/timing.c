#include <stdio.h>
#include <stdlib.h>

int pid;

int main()
{
	printf ( "hello " );
	pid = fork();
	if ( pid == 0 )
	{
		printf ( "hello world\n");
		exit(0);
	}
	printf ( "world\n" );
	return 0;
}
