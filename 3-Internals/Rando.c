#include <time.h>
#include <stdlib.h>


void rando_init(void)
{
	// LOCAL VARIABLES
	static int _rando_init = 0;  // Init variable

	// CHECK INIT VAR
	if (!_rando_init)
	{
		srand(clock());
		_rando_init = 1;
	}

	// DONE
	return;
}


int rando_me(int low, int high)
{
	// LOCAL VARIABLES
	int retVal = 0;

	// INPUT VALIDATION
	if ((high - low) == 0)
	{
		retVal = -1;			// Div by 0 error
	}
	else if (high == low)
	{
		retVal = -1;
	}
	else if (low > high)
	{
		retVal = -1;			// Input error
	}

	// PSUEDO RANDOM NUMBER
	if (-1 != retVal)
	{
		// Initialize the random number generator
		rando_init();

		// retVal = 42;  // DEBUGGING
		retVal = (rand() % (high - low)) + low;
	}

	// DONE
	return retVal;
}
