/*
1- Consider the analogy of a tunnel with only a single lane. To avoid deadlock, cars must be prevented from entering the tunnel at both ends simultaneously.  Once a car enters, other cars from the same direction may follow immediately. 

Write the code using semaphores to solve the problem.
*/

ANSWER: Below I present two solutions to the problem, one randomly generating traffic and one that assumes the presence of queues of traffic created for me. 
	The first solution is consists of the commented out code in the beginning of the leftTraffic() and rightTraffic() functions. In this code, I generate a random number of cars that need to use the tunnel in one direction ( up to 10 cars ), sleep 1 second for each car that goes through tunnel, and sleep up to 5 seconds before requesting tunnel again. The use of the tunnel is guarded by a binary semaphore. I have commented out this code at the beginning of the leftTraffic() and rightTraffic().

	In the second solution, I assume the existence of two queues, leftCars and rightCars. These queues contain cars, a struct that has at least two member functions, isEmpty() and remove(). isEmpty() returns true if the queue is empty and false if not. remove() takes first car in the queue out of the queue. Cars are added to the queues throughout the program by someother means.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ezipc.h"

#define SIMULATE 1
#define TIME_TO_TRAVEL 2

void leftTraffic();					// This function will simulate traffic through the tunnel coming from the left
void rightTraffic();				// This function will simulate traffic through the tunnel coming from the right
int tunnel;							// This semaphore is declared outside of the main so that the simulation functions can access it

int main ()
{
	srand ( time ( NULL) );

	SETUP();
	tunnel = SEMAPHORE ( SEM_BIN, 1);		// Creates a counting semaphore with initial value 1
	int status;
	pid_t left, right, child;

	left = fork();
	if ( left == 0 )
	{
		leftTraffic();
	}

	right = fork();
	if ( right == 0 )
	{
		rightTraffic();
	}

	while ( (child = wait ( &status )) != -1 )
	{
		printf( "child pid: %d\n", child );
	}
	printf( "Main exiting\n" );
	exit ( 0 );
}

void leftTraffic ()
{
	queue leftCars;						// This queue hold cars that want to go left. It is filled for us throughout execution of the program.

	while ( 1 )
	{
		/*
		sleep ( rand () % 6 );									// Wait up to five seconds before asking for tunnel again.
		P ( tunnel );
		int cars = rand() % 10 + 1;								//Maximum of 10 cars in one direction
		printf ( "%d cars travelling left!\n", cars );
		sleep ( cars );											//Assume each car takes 1 second to travel through tunnel
		V ( tunnel );
		*/

		if ( !leftCars.isEmpty() )					// Assume queue has a member function isEmpty() that returns true if queue is empty, false otherwise.
		{
			P ( tunnel );
			while ( !leftCars.isEmpty() )
			{
				leftCars.remove();					// Assume queue has a member function remove() that removes the first car in the queue.
				sleep ( TIME_TO_TRAVEL );			// Assume each car takes a predetermined time to travel through tunnel, and assume that this predetermined time has been stored in the variable TIME_TO_TRAVEL
			}
			V ( tunnel);
		}											// We assumed that queue would be filled for us.
	}
	exit ( 0 );
}

void rightTraffic ()
{
	queue rightCars;						// This queue hold cars that want to go left. It is filled for us throughout execution of the program.

	while ( 1 )
	{
		/*
		sleep ( rand () % 6 );									// Wait up to five seconds before asking for tunnel again.
		P ( tunnel );
		int cars = rand() % 10 + 1;								//Maximum of 10 cars in one direction
		printf ( "%d cars travelling left!\n", cars );
		sleep ( cars );											//Assume each car takes 1 second to travel through tunnel
		V ( tunnel );
		*/

		if ( !rightCars.isEmpty() )					// Assume queue has a member function isEmpty() that returns true if queue is empty, false otherwise.
		{
			P ( tunnel );
			while ( !rightCars.isEmpty() )
			{
				rightCars.remove();					// Assume queue has a member function remove() that removes the first car in the queue.
				sleep ( TIME_TO_Travel );			// Assume each car takes a predetermined time to travel through tunnel, and assume that this predetermined time has been stored in the variable TIME_TO_TRAVEL
			}
			V ( tunnel);
		}											// We assumed that queue would be filled for us.
	}
	exit ( 0 );
}





/*
2- Consider the following definition of a semaphore:

void  P(s)
{
	if (s > 0 ) {
		s--;
	}
	else {
		block process on semaphore s
	}
}
void V(s)
{
	if (there is a process waiting for semaphore s) {
		wake up process
	}
	else {
		s++;
	}
}

Compare to the definition in Figure 5.3 (textbook). Is there any difference in the effect of the two sets of definitions when used in a program? That is, could you substitute one set for the other without altering the meaning of the program?
*/

ANSWER: There is no difference in effect of the two sets of definitions when used in a program. In the definition of a basic semaphore in the book however, the semaphore can be decremented below 0 therefore keeping track of the number of programs that have requested use of the semaphore but have been blocked. The definition above does not allow the value of the semaphore to be less than zero; therefore the semaphore above cannot keep track of the number of blocked processes. The value of the semaphore above represents the number of resourses availble, while the semaphore in the book keep track of the resources available ( while s >= 0 ) and the number of processes waiting on the resource ( s < 0 ).





/*
3- Let a bounded semaphore s be a general semaphore that cannot exceed a given value smax > 0 .  The corresponding operations  PB and VB are defined as follows:

    PB(s):  wait until s > 0; then decrement s by 1
    VB(s):  wait until s < smax; then increment s by 1.
 
Write a monitor, named s,  such that the calls s.PB() and s.VB() to emulate the operations on bounded semaphores,
*/

ANSWER: I used a struct to represent the monitor emulating the bounded semaphore. Two functions acts as the PB and VB operations. Both of these functions take a pointer to the struct so that the value of the function parameter can be changed in the function. A semaphore made by the EZIPC library guards the semaphores so that only one function ( PB or VB ) can be executed at once. The notation s.PB() is not possible in C as structs cannot have member functions. 


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

