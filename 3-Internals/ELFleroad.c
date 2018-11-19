#include "ELFleroad.h"
#include "Fileroad.h"
#include "Harklerror.h"

bool is_elf(char *filename)
{
	// LOCAL VARIABLES
	bool retVal = false;
	char *fileCont = NULL;
	char magicNum[] = { 0x7F, 'E', 'L', 'F', 0x0 };

	// INPUT VALIDATION
	if (filename && *filename)
	{
		// CHECK FILE
		fileCont = fread_a_file(filename);
		
		if (fileCont)
		{
			if (*fileCont)
			{
				if (!strcmp(fileCont, magicNum))
				{
					retVal = true;	
				}
			}
		}
		else
		{
			HARKLE_ERROR(ELFleroad, is_elf, fread_a_file failed to read a file);
		}
	}	
	else
	{
		HARKLE_ERROR(ELFleroad, is_elf, Bad input);
	}

	// DONE
	return retVal;
}
