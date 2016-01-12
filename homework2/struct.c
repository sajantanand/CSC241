#include <stdio.h>
#include <stdlib.h>
#define smax 5

struct Monitor
{
	int sem;

	Monitor ( int value )
	{
		sem = value;
	}

	void PB()
	{
		while ( sem <= 0 )
		{}
		sem--;
	}

	void VB()
	{
		while ( sem >= smax)
		{}
		sem++;
	}
};

int main () 
{
	struct Monitor sajant(4);
	//sajant.sem = 5;
	sajant.VB();
	printf("%d\n", sajant.sem);
	return 0;
}