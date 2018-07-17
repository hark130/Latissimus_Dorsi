#include <dirent.h>		// d_type MACROs
#include <errno.h>		// errno
#include <fcntl.h>	 	// open() flags
#include "Fileroad.h"
#include "Harklerror.h"	// HARKLE_ERROR
#include <inttypes.h>	// intmax_t
#include <libgen.h>		// basename, dirname
#include <limits.h>		// UCHAR_MAX, PATH_MAX
#include "Memoroad.h"
#include <stdbool.h>	// bool, true, false
#include <stdio.h>		// fscanf, getchar
#include <stdlib.h>	 	// calloc
#include <string.h>	 	// strlen, strstr, strerror
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

	if (numBytesRead > FROAD_SML_BUFF_SIZE)
	{
		// This should never happen
		HARKLE_ERROR(Fileroad, buff_a_num, How did read get too many bytes);
		success = false;
	}
	else if (numBytesRead <= 0)
	{
		// This should never happen
		HARKLE_ERROR(Fileroad, buff_a_num, 0 bytes read?);
		success = false;
	}
	else  // "Just write." -Goldilocks(?)
	{
		// REMOVE ANY NEWLINES
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

		// VALIDATE READ
		// Look for non-numbers
		i = 0;  // Reset temp variable
		while (i < FROAD_SML_BUFF_SIZE && (*(localBuff + i)) != '\0' && success == true)
		{
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
			retVal = copy_a_string(localBuff);

			if (!retVal)
			{
				HARKLE_ERROR(Fileroad, buff_a_num, copy_a_string failed);
				success = false;
			}
		}
	}

	// DONE
	return retVal;
}


char** split_lines(char* haystack, char splitChar)
{
	// LOCAL VARIABLES
	char** retVal = NULL;
	char** tempArr_ptr = NULL;  // A copy of retVal for the purposes of iterating
	char* temp_ptr = NULL;  // Return value from string.h function calls
	bool success = true;
	int charCount = 0;  // Number of splitChars in haystack (concurrent splitChars count as one)
	char* hsCopy = NULL;  // A copy of the haystack to aid in parsing
	size_t hsLen = 0;  // Length of the original haystack
	char* currStr = NULL;  // Memory address of the current string truncated from hsCopy
	char* currNul = NULL;  // Memory address of the current artificial nul terminator in hsCopy

	// INPUT VALIDATION
	if (!haystack)
	{
		HARKLE_ERROR(Fileroad, split_lines, NULL haystack pointer);
		success = false;
	}
	else if (*haystack == '\0')
	{
		HARKLE_ERROR(Fileroad, split_lines, Empty haystack);
		success = false;
	}
	else if (splitChar == '\0')
	{
		HARKLE_ERROR(Fileroad, split_lines, Invalid split character);
		success = false;
	}
	else
	{
		temp_ptr = strchr(haystack, splitChar);
		
		if (!temp_ptr)
		{
			HARKLE_ERROR(Fileroad, split_lines, Split character not found);
			success = false;
		}
	}
	
	// Size haystack
	if (success == true)
	{
		hsLen = strlen(haystack);
			
		if (hsLen < 1)
		{
			HARKLE_ERROR(Fileroad, split_lines, strlen failed);
			success = false;
		}
	}
	
	// Copy haystack
	if (success == true)
	{
		hsCopy = copy_a_string(haystack);
		
		if (!hsCopy)
		{
			HARKLE_ERROR(Fileroad, split_lines, copy_a_string failed);
			success = false;
		}
		else if (strlen(hsCopy) != hsLen)
		{
			HARKLE_ERROR(Fileroad, split_lines, String length mismatch between haystack and hsCopy);
			success = false;
		}
	}
	
	// PARSE haystack
	if (success == true)
	{
		// 1. Count occurrences of the splitChar
		// NOTE: Count only the final concurrent splitChar occurrence as long as it does
		//	not begin or end the string.
		temp_ptr = haystack;

		while(*temp_ptr != '\0')
		{
			if (*temp_ptr == splitChar)
			{
				if (temp_ptr > haystack)
				{
					if ((*(temp_ptr - 1)) != splitChar)
					{
						// Don't count splitChar if it's last because that would yield an empty string
						if ((*(temp_ptr + 1)) != splitChar && (*(temp_ptr + 1)) != '\0')
						{
							charCount++;
						}
					}
				}
				else
				{
					// Don't count splitChar if it's first because that would yield an empty string
				}
			}
		}
		
		if (charCount == 0)
		{
			success = false;	
		}
	}
	
	// 2. Allocate the array of char*s into retVal
	if (success == true)
	{
		// charCount + 1 because 3 splitChars make 4 strings
		// SSSScSSSScSSSScSSSS if c == char && S == string
		retVal = get_me_a_buffer_array(charCount + 1, true);
		
		if (!retVal)
		{
			HARKLE_ERROR(Fileroad, split_lines, get_me_a_buffer_array failed);
			success = false;
		}
	}
	
	// 3. Parse each substring from the haystack
	if (success == true)
	{
		if (!hsCopy)
		{
			success = false;	
		}
		else
		{
			// Copy retVal
			tempArr_ptr = retVal;
			// Start parsing hsCopy
			currStr = hsCopy;
			
			while (*currStr && success == true)
			{
				// Find the first splitChar
				temp_ptr = strchr(currStr, splitChar);
				
				if (!temp_ptr)
				{
					// This is the last string to parse
					if (*currStr)
					{
						(*tempArr_ptr) = copy_a_string(currStr);
						
						if (!(*tempArr_ptr))
						{
							HARKLE_ERROR(Fileroad, split_lines, copy_a_string failed);
							success = false;
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					currNul = temp_ptr;  // Save the address
					*currNul = '\0';  // Truncate currStr
					temp_ptr = currStr;  // Reset temp_ptr to start of string
					(*tempArr_ptr) = copy_a_string(temp_ptr);
					
					if (!(*tempArr_ptr))
					{
						HARKLE_ERROR(Fileroad, split_lines, copy_a_string failed);
						success = false;
					}
					else
					{
						tempArr_ptr++;  // Next char* index in retVal
						currStr = currNul + 1;  // Start of the next string
						temp_ptr = NULL;  // Empty temp_ptr
					}
				}
			}
		}
	}	

	// CLEAN UP
	// hsCopy
	if (hsCopy)
	{
		if (false == release_a_string_len(&hsCopy, hsLen))
		{
			HARKLE_ERROR(Fileroad, split_lines, release_a_string_len failed);
		}
	}
	if (success == false)
	{
		if (retVal)
		{
			if (false == free_char_arr(&retVal))
			{
				HARKLE_ERROR(Fileroad, split_lines, free_char_arr failed);
			}
		}
	}
	
	// DONE
	return retVal;
}


int search_char_arr(char** haystack_arr, char* needle_ptr)
{
	// LOCAL VARIABLES
	int retVal = -1;  // Default status
	int i = 0;  // Used for pointer arithmetic
	char** temp_arr = NULL;  // Temp variable to iterate through haystack_arr

	// INPUT VALIDATION
	if (!haystack_arr)
	{
		HARKLE_ERROR(Fileroad, search_char_arr, NULL search_char_arr pointer);
	}
	else if (!(*haystack_arr))
	{
		// I changed my mind.  I decided that an emtpy array was valid input.
		//	Obviously, needle_ptr isn't in there anywhere.  It saves the
		// 	caller from having to validate the state of their own array
		//	before calling this function.  You're welcome.
		// HARKLE_ERROR(Fileroad, search_char_arr, Empty array);
	}
	else if (!needle_ptr)
	{
		HARKLE_ERROR(Fileroad, search_char_arr, NULL needle_ptr pointer);
	}
	else if (!(*needle_ptr))
	{
		HARKLE_ERROR(Fileroad, search_char_arr, Empty string);
	}

	// GO LOOKING
	while ((*(haystack_arr + i)) != NULL)
	{
		if (0 == strcmp((*(haystack_arr + i)), needle_ptr))
		{
			retVal = i;
			break;  // FOUND IT!
		}
		else
		{
			// Next haystack string
			i++;
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


char* fread_a_file(char* fileName)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	bool success = true;
	FILE* theFile = NULL;
	size_t fileSize = 0;  // Holds the return value from size_a_file_ptr()
	size_t bytesRead = 0;  // Bytes read by fread()

	// INPUT VALIDATION
	if (!fileName)
	{
		HARKLE_ERROR(Fileroad, fread_a_file, NULL pointer);
		success = false;
	}
	else if (!(*fileName))
	{
		HARKLE_ERROR(Fileroad, fread_a_file, Empty filename);
		success = false;
	}

	// READ IT
	// 1. Open it
	if (success == true)
	{
		theFile = fopen(fileName, "r");

		if (!theFile)
		{
			HARKLE_ERROR(Fileroad, fread_a_file, fopen failed);
			success = false;
		}
	}

	// 2. Size it
	if (success == true)
	{
		fileSize = size_a_file_ptr(theFile);

		if (fileSize < 0)
		{
			HARKLE_ERROR(Fileroad, fread_a_file, size_a_file_ptr failed);
			success = false;
		}
	}

	// 4. Allocate a buffer
	if (success == true)
	{
		retVal = get_me_a_buffer(fileSize);

		if (!retVal)
		{
			HARKLE_ERROR(Fileroad, fread_a_file, get_me_a_buffer failed);
			success = false;
		}
	}

	// 5. fread it in
	if (success == true)
	{
		// size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
		bytesRead = fread(retVal, sizeof(char), fileSize, theFile);

		if (bytesRead != fileSize)
		{
			HARKLE_ERROR(Fileroad, fread_a_file, mismatch between size_a_file_ptr and fread);
		}
	}

	// CLEAN UP
	// 1. theFile
	if (theFile)
	{
		if (EOF == fclose(theFile))
		{
			HARKLE_ERROR(Fileroad, fread_a_file, fclose failed);
		}
	}

	if (success == false)
	{
		if (retVal)
		{
			if (false == release_a_string(&retVal))
			{
				HARKLE_ERROR(Fileroad, fread_a_file, release_a_string failed);
			}
		}
	}

	// DONE
	return retVal;
}


char* read_a_file(char* fileName)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	int fileDesc = 0;  // Holds the file descriptor returned by open()
	bool success = true;  // If anything fails, set this to false
	ssize_t numBytesRead = 0;  // Return value from read()
	ssize_t prevNumBytesRead = 0;  // Return value from read()
	int numTries = 0;  // Keeps track of allocation attempts
	char* temp_ptr = NULL;  // Return values from string.h function calls
	off_t fileSize = 0;  // File size read from file descriptor
	int errNum = 0;  // [OUT] parameter for size_a_file_desc()
	bool guessingSize = false;  // Set this to true if file size comes back 0

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
					HARKLE_ERROR(Fileroad, read_a_file, open failed);
					success = false;
				}
			}

			// 2. Size the file
			if (success == true)
			{
				fileSize = size_a_file_desc(fileDesc, &errNum);

				if (fileSize == -1)
				{
					HARKLE_ERROR(Fileroad, read_a_file, size_a_file_desc failed);
					fprintf(stderr, "size_a_file_desc() set errNum to %d\n", errNum);
					success = false;
				}
				else if (fileSize == 0)
				{
					fileSize = FROAD_BUFF_SIZE + 1;
					guessingSize = true;
				}
			}

			// 3. Allocate a buffer
			if (success == true)
			{
				retVal = get_me_a_buffer(fileSize);

				if (!retVal)
				{
					HARKLE_ERROR(Fileroad, read_a_file, get_me_a_buffer failed);
					success = false;
				}
			}

			// 4. Read the file
			if (success == true)
			{
				numBytesRead = read(fileDesc, retVal, fileSize);

				if (numBytesRead < 0)
				{
					HARKLE_ERROR(Fileroad, read_a_file, read failed);
					success = false;
				}
				else if (numBytesRead == 0)
				{
					// It's ok if 0 bytes were read.  Some cmdline files are empty.
					temp_ptr = strcpy(retVal, "<EMPTY>");

					if (temp_ptr != retVal)
					{
						HARKLE_ERROR(Fileroad, read_a_file, strcpy failed);
						success = false;
					}
					else
					{
						numBytesRead = strlen(retVal);
					}
				}
				else if (numBytesRead == fileSize && guessingSize == true)
				{
					// There may be more to read
					do
					{
						// 0. Record previous number of bytes read
						prevNumBytesRead = numBytesRead;

						// 1. Increase the buffer size
						temp_ptr = realloc(retVal, fileSize + FROAD_BUFF_SIZE);

						if (!temp_ptr)
						{
							HARKLE_ERROR(Fileroad, read_a_file, realloc failed);
							success = false;
						}
						else
						{
							retVal = temp_ptr;
							temp_ptr = NULL;
							fileSize += FROAD_BUFF_SIZE;
						}

						// 2. Rewind the file descriptor
						if (false == rewind_a_file_desc(fileDesc, &errNum))
						{
							HARKLE_ERROR(Fileroad, read_a_file, rewind_a_file_desc failed);
							fprintf(stderr, "rewind_a_file_desc() set errNum to %d\n", errNum);
							success = false;
						}
						else
						{
							// 3. Read the file descriptor again
							numBytesRead = read(fileDesc, retVal, fileSize);

							if (numBytesRead <= 0)
							{
								HARKLE_ERROR(Fileroad, read_a_file, (re)read failed);
								success = false;
							}
							else
							{

							}
						}
					}
					while (numBytesRead == fileSize && guessingSize == true && success == true && prevNumBytesRead != numBytesRead);
				}
			}
		}
	}
	else
	{
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
	if (fileDesc >= 0)
	{
		if (close(fileDesc) < 0)
		{
			HARKLE_ERROR(Fileroad, read_a_file, close failed);
		}
	}

	// DONE
	return retVal;
}


off_t size_a_file(char* fileName, int* errNum)
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
	else if (!errNum)
	{
		HARKLE_ERROR(Fileroad, size_a_file, NULL pointer);
		success = false;
	}
	else
	{
		*errNum = 0;
	}
	
	// SIZE IT
	if (success == true)
	{
		// fprintf(stdout, "Sizing %s\n", fileName);  // DEBUGGING
		stRetVal = lstat(fileName, &fileStat);
		
		if (stRetVal == -1)
		{
			*errNum = errno;
			HARKLE_ERROR(Fileroad, size_a_file, stat failed);
			success = false;
		}
		else
		{
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


off_t size_a_file_desc(int fileDesc, int* errNum)
{
	// LOCAL VARIABLES
	off_t retVal = 0;  // This will be converted from data type off_t
	bool success = true;  // If anything fails, set this to false
	struct stat fileStat;  // OUT parameter for stat()
	int stRetVal = 0;  // Return value from stat()
	
	// INPUT VALIDATION
	if (fileDesc < 0)
	{
		HARKLE_ERROR(Fileroad, size_a_file_desc, Invalid file descriptor);
		success = false;
	}
	else if (!errNum)
	{
		HARKLE_ERROR(Fileroad, size_a_file_desc, NULL pointer);
		success = false;
	}
	else
	{
		*errNum = 0;
	}
	
	// SIZE IT
	if (success == true)
	{
		stRetVal = fstat(fileDesc, &fileStat);
		
		if (stRetVal == -1)
		{
			*errNum = errno;
			HARKLE_ERROR(Fileroad, size_a_file_desc, stat failed);
			success = false;
		}
		else
		{
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


size_t size_a_file_ptr(FILE* openFile)
{
	// LOCAL VARIABLES
	size_t retVal = 0;
	bool success = true;

	// INPUT VALIDATION
	if (!openFile)
	{
		HARKLE_ERROR(Fileroad, size_a_file_ptr, NULL pointer);
		success = false;
	}

	if (success == true)
	{
		// SIZE THE FILE
		// 1. Rewind the file
		rewind(openFile);

		// 2. Read the file byte by tedious byte
		while (fgetc(openFile) != EOF)
		{
			retVal++;
		}

		// 3. Rewind the file again
		rewind(openFile);
	}
	
	// CLEAN UP
	if (success == false)
	{
		retVal = -1;
	}

	// DONE
	return retVal;
}


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
	// fprintf(stdout, "The type of %s is %u.\n", fileName, retVal);  // DEBUGGING
	return retVal;
}


bool os_path_isfile(char* path_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	unsigned char fileType = 0;  // Holds return value from get_a_file_type()

	// INPUT VALIDATION
	if (!path_ptr)
	{
		retVal = false;
	}
	else if (!(*path_ptr))
	{
		retVal = false;
	}
	else
	{
		// FILE EXISTS?
		retVal = os_path_exists(path_ptr);

		if (retVal == true)
		{
			// FILE IS A REGULAR FILE?
			fileType = get_a_file_type(path_ptr);

			if (fileType != DT_REG)
			{
				retVal = false;		
			}
		}
	}

	// DONE
	return retVal;
}


bool os_path_exists(char* path_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	int fileDesc = -1;  // Holds return value from open()

	// INPUT VALIDATION
	if (!path_ptr)
	{
		retVal = false;
	}
	else if (!(*path_ptr))
	{
		retVal = false;
	}
	else
	{
		// FILE EXISTS?
		fileDesc = open(path_ptr, O_RDONLY);

		if (fileDesc < 0)
		{
			retVal = false;
			errno = 0;
		}
		else
		{
			close(fileDesc);
			fileDesc = -1;
		}		
	}

	// CLEAN UP
	if (fileDesc > -1)
	{
		close(fileDesc);
		fileDesc = -1;
	}

	// DONE
	return retVal;
}


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
	char newRetv[PATH_MAX + 1] = { "/" };
	char* temp_ptr = NULL;  // Temp pointer to iterate through arrays

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
	else if (strlen(path_ptr) + strlen(join_ptr) + 3 > PATH_MAX)
	{
		HARKLE_ERROR(Fileroad, os_path_join, Strings are too long);
		success = false;
	}

	// POPULATE ARRAYS
	if (success == true)
	{
		// newPath
		srce_ptr = path_ptr;
		dest_ptr = newRetv + 1;

		while (*srce_ptr != '\0')
		{
			if (srce_ptr == path_ptr && *srce_ptr == '/')
			{
				srce_ptr++;
			}
			else if (*srce_ptr == '/' && *(srce_ptr + 1) == '\0')
			{
				srce_ptr++;
			}
			else
			{
				*dest_ptr = *srce_ptr;
				dest_ptr++;
				srce_ptr++;
			}
		}

		// Check for root directory
		if (strlen(path_ptr) != 1 || *path_ptr != '/')
		{
			*dest_ptr = '/';
			dest_ptr++;
		}

		// newJoin
		srce_ptr = join_ptr;

		while(*srce_ptr != '\0')
		{
			if (srce_ptr == join_ptr && *srce_ptr == '/')
			{
				srce_ptr++;
			}
			else if (*srce_ptr == '/' && *(srce_ptr + 1) == '\0')
			{
				srce_ptr++;
			}
			else
			{
				*dest_ptr = *srce_ptr;
				dest_ptr++;
				srce_ptr++;
			}
		}

		// Trailing slash
		if (isFile == false)
		{
			*dest_ptr = '/';
			dest_ptr++;
		}

		// Prepare output
		retVal = copy_a_string(newRetv);

		if (!retVal)
		{
			HARKLE_ERROR(Fileroad, os_path_join, copy_a_string failed);
			success = false;
		}
	}

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


char* os_path_basename(char* path_ptr)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	char* temp_ptr = NULL;  // Return value from basename() call
	bool success = true;  // Make this false if anything fails
	int errNum = 0;  // Catch errno
	
	// INPUT VALIDATION
	if (!path_ptr)
	{
		HARKLE_ERROR(Fileroad, os_path_basename, NULL pointer);
		success = false;
	}
	else if (!(*path_ptr))
	{
		HARKLE_ERROR(Fileroad, os_path_basename, Empty string);
		success = false;	
	}
	
	// DETERMINE BASENAME
	if (success == true)
	{
		temp_ptr = basename(path_ptr);
		
		if (!temp_ptr)
		{
			errNum = errno;
			HARKLE_ERROR(Fileroad, os_path_basename, basename failed);
			fprintf(stderr, "basename(%s) set errno to %d:\t%s\n", path_ptr, errNum, strerror(errNum));
			success = false;
		}
		else if (!(*temp_ptr))
		{
			HARKLE_ERROR(Fileroad, os_path_basename, basename failed);
			fprintf(stderr, "basename(%s) returned an empty string!\n", path_ptr);
			success = false;
		}
	}
	
	// ALLOCATE BUFFER
	if (success == true)
	{
		retVal = copy_a_string(temp_ptr);
		
		if (!retVal)
		{
			HARKLE_ERROR(Fileroad, os_path_basename, copy_a_string failed);
			success = false;
		}
	}
	
	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == release_a_string(&retVal))
			{
				HARKLE_ERROR(Fileroad, os_path_basename, release_a_string failed);
			}
		}
	}
	
	// DONE
	return retVal;
}


char* os_path_dirname(char* path_ptr)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	char* temp_ptr = NULL;  // Return value from basename() call
	bool success = true;  // Make this false if anything fails
	int errNum = 0;  // Catch errno
	
	// INPUT VALIDATION
	if (!path_ptr)
	{
		HARKLE_ERROR(Fileroad, os_path_dirname, NULL pointer);
		success = false;
	}
	else if (!(*path_ptr))
	{
		HARKLE_ERROR(Fileroad, os_path_dirname, Empty string);
		success = false;	
	}
	
	// DETERMINE BASENAME
	if (success == true)
	{
		temp_ptr = dirname(path_ptr);
		
		if (!temp_ptr)
		{
			errNum = errno;
			HARKLE_ERROR(Fileroad, os_path_dirname, dirname failed);
			fprintf(stderr, "dirname(%s) set errno to %d:\t%s\n", path_ptr, errNum, strerror(errNum));
			success = false;
		}
		else if (!(*temp_ptr))
		{
			HARKLE_ERROR(Fileroad, os_path_dirname, dirname failed);
			fprintf(stderr, "dirname(%s) returned an empty string!\n", path_ptr);
			success = false;
		}
	}
	
	// ALLOCATE BUFFER
	if (success == true)
	{
		retVal = copy_a_string(temp_ptr);
		
		if (!retVal)
		{
			HARKLE_ERROR(Fileroad, os_path_dirname, copy_a_string failed);
			success = false;
		}
	}
	
	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == release_a_string(&retVal))
			{
				HARKLE_ERROR(Fileroad, os_path_dirname, release_a_string failed);
			}
		}
	}
	
	// DONE
	return retVal;
}


bool rewind_a_file_desc(int fileDesc, int* errNum)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Set this to false if anything fails
	
	// INPUT VALIDATION
	if (fileDesc < 0)
	{
		HARKLE_ERROR(Fileroad, size_a_file_desc, Invalid file descriptor);
		retVal = false;
	}
	else if (!errNum)
	{
		HARKLE_ERROR(Fileroad, size_a_file_desc, NULL pointer);
		retVal = false;
	}
	else
	{
		*errNum = 0;
	}

	// REWIND
	if (retVal == true)
	{
		// Seek to start of file
		if (-1 == lseek(fileDesc, 0x0, SEEK_SET))
		{
			*errNum = errno;
			retVal = false;
		}
	}

	// DONE
	return retVal;
}


char* clean_filename(char* dirtyFile, bool inPlace)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	char* backUp = NULL;  // Backup copy of dirtyFile in case we want to revert
	bool success = true;  // Make this false if anything fails
	// char currChar = 0;  // One character to switch on
	// char badChar = 0;  // Temp var to hold bad chars
	char* temp_ptr = NULL;  // Iterating pointer variable
	char badChars[33] = { 0 };  // Bad characters to replace
	size_t dirtStrLen = 0;  // strlen() of dirtyFile
	int i = 0;  // Iterating variable

	// INPUT VALIDATION
	if (!dirtyFile)
	{
		HARKLE_ERROR(Fileroad, clean_filename, NULL pointer);
		success = false;
	}
	else if (!(*dirtyFile))
	{
		HARKLE_ERROR(Fileroad, clean_filename, Empty string);
		success = false;
	}
	else
	{
		if (inPlace == true)
		{
			retVal = dirtyFile;
			// Copy dirtyFile for the sake of recovery
			backUp = copy_a_string(dirtyFile);
			
			if (!backUp)
			{
				HARKLE_ERROR(Fileroad, clean_filename, copy_a_string failed);
				success = false;
			}
		}
		else
		{
			retVal = copy_a_string(dirtyFile);

			if (!retVal)
			{
				HARKLE_ERROR(Fileroad, clean_filename, copy_a_string failed);
				success = false;
			}
			else
			{
				dirtStrLen = strlen(retVal);
			}
		}
	}

	// BUILD BAD CHAR ARRAY
	if (success == true)
	{
		// 1. 0x01 - 0x1F
		for (i = 0; i < 31; i++)
		{
			(*(badChars + i)) = i + 1;
		}
		// 2. 0x7F
		(*(badChars + i)) = 0x7F;
		i++;
	}

	// CLEAN
	if (success == true)
	{
		// 1. Check first character for non-alpha && non-numeric
		for (i = 0; i < dirtStrLen; i++)
		{
			// Edge case
			if ((*(retVal + i)) == 0x2F)								// 0x2F == '/'
			{
				(*(retVal + i)) = 0x0;
			}
			// Wide swaths
			else if ((*(retVal + i)) < 0x2E)  							// 0x2E == '.'
			{
				(*(retVal + i)) = 0x0;
			}
			else if ((*(retVal + i)) > 0x39 && (*(retVal + i)) < 0x41)	// 0x39 == '9', 0x41 == 'A'
			{
				(*(retVal + i)) = 0x0;	
			}
			else if ((*(retVal + i)) > 0x5A && (*(retVal + i)) < 0x61)	// 0x5A == 'Z', 0x61 == 'a'
			{
				(*(retVal + i)) = 0x0;
			}
			else if ((*(retVal + i)) > 0x7A)  							// 0x7A == 'z'
			{
				(*(retVal + i)) = 0x0;	
			}

			// Did we find a leading "bad char"?
			if ((*(retVal + i)) != 0x0)
			{
				break;
			}
		}
		// 2. Fill in the 'new' nul-terminator (if any)
		if (*retVal == 0x0)
		{
			i = 0;  // Reset temp var
			// 2.1. Find the first non-nul
			while ((*(retVal + i)) == 0x0)
			{
				i++;
				// Verify we don't go out of bounds
				if (i >= dirtStrLen)
				{
					// Apparently, *all* of dirtyFile was 'dirty'
					success = false;
					break;
				}
			}

			if (success == true)
			{
				// 2.2. Copy left
				temp_ptr = retVal;  // Reset temp var
				while ((*(retVal + i)) != 0x0)
				{
					*temp_ptr = (*(retVal + i));
					temp_ptr++;
					i++;
				}

				// 2.3. Resize dirtStrLen
				dirtStrLen = strlen(retVal);

				if (dirtStrLen < 1)
				{
					HARKLE_ERROR(Fileroad, clean_filename, copy left algorithm appears to have failed);
					success = false;
				}
			}
		}
	}
	
	// 3. Start replacing *ALL* bad chars
	if (success == true)
	{
		temp_ptr = badChars;  // Reset temp var
		for (i = 0; i < dirtStrLen; i++)
		{
			while (*temp_ptr != 0x0)
			{
				if ((*(retVal + i)) == *temp_ptr)
				{
					(*(retVal + i)) = '_';
				}

				temp_ptr++;  // Next badChar
			}

			temp_ptr = badChars;  // Reset temp var
		}
	}

	// CLEAN UP
	if (backUp)
	{
		if (false == release_a_string(&backUp))
		{
			HARKLE_ERROR(Fileroad, clean_filename, release_a_string failed);
		}
	}

	if (success == false && inPlace == false && retVal)
	{
		if (false == release_a_string(&retVal))
		{
			HARKLE_ERROR(Fileroad, clean_filename, release_a_string failed);
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
