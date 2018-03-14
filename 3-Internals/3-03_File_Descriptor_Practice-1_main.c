// #define _GNU_SOURCE  // Don't force it

#include "Fileroad_Descriptors.h"
#include <fcntl.h>			// open flags
#include <stdbool.h>	    // bool, true, false
#include <stdio.h>			// fprintf
#include <string.h>			// strlen
// #include <sys/stat.h>
#include <unistd.h>			// close

#define BIN_CHECK(flag, val) ((flag & val) == flag ? 1 : 0)

void print_binary(FILE* stream, void* valueToPrint, size_t numBytesToPrint, bool bigEndian);

int main(int argc, char* argv[])
{
	// 1. LOCAL VARIABLES
	int retVal = 0;
	char* filename = NULL;
	fdDetails_ptr fileStruct_ptr = NULL;

	// 2. INPUT VALIDATION
	if (argc < 2)
	{
		fprintf(stderr, "<<<ERROR>>> - main() - Too few arguments!\n");
	}
	else if (argc > 2)
	{
		fprintf(stderr, "<<<ERROR>>> - main() - Too few arguments!\n");	
	}
	else if (argv[1] == NULL)
	{
		fprintf(stderr, "<<<ERROR>>> - main() - NULL argument?!?!\n");		
	}
	else if (strlen(argv[1]) < 1)
	{
		fprintf(stderr, "<<<ERROR>>> - main() - Empty argument?!?!\n");	
	}
	else
	{
		// 3. CREATE A FILE
		// Get the filename
		filename = argv[1];

		// Open the file descriptor
		// fileStruct_ptr = open_fd(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
		fileStruct_ptr = open_fd(filename, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );  // Read-only test

		if (fileStruct_ptr == NULL)
		{
			fprintf(stderr, "<<<ERROR>>> - main() - open_fd() has failed!\n");	
		}
		else
		{
			// 4. PRINT FILE DETAILS
			
			// 4.1. General File Details
			fprintf(stdout, "\n\nSuccessfully opened %s\n", fileStruct_ptr->filename_ptr);
			fprintf(stdout, "The file descriptor is %d\n", fileStruct_ptr->fileDesc);
			fprintf(stdout, "The size of the file is %ld bytes\n", fileStruct_ptr->fileSize);
			
			// 4.2. File Descriptor Flags
			fprintf(stdout, "The file descriptor flags:\t");
			print_binary(stdout, &(fileStruct_ptr->fileDescFlags), sizeof(fileStruct_ptr->fileDescFlags), false);
			fprintf(stdout, "\n");
			fprintf(stdout, "\tFD_CLOEXEC:\t%d\n", FD_CLOEXEC & fileStruct_ptr->fileDescFlags ? 1 : 0);
			
			// 4.3. File Status Flags
			fprintf(stdout, "\nThe file status flags:\t\t");
			print_binary(stdout, &(fileStruct_ptr->fileStatFlags), sizeof(fileStruct_ptr->fileStatFlags), false);
			
			// 4.3.1. File Access Mode Flags
			// https://www.gnu.org/software/libc/manual/html_node/Access-Modes.html#Access-Modes
			fprintf(stdout, "\n\tFile Access Modes:\n");
			// O_RDONLY == 0 so don't use BIN_CHECK() or you'll get a false positive
			// O_RDONLY == 0 so we have to be tricky here
			// http://www.delorie.com/djgpp/doc/incs/fcntl.h
			fprintf(stdout, "\tO_RDONLY:\t%d\n", (O_WRONLY | O_RDWR) & fileStruct_ptr->fileStatFlags ? 0 : 1);
			fprintf(stdout, "\tO_WRONLY:\t%d\n", BIN_CHECK(O_WRONLY, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_RDWR: \t%d\n", BIN_CHECK(O_RDWR, fileStruct_ptr->fileStatFlags));

			// 4.3.2. Open Time Flags
			// https://www.gnu.org/software/libc/manual/html_node/Open_002dtime-Flags.html#Open_002dtime-Flags
			// NOTES: 
			//	The open-time flags specify options affecting how open will behave. 
			// 	These options are not preserved once the file is open. 
			//	The exception to this is O_NONBLOCK, which is also an I/O operating mode and so it is saved.
			// fprintf(stdout, "\tO_CREAT:\t%d\n", BIN_CHECK(O_CREAT, fileStruct_ptr->fileStatFlags));
			// fprintf(stdout, "\tO_EXCL: \t%d\n", BIN_CHECK(O_EXCL, fileStruct_ptr->fileStatFlags));
			// fprintf(stdout, "\tO_NOCTTY: \t%d\n", BIN_CHECK(O_NOCTTY, fileStruct_ptr->fileStatFlags));
			
			// 4.3.3. I/O Operating Mode Flags
			// https://www.gnu.org/software/libc/manual/html_node/Operating-Modes.html#Operating-Modes
			// NOTES:
			//	The operating modes affect how input and output operations using a file descriptor work. 
			//	These flags are set by open and can be fetched and changed with fcntl.
			fprintf(stdout, "\tO_APPEND:\t%d\n", BIN_CHECK(O_APPEND, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_NONBLOCK: \t%d\n", BIN_CHECK(O_NONBLOCK, fileStruct_ptr->fileStatFlags));
			// O_NDELAY is an obsolete name for O_NONBLOCK, provided for compatibility with BSD. 
			//	It is not defined by the POSIX.1 standard. 
			fprintf(stdout, "\tO_NDELAY: \t%d\n", BIN_CHECK(O_NDELAY, fileStruct_ptr->fileStatFlags));

			// WRITE TO THE FILE DESCRIPTOR
			// Implement write and call it here?
			// getchar();
		}
	}

	// WRAP IT UP
	// Close the file descriptor
	if (fileStruct_ptr)
	{
		if (fileStruct_ptr->fileDesc > 2)
		{
			close(fileStruct_ptr->fileDesc);
		}
		// Free the struct
		free_fdDetails(&fileStruct_ptr);
	}

	// DONE
	fprintf(stdout, "\n\n");
	return retVal;
}


// Purpose:    Print a certain number of bytes in binary
// Input:    
//            stream - The stream to print output
//            valueToPrint - A variable to print data from
//            numBytesToPrint - The number of bytes to print
//            bigEndian - If true, bigEndian byte ordering
// Output:    None
// Note:    
//            Endianness does not appear to matter since these are flags
//            Function will print one space on invalid input
void print_binary(FILE* stream, void* valueToPrint, size_t numBytesToPrint, bool bigEndian)
{
    /* LOCAL VARIABLES */
    unsigned char mask = 0;            // Bit mask used to print bits
    unsigned char printThis = 0;    // Value to print
    int i = 0;                    // Iterating variable
    int j = 0;                    // Iterating variable

    /* INPUT VALIDATION */
    if (!stream)
    {
        return;
    }
    else if (!valueToPrint)
    {
        fprintf(stream, " ");
        return;
    }
    else if (numBytesToPrint < 1)
    {
        fprintf(stream, " ");
        return;
    }
    else if (bigEndian != true && bigEndian != false)
    {
        fprintf(stream, " ");
        return;
    }

    /* START PRINTING */
    if (bigEndian == true)
    {
        // for (i = (numBytesToPrint - 1); i >= 0; i--)
        for (i = 0; i < numBytesToPrint; i++)
        {
            printThis = (unsigned char)(*(unsigned char*)(valueToPrint + i));

            for (j = 7; j >= 0; j--)
            {
                if (j == 3)
                {
                    fprintf(stream, " ");
                }

                mask = 1 << j;
                // printf("i == %d\tj== %d\tMask:\t0x%02X\tCurrent Value:\t0x%02X\n", i, j, mask, printThis);  // DEBUGGING
                if (printThis & mask)
                {
                    fprintf(stream, "1");
                }
                else
                {
                    fprintf(stream, "0");
                }
            }
            fprintf(stream, " ");
        }
    }
    else if (bigEndian == false)
    {
        // for (i = 0; i < numBytesToPrint; i++)
        for (i = (numBytesToPrint - 1); i >= 0; i--)
        {
            printThis = (unsigned char)(*(unsigned char*)(valueToPrint + i));

            for (j = 7; j >= 0; j--)
            {
                if (j == 3)
                {
                    fprintf(stream, " ");
                }

                mask = 1 << j;
                // printf("i == %d\tj== %d\tMask:\t0x%02X\tCurrent Value:\t0x%02X\n", i, j, mask, printThis);  // DEBUGGING
                if (printThis & mask)
                {
                    fprintf(stream, "1");
                }
                else
                {
                    fprintf(stream, "0");
                }
            }
            fprintf(stream, " ");
        }
    }
    else
    {
        fprintf(stream, " ");
    }

    return;
}
