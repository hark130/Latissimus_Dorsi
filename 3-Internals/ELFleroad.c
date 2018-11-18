#include "ELFleroad.h"
#include "Harklerror.h"

bool is_elf(char *filename)
{
	// LOCAL VARIABLES
	bool retVal = false;

	// INPUT VALIDATION
	if (filename && *filename)
	{
		// CHECK FILE	
	}	
	else
	{
		HARKLE_ERROR(ELFleroad, is_elf, Bad input);
	}

	// DONE
	return retVal;
}
