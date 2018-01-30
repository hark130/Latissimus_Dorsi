#include "Fileroad_Descriptors.h"

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
    
    // DONE
    return retVal;
}


/*
	Purpose - Open a file and populate a fileDescriptorDetails struct
	Input - Filename to open
	Output - Pointer to a fileDescriptorDetails struct
	Notes:
		fdDetails_ptr must be free()'d by the calling function
		fdDetails_ptr->filename_ptr must be free()'d by the calling function
        This function calls create_fdDetails_ptr() to allocate a struct
 */
fdDetails_ptr open_fd(const char* filename)
{
    // LOCAL VARIABLES
    fdDetails_ptr retVal = NULL;
    
    // DONE
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
    // INPUT VALIDATION
    
    
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
