#include <elf.h>
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


int determine_elf_class(void *fileCont)
{
	// LOCAL VARIABLES
	int retVal = -1;
	char *tmp_ptr = (char *)fileCont;
	
	// INPUT VALIDATION
	if (fileCont && *fileCont)
	{
		if (false == is_elf(fileCont))
		{
			retVal = ELFCLASSNONE;	
		}
		else
		{
			// READ IT
			retVal = (int)(tmp_ptr[4]);
			
			// VALIDATE IT
			if (ELFCLASSNONE != retVal && ELFCLASS32 != retVal && ELFCLASS64 != retVal)
			{
				retVal = -1;	
			}
		}
	}
	else
	{
		HARKLE_ERROR(ELFleroad, determine_elf_class, Bad input);	
	}
	
	// DONE
	return retVal;
}


bool unmap_file(void **oldMem_ptr, size_t memSize)
{
	// LOCAL VARIABLES
	bool retVal = false;
	int errNum = 0;  // Store errno here
	void *tmp_ptr = NULL;  // Store *oldMem_ptr here
	
	// INPUT VALIDATION
	if (!oldMem_ptr)
	{
		HARKLE_ERROR(ELFleroad, unmap_file, NULL pointer);
	}
	else if (0 >= memSize)
	{
		HARKLE_ERROR(ELFleroad, unmap_file, Invalid memory size);
	}
	else
	{
		tmp_ptr = *oldMem_ptr;
		
		if (!tmp_ptr)
		{
			HARKLE_ERROR(ELFleroad, unmap_file, NULL mapped pointer);
		}
		else
		{
			if (munmap(tmp_ptr, memSize))
			{
				errNum = errno;
				HARKLE_ERROR(ELFleroad, unmap_file, munmap failed);
				HARKLE_ERRNO(ELFleroad, munmap, errNum);
			}
			else
			{
				*oldMem_ptr = NULL;
				retVal = true;
			}
		}
	}
}
