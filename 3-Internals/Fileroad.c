#include <dirent.h>		// d_type MACROs
#include <fcntl.h>	 	// open() flags
#include "Fileroad.h"
#include <limits.h>		// UCHAR_MAX
#include "Memoroad.h"
#include <stdbool.h>	// bool, true, false
#include <stdio.h>		// fscanf, getchar
#include <stdlib.h>	 	// calloc
#include <string.h>	 	// strlen, strstr
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>		// read, stream macros

#ifndef FROAD_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define FROAD_MAX_TRIES 3
#endif  // FROAD_MAX_TRIES

#ifndef FROAD_BUFF_SIZE
// MACRO to size static arrays
#define FROAD_BUFF_SIZE 1024
#endif  // FROAD_BUFF_SIZE

#ifndef FROAD_SML_BUFF_SIZE
// MACRO to size small static arrays
#define FROAD_SML_BUFF_SIZE 32
#endif  // FROAD_SML_BUFF_SIZE

#ifndef HARKLE_ERROR
#define HARKLE_ERROR(header, funcName, msg) do { fprintf(stderr, "<<<ERROR>>> - %s - %s() - %s!\n", #header, #funcName, #msg); } while (0);
#endif  // HARKLE_ERROR

//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION PROTOTYPES START ///////////////////////
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION PROTOTYPES STOP ////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// INPUT FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


void clear_stdin(void)
{
	char read = 0;
	while (EOF != read && '\n' != read)
	{
		read = getchar();
		// putchar(read);  // DEBUGGING
	}

	return;
}


int read_a_num(void)
{
	// LOCAL VARIABLES
	int retVal = 0;

	// READ
	fscanf(stdin, "%d", &retVal);

	// CLEAR STDIN
	clear_stdin();

	// DONE
	return retVal;
}


char* buff_a_num(void)
{
	// LOCAL VARIABLES
	char* retVal;
	char localBuff[FROAD_SML_BUFF_SIZE + 1] = { 0 };
	bool success = true;  // If anything fails, this becomes false
	ssize_t numBytesRead = 0;  // Return value from read()
	int i = 0;  // Iterating variable used to check each char

	// READ
	numBytesRead = read(STDIN_FILENO, localBuff, FROAD_SML_BUFF_SIZE);
	// fprintf(stdout, "numBytesRead == %zu\n", numBytesRead);  // DEBUGGING
	// fprintf(stdout, "localBuff == %s\n", localBuff);  // DEBUGGING

	if (numBytesRead > FROAD_SML_BUFF_SIZE)
	{
		// This should never happen
		// fprintf(stderr, "<<<ERROR>>> - Fileroad - buff_a_num() - How did read get too many bytes?!\n");
		HARKLE_ERROR(Fileroad, buff_a_num, How did read get too many bytes);
		success = false;
	}
	else if (numBytesRead <= 0)
	{
		// This should never happen
		// fprintf(stderr, "<<<ERROR>>> - Fileroad - buff_a_num() - 0 bytes read?!\n");
		HARKLE_ERROR(Fileroad, buff_a_num, 0 bytes read?);
		success = false;
	}
	else  // "Just write." -Goldilocks(?)
	{
		// REMOVE ANY NEWLINES
		// puts("Entering buff_a_num() removing newlines");  // DEBUGGING
		i = 0;  // Reset temp variable
		while (i < FROAD_SML_BUFF_SIZE && (*(localBuff + i)) != '\0' && success == true)
		{
			if ((*(localBuff + i)) == '\n')
			{
				(*(localBuff + i)) = '\0';
			}
			else if ((*(localBuff + i)) == '\r')
			{
				(*(localBuff + i)) = '\0';
			}
			else
			{
				i++;
			}
		}

		// puts("Entering buff_a_num() read");  // DEBUGGING
		// VALIDATE READ
		// Look for non-numbers
		i = 0;  // Reset temp variable
		while (i < FROAD_SML_BUFF_SIZE && (*(localBuff + i)) != '\0' && success == true)
		{
			// fprintf(stdout, "Looking at:\t%c (0x%X)\n", (*(localBuff + i)), (*(localBuff + i)));  // DEBUGGING
			if ((*(localBuff + i)) < 48 || (*(localBuff + i)) > 57)
			{
				success = false;
			}
			else
			{
				i++;
			}
		}

		// Copy it onto the heap
		if (success == true)
		{
			// puts("Entering buff_a_num() heap allocation");  // DEBUGGING
			// fprintf(stdout, "localBuff == %s\n", localBuff);  // DEBUGGING
			retVal = copy_a_string(localBuff);
			// fprintf(stdout, "retVal == %s\n", retVal);  // DEBUGGING

			if (!retVal)
			{
				// fprintf(stderr, "<<<ERROR>>> - Fileroad - buff_a_num() - copy_a_string failed!\n");
				HARKLE_ERROR(Fileroad, buff_a_num, copy_a_string failed);
				success = false;
			}
		}
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// INPUT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FILE FUNCTIONS START ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


char* read_a_file(char* fileName)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	int fileDesc = 0;  // Holds the file descriptor returned by open()
	bool success = true;  // If anything fails, set this to false
	char buff[FROAD_BUFF_SIZE + 1] = { 0 };  // Static array to read file contents
	ssize_t numBytesRead = 0;  // Return value from read()
	int numTries = 0;  // Keeps track of allocation attempts
	char* temp_ptr = NULL;  // Return values from string.h function calls

	// INPUT VALIDATION
	if (fileName)
	{
		if (*fileName)
		{
			// READ THE FILE
			// 1. Open the file
			if (success == true)
			{
				fileDesc = open(fileName, O_RDONLY);

				if (fileDesc < 0)
				{
					// fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - open failed!\n");
					HARKLE_ERROR(Fileroad, read_a_file, open failed);
					success = false;
				}
			}

			// 2. Read the file
			if (success == true)
			{
				numBytesRead = read(fileDesc, buff, FROAD_BUFF_SIZE);

				if (numBytesRead < 0)
				{
					// fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - read failed!\n");
					HARKLE_ERROR(Fileroad, read_a_file, read failed);
					success = false;
				}
				else if (numBytesRead == 0)
				{
					// It's ok if 0 bytes were read.  Some cmdline files are empty.
					// fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - 0 bytes read!\n");
					// success = false;
					temp_ptr = strcpy(buff, "<EMPTY>");

					if (temp_ptr != buff)
					{
						// fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - strcpy failed!\n");
						HARKLE_ERROR(Fileroad, read_a_file, strcpy failed);
						success = false;
					}
					else
					{
						numBytesRead = strlen(buff);
					}
				}
			}

			// 3. Allocate an array to store the file contents
			if (success == true && numBytesRead > 0)
			{
				while (!retVal && numTries < FROAD_MAX_TRIES)
				{
					retVal = (char*)calloc(numBytesRead + 1, sizeof(char));
					numTries++;
				}

				if (!retVal)
				{
					// fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - calloc failed!\n");
					HARKLE_ERROR(Fileroad, read_a_file, calloc failed);
					success = false;
				}
			}

			// 4. Read the file contents into the heap-allocated array
			if (success == true && retVal)
			{
				temp_ptr = strncpy(retVal, buff, numBytesRead);

				if (temp_ptr != retVal)
				{
					// fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - strncpy failed!\n");
					HARKLE_ERROR(Fileroad, read_a_file, strncpy failed);
					success = false;
				}
			}
		}
	}
	else
	{
		// fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - NULL fileName!\n");
		HARKLE_ERROR(Fileroad, read_a_file, NULL fileName);
		success = false;
	}

	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			// memset
			if (*retVal)
			{
				temp_ptr = memset(retVal, 0x0, strlen(retVal));

				if (temp_ptr != retVal)
				{
					// fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - memset failed!\n");
					HARKLE_ERROR(Fileroad, read_a_file, memset failed);
				}

				temp_ptr = NULL;
			}

			// free
			free(retVal);

			// NULL
			retVal = NULL;
		}
	}

	// Close the file descriptor regardless of the success status
	if (fileDesc > 2)
	{
		if (close(fileDesc) < 0)
		{
			// fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - close failed!\n");
			HARKLE_ERROR(Fileroad, read_a_file, close failed);
		}
	}

	// DONE
	return retVal;
}


off_t size_a_file(char* fileName)
{
	// LOCAL VARIABLES
	off_t retVal = 0;  // This will be converted from data type off_t
	bool success = true;  // If anything fails, set this to false
	struct stat fileStat;  // OUT parameter for stat()
	int stRetVal = 0;  // Return value from stat()
	
	// INPUT VALIDATION
	if (!fileName)
	{
		HARKLE_ERROR(Fileroad, size_a_file, NULL pointer);
		success = false;
	}
	else if (!(*fileName))
	{
		HARKLE_ERROR(Fileroad, size_a_file, Empty string);
		success = false;
	}
	
	// SIZE IT
	if (success == true)
	{
		stRetVal = stat(fileName, &fileStat);
		
		if (stRetVal == -1)
		{
			HARKLE_ERROR(Fileroad, size_a_file, stat failed);
			success = false;
		}
		else
		{
			// printf("File size:\t%lld bytes\n", (long long)fileStat.st_size);  // DEBUGGING
			retVal = fileStat.st_size;
		}
	}
	
	// CLEAN UP
	if (success == false)
	{
		retVal = -1;	
	}
	
	// DONE
	return retVal;
}


/*
	Purpose - Utilize stat to determine a file's type
	Input
		fileName - nul-terminated char array of the file to read
	Output
		On success, file's type
		On failure, UCHAR_MAX
 */
unsigned char get_a_file_type(char* fileName)
{
	// LOCAL VARIABLES
	unsigned char retVal = 0;
	bool success = true;  // If anything fails, set this to false
	struct stat fileStat;  // OUT parameter for stat()
	int stRetVal = 0;  // Return value from stat()
	mode_t modeVal = 0;  // Will hold the flag bitwise of the struct stat.st_mode
	
	// INPUT VALIDATION
	if (!fileName)
	{
		HARKLE_ERROR(Fileroad, get_a_file_type, NULL pointer);
		success = false;
	}
	else if (!(*fileName))
	{
		HARKLE_ERROR(Fileroad, get_a_file_type, Empty string);
		success = false;
	}
	
	// SIZE IT
	if (success == true)
	{
		stRetVal = stat(fileName, &fileStat);
		
		if (stRetVal == -1)
		{
			HARKLE_ERROR(Fileroad, get_a_file_type, stat failed);
			success = false;
		}
		else
		{
			modeVal = fileStat.st_mode;

			if (S_ISREG(modeVal))			// is it a regular file?
			{
				retVal = DT_REG;
			}
			else if (S_ISDIR(modeVal))		// directory?
			{
				retVal = DT_DIR;
			}
			else if (S_ISCHR(modeVal))		// character device?
			{
				retVal = DT_CHR;
			}
			else if (S_ISBLK(modeVal))		// block device?
			{
				retVal = DT_BLK;
			}
			else if (S_ISFIFO(modeVal))		// FIFO (named pipe)?
			{
				retVal = DT_FIFO;
			}
			else if (S_ISLNK(modeVal))		// symbolic link?  (Not in POSIX.1-1996.)
			{
				retVal = DT_LNK;
			}
			else if (S_ISSOCK(modeVal))		// socket?  (Not in POSIX.1-1996.)
			{
				retVal = DT_SOCK;
			}
			else
			{
				success = false;
			}
		}
	}
	
	// CLEAN UP
	if (success == false)
	{
		retVal = UCHAR_MAX;	
	}
	
	// DONE
	fprintf(stdout, "The type of %s is %u.\n", fileName, retVal);
	return retVal;
}


/*
	Purpose - Replicate os.path.join (in a very hacky way)
	Input
		path_ptr - nul-terminated string presumably representing a path
		join_ptr - nul-terminate string presumably representing something to add
			to path_ptr
		isFile - true if join_ptr is a file, false if join_ptr is a directory
	Output
		On success, a heap-allocated, nul-terminated string containing
			(essentially) a strcat of path_ptr and join_ptr
		On failure, NULL
	Notes:
		It is the caller's responsibility to free the return value
	Examples:
		path_ptr == "/proc", join_ptr == "31337", isFile == false, returns "/proc/31337/"
		path_ptr == "/proc", join_ptr == "31337", isFile == true, returns "/proc/31337"
 */
char* os_path_join(char* path_ptr, char* join_ptr, bool isFile)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	char* srce_ptr = NULL;  // Temp source
	char* dest_ptr = NULL;  // Temp destination
	bool success = true;  // Make this false if anything fails
	size_t pathLen = 0;  // Length of the path
	size_t joinLen = 0;  // Length of the joining
	size_t newLen = 0;  // Holds the calculation for the length of the new array

	// INPUT VALIDATION
	if (!path_ptr)
	{
		HARKLE_ERROR(Fileroad, os_path_join, path_ptr is NULL);
		success = false;
	}
	else if (!join_ptr)
	{
		HARKLE_ERROR(Fileroad, os_path_join, join_ptr is NULL);
		success = false;
	}
	else if (!(*path_ptr))
	{
		HARKLE_ERROR(Fileroad, os_path_join, path_ptr is empty);
		success = false;
	}
	else if (!(*join_ptr))
	{
		HARKLE_ERROR(Fileroad, os_path_join, join_ptr is empty);
		success = false;
	}

	// JOIN
	// 1. Calculate new length
	// 1.1. Add in pathLen
	pathLen = strlen(path_ptr);
	newLen += pathLen;

	if ((*(path_ptr + pathLen - 1)) != '/')
	{
		newLen++;
	}

	// 1.2. Prepare joinLen by removing preceding slashes
	while (*join_ptr == '/')
	{
		join_ptr++;
	}	

	// 1.3. Add in joinLen
	joinLen = strlen(join_ptr);
	newLen += joinLen;

	if ((*(join_ptr + joinLen - 1)) == '/')
	{
		if (isFile == true)
		{
			(*(join_ptr + joinLen - 1)) = '\0';
			joinLen--;
			newLen--;
		}
	}
	else if (isFile == false)
	{
		newLen++;
	}

	// 2. Allocate the array
	retVal = get_me_a_buffer(newLen);

	if (!retVal)
	{
		HARKLE_ERROR(Fileroad, os_path_join, get_me_a_buffer failed);
		success = false;
	}

	// 3. Copy in path & join
	// 3.1. Path
	srce_ptr = path_ptr;
	dest_ptr = retVal;
	while (*srce_ptr != '\0')
	{
		*dest_ptr = *srce_ptr;
		dest_ptr++;
		srce_ptr++;
	}
	if (*(dest_ptr - 1) != '/')
	{
		*(dest_ptr - 1) = '/';
		// dest_ptr++;
	}

	// 3.2. Join
	srce_ptr = join_ptr;
	if (*srce_ptr == '/')
	{
		srce_ptr++;
	}
	while (*srce_ptr != '\0')
	{
		*dest_ptr = *srce_ptr;
		dest_ptr++;
		srce_ptr++;
	}

	// 3.3. Verify trailing slash (if applicable)
	if (*(dest_ptr - 1) != '/' && isFile == false)
	{
		*(dest_ptr - 1) = '/';
	}

	// 3.4. Verify nul termination
	// dest_ptr++;
	*dest_ptr = '\0';

	// VERIFY
	if (strlen(retVal) != newLen)
	{
		HARKLE_ERROR(Fileroad, os_path_join, length mismatch);
		fprintf(stdout, "Path:\t%s\n", path_ptr);  // DEBUGGING
		fprintf(stdout, "Join:\t%s\n", join_ptr);  // DEBUGGING
		success = false;
	}
	fprintf(stdout, "New: \t%s\n", retVal);  // DEBUGGING

	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == release_a_string(&retVal))
			{
				HARKLE_ERROR(Fileroad, os_path_join, release_a_string failed);
			}
		}
	}

	// DONE
	return retVal;
}





//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FILE FUNCTIONS STOP /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION DEFINITIONS START //////////////////////
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION DEFINITIONS STOP ///////////////////////
//////////////////////////////////////////////////////////////////////////////
