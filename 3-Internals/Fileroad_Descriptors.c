#include "Fileroad.h"		// os_path_basename, os_path_dirname
#include "Fileroad_Descriptors.h"
#include <fcntl.h>			// open, open flags
#include "Harklerror.h"		// HARKLE_ERROR
#include "Memoroad.h"		// release_a_string, copy_a_string
#include <stdio.h>          // fprintf
#include <stdlib.h>			// calloc
#include <string.h>         // memset
#include <sys/stat.h>		// mode_t
#include <unistd.h>			// close, pid_t

#ifndef FD_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define FD_MAX_TRIES 3
#endif  // FD_MAX_TRIES

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// rBinDat FUNCTIONS START //////////////////////////
//////////////////////////////////////////////////////////////////////////////


rBinDat_ptr create_rBinDat_ptr(void)
{
    // LOCAL VARIABLES
    rBinDat_ptr retVal = NULL;
	int numTries = 0;

	// ALLOCATE MEMORY
	while(numTries < FD_MAX_TRIES && retVal == NULL)
	{
		retVal = (rBinDat_ptr)calloc(1, sizeof(rBinDat));
		numTries++;
    }
    
    // DONE
    return retVal;
}


rBinDat_ptr build_rBinDat_ptr(char* binaryName, char** binArgs)
{
	// LOCAL VARIABLES
	rBinDat_ptr retVal = NULL;
	char* justBinName = NULL;  // Pointer to the beginning of the binary name
	bool success = true;  // Make this false if anything fails
	
	// INPUT VALIDATION	
	if (!binaryName)
	{
		HARKLE_ERROR(Fileroad_Descriptors, build_rBinDat_ptr, NULL pointer);
		success = false;
	}
	else if (!(*binaryName))
	{
		HARKLE_ERROR(Fileroad_Descriptors, build_rBinDat_ptr, Empty binary filename);
		success = false;
	}
	else if (!binArgs)
	{
		HARKLE_ERROR(Fileroad_Descriptors, build_rBinDat_ptr, NULL pointer);
		success = false;
	}
	else if (!(*binArgs))
	{
		HARKLE_ERROR(Fileroad_Descriptors, build_rBinDat_ptr, NULL pointer);
		success = false;
	}
	
	// ALLOCATE STRUCT
	if (success == true)
	{
		retVal = create_rBinDat_ptr();
		
		if (!retVal)
		{
			HARKLE_ERROR(Fileroad_Descriptors, build_rBinDat_ptr, create_rBinDat_ptr failed);
			success = false;
		}
	}
	
// 	// PARSE NAME
// 	///////////////////////////////// IMPLEMENT LATER /////////////////////////////////
// 	justBinName = binaryName;  // This is just a placeholder
	
	// POPULATE BINARY NAME
	if (success == true)
	{
		// 1. char* binName; // Just the binary name
// 		retVal->binName = copy_a_string(justBinName);
		retVal->binName = os_path_basename(binaryName);
		
		if (!(retVal->binName))
		{
// 			HARKLE_ERROR(Fileroad_Descriptors, build_rBinDat_ptr, copy_a_string failed);
			HARKLE_ERROR(Fileroad_Descriptors, build_rBinDat_ptr, os_path_basename failed);
			success = false;
		}
		else
		{
			// 2. char* binPath;
			retVal->dirName = os_path_dirname(binaryName);
			
			if (!(retVal->dirName))
			{
// 				HARKLE_ERROR(Fileroad_Descriptors, build_rBinDat_ptr, copy_a_string failed);
				HARKLE_ERROR(Fileroad_Descriptors, build_rBinDat_ptr, os_path_dirname failed);
				success = false;
			}
			else
			{
				// 3. char** fullCmd; // argv[1]... do NOT free()!
				retVal->fullCmd = binArgs;
			}
		}		
		
		// NOTE: Other struct members are populated later
		// 4. char* outputFile; 	// File capturing binary's stdout		
		// 5. char* errorsFile; 	// File capturing binary's stderr
		// 6. int readPipe;			// Implement later... Binary (child) reads stdin from here
		retVal->readPipe = -1;  	// Set to an invalid file descriptor so they're not used
		
		// 7. int writePipe; 		// Implement later... redirect_bin_output.exe (parent) writes binary's input here
		retVal->writePipe = -1;  	// Set to an invalid file descriptor so they're not used
	}
	
	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == free_rBinDat_ptr(&retVal))
			{
				HARKLE_ERROR(Fileroad_Descriptors, build_rBinDat_ptr, free_rBinDat_ptr failed);
			}
		}
	}
	
	// DONE
	return retVal;
}


bool free_rBinDat_ptr(rBinDat_ptr* oldStruct_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Make this false if anything fails
	bool memoRetVal = true;  // Return value from calls to Memoroad functions
	rBinDat_ptr tempStruct_ptr = NULL;
	
	// INPUT VALIDATION
	if (oldStruct_ptr)
	{
		if (*oldStruct_ptr)
		{
			tempStruct_ptr = *oldStruct_ptr;
		}
		else
		{
			HARKLE_ERROR(Fileroad_Descriptors, free_rBinDat_ptr, NULL pointer);
			retVal = false;
		}
	}
	else
	{
		HARKLE_ERROR(Fileroad_Descriptors, free_rBinDat_ptr, NULL pointer);
		retVal = false;
	}
	
	// CLEAR STRUCT MEMBERS
	if (retVal == true)
	{
		// 1. char* binName; // Just the binary name
		if (tempStruct_ptr->binName)
		{
			memoRetVal = release_a_string(&(tempStruct_ptr->binName));
			
			if (memoRetVal == false)
			{
				HARKLE_ERROR(Fileroad_Descriptors, free_rBinDat_ptr, release_a_string failed on binName);
				retVal = false;
			}
		}
		
		// 2. char* binPath; // NOT YET IMPLEMENTED
		if (tempStruct_ptr->binPath)
		{
			memoRetVal = release_a_string(&(tempStruct_ptr->binPath));
			
			if (memoRetVal == false)
			{
				HARKLE_ERROR(Fileroad_Descriptors, free_rBinDat_ptr, release_a_string failed on binPath);
				retVal = false;
			}
		}
		
		// 3. char** fullCmd; // argv[1]... do NOT free()!
		if (tempStruct_ptr->fullCmd)
		{
			// NULL but do not free()
			tempStruct_ptr->fullCmd = NULL;
		}

		// 4. char* outputFile; // File capturing binary's stdout
		if (tempStruct_ptr->outputFile)
		{
			memoRetVal = release_a_string(&(tempStruct_ptr->outputFile));
			
			if (memoRetVal == false)
			{
				HARKLE_ERROR(Fileroad_Descriptors, free_rBinDat_ptr, release_a_string failed on outputFile);
				retVal = false;
			}
		}
		
		// 5. char* errorsFile; // File capturing binary's stderr
		if (tempStruct_ptr->errorsFile)
		{
			memoRetVal = release_a_string(&(tempStruct_ptr->errorsFile));
			
			if (memoRetVal == false)
			{
				HARKLE_ERROR(Fileroad_Descriptors, free_rBinDat_ptr, release_a_string failed on errorsFile);
				retVal = false;
			}
		}
		
		// 6. int readPipe; // Implement later... Binary (child) reads stdin from here
		tempStruct_ptr->readPipe = 0x0;

		// 7. int writePipe; // Implement later... redirect_bin_output.exe (parent) writes binary's input here
		tempStruct_ptr->writePipe = 0x0;
	}
	
	// FREE STRUCT
	if (oldStruct_ptr)
	{
		if (*oldStruct_ptr)
		{
			// 1. Free the struct pointer
			free(*oldStruct_ptr);
			
			// 2. NULL the struct pointer
			*oldStruct_ptr = NULL;
			tempStruct_ptr = NULL;
		}
	}
	
	// DONE
	return retVal;
}


int wrap_bin(rBinDat_ptr binToWrap)
{
	// LOCAL VARIABLES
	int retVal = -2;  // Default return value; Change after fork() call
	bool success = true;  // If anything fails, make this false
	pid_t forkPID = 0;  // Return value from fork()
	int errNum = 0;  // Store errno here on error
	int wStatus = 0;  // Information regarding the child process
	int outFileDesc = 0;  // File descriptor for binToWrap's outputFile file descriptor
	int errFileDesc = 0;  // File descriptor for binToWrap's errorsFile file descriptor
	int dupRetVal = 0;  // Return value from dup2() function call
	
	// INPUT VALIDATION
	if (!binToWrap)
	{
		HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, NULL pointer);
		success = false;
	}
	else if (!(binToWrap->fullCmd))
	{
		HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, NULL pointer);
		success = false;
	}
	else if (!(*(binToWrap->fullCmd)))
	{
		HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, NULL pointer);
		success = false;
	}
	else if (!(binToWrap->outputFile))
	{
		HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, NULL pointer);
		success = false;
	}
	else if (!(binToWrap->errorsFile))
	{
		HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, NULL pointer);
		success = false;
	}

	// OPEN FILES
	// 1. outputFile
 	// outFileDesc = open(binToWrap->outputFile, O_WRONLY | O_CREAT | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	outFileDesc = open(binToWrap->outputFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	
	if (outFileDesc == -1)
	{
		errNum = errno;
		HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, open failed);
		fprintf(stderr, "open(%s) failed with an errno of %d:\t%s\n", binToWrap->outputFile, errNum, strerror(errNum));
		success = false;
	}
	
	// 2. errorsFile
	// errFileDesc = open(binToWrap->errorsFile, O_WRONLY | O_CREAT | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	errFileDesc = open(binToWrap->errorsFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
	
	if (errFileDesc == -1)
	{
		errNum = errno;
		HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, open failed);
		fprintf(stderr, "open(%s) failed with an errno of %d:\t%s\n", binToWrap->errorsFile, errNum, strerror(errNum));
		success = false;
	}
	
	// FORK
	if (success == true)
	{
		forkPID = fork();

		switch (forkPID)
		{
			case(-1): 	// Failure
				errNum = errno;
				HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, fork failed);
				fprintf(stderr, "fork() failed with an errno of %d:\t%s\n", errNum, strerror(errNum));
				success = false;
				retVal = -1;
				break;
			case(0):	// Child
				// Redirect stdout to outputFile
				dupRetVal = dup2(outFileDesc, STDOUT_FILENO);
				
				if (dupRetVal == -1)
				{
					errNum = errno;
					HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, dup2 failed);
					fprintf(stderr, "dup2(outFD, stdout) failed with an errno of %d:\t%s\n", errNum, strerror(errNum));
					success = false;
				}
				
				// Redirect stderr to errorsFile
				dupRetVal = dup2(errFileDesc, STDERR_FILENO);
				
				if (dupRetVal == -1)
				{
					errNum = errno;
					HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, dup2 failed);
					fprintf(stderr, "dup2(errFD, stderr) failed with an errno of %d:\t%s\n", errNum, strerror(errNum));
					success = false;
				}
				
				// Execvp()
				if (success == true)
				{
					if (-1 == execvp(binToWrap->fullCmd[0], binToWrap->fullCmd))
					{
						errNum = errno;
						HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, execvp failed);
						fprintf(stderr, "execvp(argv[1], argv + 1) failed with an errno of %d:\t%s\n", errNum, strerror(errNum));
						success = false;
					}
				}
				break;
			default:	// Parent
				fprintf(stdout, "Waiting for %s to terminate...", binToWrap->binName);  // DEBUGGING
				while (wait(&wStatus) != -1)
				{
					fprintf(stdout, ".");
				}
				fprintf(stdout, "Complete.\n");
				// Investigate child process' status
				if (WIFEXITED(wStatus))
				{
					retVal = WEXITSTATUS(wStatus);
					fprintf(stdout, "The child process running %s exited with status %d.\n", binToWrap->binName, retVal);  // DEBUGGING
				}
				else
				{
					retVal = -1;
					fprintf(stderr, "The child process running %s did NOT terminate normally!\n", binToWrap->binName);  // DEBUGGING
					success = false;
				}
				break;
		}
	}
	
	// CLEAN UP
	// If we made it here, one of the following cases is true:
	//	A. We're the parent and the child terminated, normally or otherwise.
	//	B. We're the child and execvp() failed
	//	C. We have no children because fork() failed
	// 1. outputFile file descriptor
	if (outFileDesc > -1)
	{
		if (-1 == close(outFileDesc))
		{
			errNum = errno;
			HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, close failed);
			fprintf(stderr, "close(outFileDesc) failed with an errno of %d:\t%s\n", errNum, strerror(errNum));
		}
	}
	// 2. errorsFile file descriptor
	if (errFileDesc > -1)
	{
		if (-1 == close(errFileDesc))
		{
			errNum = errno;
			HARKLE_ERROR(Fileroad_Descriptors, wrap_bin, close failed);
			fprintf(stderr, "close(errFileDesc) failed with an errno of %d:\t%s\n", errNum, strerror(errNum));
		}
	}
	
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// rBinDat FUNCTIONS STOP ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
////////////////////////// fdDetails FUNCTIONS START /////////////////////////
//////////////////////////////////////////////////////////////////////////////


fdDetails_ptr create_fdDetails_ptr(void)
{
    // LOCAL VARIABLES
    fdDetails_ptr retVal = NULL;
	int numTries = 0;

	// ALLOCATE MEMORY
	while(numTries < FD_MAX_TRIES && retVal == NULL)
	{
		retVal = (fdDetails_ptr)calloc(1, sizeof(fdDetails));
		numTries++;
    }
    
    // DONE
    return retVal;
}


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
		flags != (O_WRONLY | O_CREAT) &&
		flags != (O_RDWR | O_CREAT) &&
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
				retVal->filename_ptr = (char*)calloc(strlen(fname) + 1, sizeof(char));

				if (retVal->filename_ptr != NULL)
				{
					temp_ptr = (char*)strncpy(retVal->filename_ptr, fname, len);

					if (temp_ptr != retVal->filename_ptr)
					{
						success = false;
						fprintf(stderr, "<<<ERROR>>> - open_fd() - strncpy() failed!\n");
					}
					else
					{
						// 5. Store the file length
						retVal->fileSize = get_file_len(retVal->fileDesc);

						if (retVal->fileSize == -1)
						{
							success = false;
							fprintf(stderr, "<<<ERROR>>> - open_fd() - get_file_len() failed!\n");		
						}

						// 6. Store the file descriptor flags
						retVal->fileDescFlags = fcntl(retVal->fileDesc, F_GETFD);

						// 7. Store the file status flags
						retVal->fileStatFlags = fcntl(retVal->fileDesc, F_GETFL);
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
				close(retVal->fileDesc);
			}
		}
		
		// 2. Free the struct
		free_fdDetails(&retVal);
	}

    return retVal;
}


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
                    temp_ptr = memset(tempStruct_ptr->filename_ptr, 0x0, len);
                    
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
            
		    // int fileDescFlags;		// File descriptor flags
            tempStruct_ptr->fileDescFlags = 0;

		    // int fileStatFlags;		// File status flags
		    tempStruct_ptr->fileStatFlags = 0;
            
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


bool validate_fdDetails(fdDetails_ptr checkThis_ptr)
{
    // LOCAL VARIABLES
    bool retVal = true;
    
    // DONE
    return retVal;
}


int update_fdDetails(fdDetails_ptr updateThis_ptr)
{
    // LOCAL VARIABLES
    int retVal = 0;
    
    // DONE
    return retVal;
}


bool set_oper_mode_flag(fdDetails_ptr updateThis_ptr, int setThisFlag)
{
	// LOCAL VARIABLES
	bool retVal = true;
	int curFlags = 0;
	
	// INPUT VALIDATION
	if (updateThis_ptr == NULL)
	{
		retVal = false;
		fprintf(stderr, "<<<ERROR>>> - set_oper_mode_flag() - updateThis_ptr is NULL!\n");
	}
	// NOTE TO THE FUTURE: Verify setThisFlag doesn't contain non-I/O Operating Mode flags
	else if (0)
	{
		//////////////////////////////// IMPLEMENT LATER ////////////////////////////////
		retVal = false;
		fprintf(stderr, "<<<ERROR>>> - set_oper_mode_flag() - Detected non-operating mode flag(s)!\n");
	}
	else
	{
		// GET CURRENT FLAGS
		curFlags = fcntl(updateThis_ptr->fileDesc, F_GETFL);
		fprintf(stdout, "set_oper_mode_flag() - fcntl() returned %d.\n", curFlags);  // DEBUGGING
		
		if (curFlags < 0)
		{
			retVal = false;
			fprintf(stderr, "<<<ERROR>>> - set_oper_mode_flag() - Unable to determine current flag(s)!\n");
		}
		else
		{	
			// ADD NEW FLAG
			curFlags |= setThisFlag;
			
			// CALL write_oper_mode_flags()
			retVal = write_oper_mode_flags(updateThis_ptr, curFlags);
		}
	}
	
	// DONE
	return retVal;
}


bool clear_oper_mode_flag(fdDetails_ptr updateThis_ptr, int clrThisFlag)
{
	// LOCAL VARIABLES
	bool retVal = true;
	int curFlags = 0;
	
	// INPUT VALIDATION
	if (updateThis_ptr == NULL)
	{
		retVal = false;
		fprintf(stderr, "<<<ERROR>>> - set_oper_mode_flag() - updateThis_ptr is NULL!\n");
	}
	// NOTE TO THE FUTURE: Verify setThisFlag doesn't contain non-I/O Operating Mode flags
	else if (0)
	{
		//////////////////////////////// IMPLEMENT LATER ////////////////////////////////
		retVal = false;
		fprintf(stderr, "<<<ERROR>>> - set_oper_mode_flag() - Detected non-operating mode flag(s)!\n");
	}
	else
	{
		// GET CURRENT FLAGS
		curFlags = fcntl(updateThis_ptr->fileDesc, F_GETFL);
		fprintf(stdout, "clear_oper_mode_flag() - fcntl() returned %d.\n", curFlags);  // DEBUGGING
		
		if (curFlags < 0)
		{
			retVal = false;
			fprintf(stderr, "<<<ERROR>>> - set_oper_mode_flag() - Unable to determine current flag(s)!\n");
		}
		else
		{	
			// CLEAR THE FLAG
			curFlags &= ~clrThisFlag;
			
			// CALL write_oper_mode_flags()
			retVal = write_oper_mode_flags(updateThis_ptr, curFlags);
		}
	}
	
	// DONE
	return retVal;
}


bool write_oper_mode_flags(fdDetails_ptr updateThis_ptr, int setTheseFlags)
{
	// LOCAL VARIABLES
	bool retVal = true;
	int fcntlRetVal = 0;  // Holds the return value from fcntl()

	// INPUT VALIDATION
	if (updateThis_ptr == NULL)
	{
		retVal = false;
		fprintf(stderr, "<<<ERROR>>> - set_oper_mode_flag() - updateThis_ptr is NULL!\n");
	}
	else
	{
		fcntlRetVal = fcntl(updateThis_ptr->fileDesc, F_SETFL, setTheseFlags);
		fprintf(stdout, "write_oper_mode_flags() - fcntl() returned %d.\n", fcntlRetVal);  // DEBUGGING
	}
	
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
////////////////////////// fdDetails FUNCTIONS STOP //////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// GENERAL FUNCTIONS START //////////////////////////
//////////////////////////////////////////////////////////////////////////////


off_t get_file_len(int fileDesc)
{
	// LOCAL VARIABLES
	off_t retVal = 0;
	off_t curVal = 0;	// We need to preserve the current offset

	// INPUT VALIDATION
	if (fileDesc >= 0)
	{
		// GET LENGTH
		// Get current offset
		curVal = lseek(fileDesc, 0, SEEK_CUR);

		// Seek to the end
	    retVal = lseek(fileDesc, 0, SEEK_END);
	     
	    if(retVal == -1)
	    {
	    	fprintf(stderr, "<<<ERROR>>> - get_file_len() - lseek() failed!\n");
	    }

	    // Preserve offset
	    if (curVal != lseek(fileDesc, curVal, SEEK_SET))
	    {
	    	fprintf(stderr, "<<<ERROR>>> - get_file_len() - Failed to recover the existing offset!\n");
	    }
	}
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// GENERAL FUNCTIONS STOP ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

/*
// For later
int fd_is_valid(int fd)
{
    return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}
 */
