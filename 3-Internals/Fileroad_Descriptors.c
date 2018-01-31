#include "Fileroad_Descriptors.h"
#include <fcntl.h>			// open(), open() flags
#include <stdio.h>          // fprintf
#include <string.h>         // memset
#include <sys/stat.h>		// mode_t

#ifndef MAX_TRIES
// MACRO to limit repeated allocation attempts
#define MAX_TRIES 3
#endif  // MAX_TRIES

/*
typedef struct fileDescriptorDetails
{
    char* filename_ptr;     // Path to file
    int fileDesc;           // File descriptor
    uintmax_t fileSize;     // Actual size of file
    uintmax_t diskSize;     // Size of file on disk
} fdDetails, *fdDetails_ptr;
 */


/*
	Purpose - Dynamically allocate a struct pointer
	Input - None
	Output - Pointer to a fileDescriptorDetails struct on the heap
	Notes:
		fdDetails_ptr must be free()'d by the calling function
 */
fdDetails_ptr create_fdDetails_ptr(void)
{
    // LOCAL VARIABLES
    fdDetails_ptr retVal = NULL;
	int numTries = 0;

	// ALLOCATE MEMORY
	while(numTries < MAX_TRIES && retVal == NULL)
	{
		retVal = (fdDetails_ptr)calloc(1, sizeof(fdDetails));
		numTries++;
    }
    
    // DONE
    return retVal;
}


/*
	Purpose - Open a file and populate a fileDescriptorDetails struct
	Input
		fname - Filename to open
		flags - Access modes: O_RDONLY, O_WRONLY, or O_RDWR
		mode - A bitwise OR of file creation flags and file status flags
	Output - Pointer to a fileDescriptorDetails struct
	Notes:
		fdDetails_ptr must be free()'d by the calling function
		fdDetails_ptr->filename_ptr must be free()'d by the calling function
        This function calls create_fdDetails_ptr() to allocate a struct
 */
fdDetails_ptr open_fd(const char* fname, int flags, mode_t mode)
{
    // LOCAL VARIABLES
    fdDetails_ptr retVal = NULL;
	bool success = true;
	size_t len = 0;  // Length of filename
	char* temp_ptr = NULL;  // Holds return value from strncpy()
	
	// INPUT VALIDATION
	// 1. Flags
	if (flags != O_RDONLY &&
		flags != O_WRONLY &&
		flags != O_RDWR &&
	    success == true)
	{
		success = false;
		fprintf(stderr, "<<<ERROR>>> - open_fd() - bad flags parameter\n");  // DEBUGGING
	}
	
	// 2. Mode
	if (!(mode >= 0) && success == true)
	{
		success = false;
		fprintf(stderr, "<<<ERROR>>> - open_fd() - bad mode parameter\n");  // DEBUGGING
	}
	
	// 3. Filename
	if (fname != NULL && success == true)
	{
		// OPEN FILE TO GET A FILE DESCRIPTOR
		// 1. Create a fileDescriptorDetails struct
		retVal = (fdDetails_ptr)create_fdDetails_ptr();

		// 2. Get the filename length
		len = strlen(fname);
		
		if (retVal != NULL)
		{
			// 3. Get the file descriptor
			retVal->fileDesc = open(fname, flags, mode);
			// NOTE TO THE FUTURE:  Logic branch based on creation intent or not?
			// open(char*, flags) vs open(char*, flags, mode)?
			
			if (retVal->fileDesc < 0)
			{
				success = false;
				fprintf(stderr, "<<<ERROR>>> - open_fd() - open() system call failed:\t%d\n", retVal->fileDesc);	
			}
			else if (retVal->fileDesc < 3)
			{
				success = false;
				fprintf(stderr, "¿¿¿ERROR??? - open_fd() - open() system call returned a standard POSIX file descriptor:\t%d\n", retVal->fileDesc);		
			}
			else
			{
				// 4. Allocate and copy the filename into the struct
				retVal->filename = (char*)calloc(strlen(fname) + 1, sizeof(char));

				if (retVal->filename != NULL)
				{
					temp_ptr = (char*)strncpy(retVal->filename, fname, len);

					if (temp_ptr != retVal->filename)
					{
						success = false;
						fprintf(stderr, "<<<ERROR>>> - open_fd() - strncpy() failed!\n");
					}
					else
					{
						retVal->fileSize = get_file_len(retVal->fileDesc);

						if (retVal->fileSize == -1)
						{
							success = false;
							fprintf(stderr, "<<<ERROR>>> - open_fd() - get_file_len() failed!\n");		
						}
					}
				}
				else
				{
					success = false;
					fprintf(stderr, "<<<ERROR>>> - open_fd() - calloc() failed!\n");
				}
			}
		}
	}
	else
	{
		success = false;
		fprintf(stderr, "<<<ERROR>>> - open_fd() - NULL filename parameter\n");  // DEBUGGING	
	}
    
    // DONE
	if (success == false)
	{
		// Something failed.  Undo everything!
		// 1. Close the file descriptor
		if (retVal != NULL)
		{
			if (retVal->fileDesc > 2)
			{
				fclose(retVal->fileDesc);
			}
		}
		
		// 2. Free the struct
		free_fdDetails(&retVal);
	}

    return retVal;
}


/*
	Purpose - Free a fileDescriptorDetails struct on the heap
	Input - Pointer to a fileDescriptorDetails pointer to free
	Output - None
	Notes:
		Will memset(0x0), free, and NULL filename_ptr
		Will zeroize all other members
		Closing the file descriptor is the responsibility of the
            calling function
 */
void free_fdDetails(fdDetails_ptr* oldStruct_ptr)
{
    // LOCAL VARIABLES
    fdDetails_ptr tempStruct_ptr = NULL;
    void* temp_ptr = NULL;
    size_t len = 0;
    
    // INPUT VALIDATION
    if (oldStruct_ptr != NULL)
    {
        if (*oldStruct_ptr != NULL)
        {
            tempStruct_ptr = (fdDetails_ptr)*oldStruct_ptr;
            
            // char* filename_ptr;     // Path to file
            if (tempStruct_ptr->filename_ptr != NULL)
            {
                // Get the filename length
                len = strlen(tempStruct_ptr->filename_ptr);
                    
                // Memset
                if (len > 0)
                {
                    temp_ptr = memeset(tempStruct_ptr->filename_ptr, 0x0, len);
                    
                    if (temp_ptr != tempStruct_ptr->filename_ptr)
                    {
						// DEBUGGING
                        fprintf(stderr, "<<<ERROR>>> - free_fdDetails() - memset failed to zeroize the struct's filename_ptr\n");
                    }
                }
                
                // Free
                free(tempStruct_ptr->filename_ptr);
                
                // NULL
                tempStruct_ptr->filename_ptr = NULL;
            }
            
            // int fileDesc;           // File descriptor
            tempStruct_ptr->fileDesc = 0;
            
            // uintmax_t fileSize;     // Actual size of file
            tempStruct_ptr->fileSize = 0;
            
            // uintmax_t diskSize; // Size of file on disk
            // tempStruct_ptr->diskSize = 0;
            
			// 2. FREE/CLEAR STRUCT
			// Free the struct pointer
			free((void*)*oldStruct_ptr);
			// Clear the struct pointer
			tempStruct_ptr = NULL;
            *oldStruct_ptr = NULL;
        }
    }
    
    // DONE
    return;
}


/*
	Purpose - Validate the contents of a fileDescriptorDetails struct pointer
	Input - Pointer to a fileDescriptorDetails struct
	Output - true if valid, otherwise false
	Notes:
		Can not validate memType against the type of memory allocated
		Merely checks that memType is valid
 */
bool validate_fdDetails(fdDetails_ptr checkThis_ptr)
{
    // LOCAL VARIABLES
    bool retVal = true;
    
    // DONE
    return retVal;
}


/*
    Purpose - Update the details of a fileDescriptorDetails struct pointer
    Input - Pointer to a fileDescriptorDetails struct
    Output - 0 on success, error code on failure
 */
int update_fdDetails(fdDetails_ptr updateThis_ptr)
{
    // LOCAL VARIABLES
    int retVal = 0;
    
    // DONE
    return retVal;
}


/*
	Purpose - Determine the file size of a file descriptor
	Input - An open file descriptor
	Output - Size of the file descriptor
	Notes:
		This function will rewind() the file descriptor
 */
long get_file_len(int fileDesc)
{
	// LOCAL VARIABLES
	long retVal = 0;

	// INPUT VALIDATION
	if (fileDesc >= 0)
	{
		// GET LENGTH
	    // Move file point at the end of file
	    fseek(fileDesc, 0, SEEK_END);
	     
	    // Get the current position of the file pointer
	    retVal = ftell(fp);
	     
	    if(retVal == -1)
	    {
	    	fprintf(stderr, "<<<ERROR>>> - get_file_len() - fseek()/ftell() failed!\n");
	    }    
	}
	// DONE
	return retVal;
}


/*
// For later
int fd_is_valid(int fd)
{
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}
 */
