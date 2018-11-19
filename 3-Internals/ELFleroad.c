#include "ELFleroad.h"
#include <errno.h>								// errno
#include <fcntl.h>								// O_RDONLY
#include "Fileroad.h"
#include "Harklerror.h"
#include <string.h>								// strstr()
#include <sys/mman.h>							// mmap(), PROT_READ
#include <sys/stat.h>							// open()


void *map_file(char *filename)
{
	// LOCAL VARIABLES
	void *retVal = NULL;
	int fileDesc = 0;  // File descriptor for filename
	struct stat fileStat;  // Store the file statistics here
	int errNum = 0;  // Store errno here
	
	// INPUT VALIDATION
	if (filename && *filename)
	{
		// MAP IT
		// 1. Get file descriptor
		fileDes = open(filename, O_RDONLY);
		
		if (0 > fileDesc)
		{
			HARKLE_ERROR(ELFleroad, map_file, Unable to open filename);	
		}
		else
		{
			// 2. Get the file status
			if (fstat(fileDesc, &fileStat))
			{
				HARKLE_ERROR(ELFleroad, map_file, Unable to fstat filename);	
			}
			else
			{
				if (0 >= fileStat.st_size)
				{
					HARKLE_ERROR(ELFleroad, map_file, Invalid file size);	
				}
				else
				{
					retVal = mmap(NULL, fileStat.st_size, PROT_READ, MAP_SHARED, fileDesc, 0);
					
					if (!retVal || MAP_FAILED == retVal)
					{
						errNum = errno;
						retVal = NULL;
						HARKLE_ERROR(ELFleroad, map_file, mmap failed);
						HARKLE_ERRNO(ELFleroad, mmap, errNum);
					}
				}
			}
		}
	}
	
	// DONE
	return retVal;
}


bool is_elf(void *fileCont)
{
	// LOCAL VARIABLES
	bool retVal = false;
	char magicNum[] = { 0x7F, 'E', 'L', 'F', 0x0 };

	// INPUT VALIDATION
	if (fileCont && *fileCont)
	{
		// CHECK FILE
		if (fileCont == strstr(fileCont, magicNum))
		{
			retVal = true;	
		}
	}	
	else
	{
		HARKLE_ERROR(ELFleroad, is_elf, Bad input);
	}

	// DONE
	return retVal;
}
