#include <stdio.h>

int main()
{
	FILE * fp;
	fp = fopen ( "lab2.txt", "w");
	int i = i;
	while ( i < 4001 )
	{	
		fprintf ( fp, "This is line number %d!\n", i );
		i++;
	}
	return 0;
}
