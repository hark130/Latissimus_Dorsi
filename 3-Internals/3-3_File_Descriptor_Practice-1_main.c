#define _GNU_SOURCE

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
	// LOCAL VARIABLES
	int retVal = 0;
	char* filename = NULL;
	fdDetails_ptr fileStruct_ptr = NULL;

	// INPUT VALIDATION
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
		// CREATE A FILE
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
			// PRINT FILE DETAILS
			fprintf(stdout, "\n\nSuccessfully opened %s\n", fileStruct_ptr->filename_ptr);
			fprintf(stdout, "The file descriptor is %d\n", fileStruct_ptr->fileDesc);
			fprintf(stdout, "The size of the file is %ld bytes\n", fileStruct_ptr->fileSize);
			fprintf(stdout, "The file descriptor flags:\t");
			print_binary(stdout, &(fileStruct_ptr->fileDescFlags), sizeof(fileStruct_ptr->fileDescFlags), false);
			fprintf(stdout, "\n");
			fprintf(stdout, "\tFD_CLOEXEC:\t%d\n", FD_CLOEXEC & fileStruct_ptr->fileDescFlags ? 1 : 0);
			fprintf(stdout, "\nThe file status flags:\t\t");
			print_binary(stdout, &(fileStruct_ptr->fileStatFlags), sizeof(fileStruct_ptr->fileStatFlags), false);
			fprintf(stdout, "\n");
			// O_RDONLY == 0 so don't use BIN_CHECK() or you'll get a false positive
			// O_RDONLY == 0 so we have to be tricky here
			// http://www.delorie.com/djgpp/doc/incs/fcntl.h
			fprintf(stdout, "\tO_RDONLY:\t%d\n", (O_WRONLY | O_RDWR) & fileStruct_ptr->fileStatFlags ? 0 : 1);
			fprintf(stdout, "\tO_WRONLY:\t%d\n", BIN_CHECK(O_WRONLY, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_RDWR: \t%d\n", BIN_CHECK(O_RDWR, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_ACCMODE: \t%d\n", BIN_CHECK(O_ACCMODE, fileStruct_ptr->fileStatFlags));

			// Apparently, these macros aren't in fcntl.h
			// fprintf(stdout, "\tO_BINARY: \t%d\n", BIN_CHECK(O_BINARY, fileStruct_ptr->fileStatFlags));
			// fprintf(stdout, "\tO_TEXT: \t%d\n", BIN_CHECK(O_TEXT, fileStruct_ptr->fileStatFlags));
			// fprintf(stdout, "\tO_NOINHERIT: \t%d\n", BIN_CHECK(O_NOINHERIT, fileStruct_ptr->fileStatFlags));

			fprintf(stdout, "\tO_CREAT:\t%d\n", BIN_CHECK(O_CREAT, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_EXCL: \t%d\n", BIN_CHECK(O_EXCL, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_NOCTTY: \t%d\n", BIN_CHECK(O_NOCTTY, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_TRUNC: \t%d\n", BIN_CHECK(O_TRUNC, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_APPEND:\t%d\n", BIN_CHECK(O_APPEND, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_NONBLOCK: \t%d\n", BIN_CHECK(O_NONBLOCK, fileStruct_ptr->fileStatFlags));

			fprintf(stdout, "\tO_ASYNC: \t%d\n", BIN_CHECK(O_ASYNC, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_DIRECT: \t%d\n", BIN_CHECK(O_DIRECT, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_NOATIME: \t%d\n", BIN_CHECK(O_NOATIME, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_DSYNC: \t%d\n", BIN_CHECK(O_DSYNC, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tO_SYNC: \t%d\n", BIN_CHECK(O_SYNC, fileStruct_ptr->fileStatFlags));

			// File Types (?)
			// https://elixir.free-electrons.com/linux/v2.6.32/source/include/linux/stat.h
			fprintf(stdout, "\tS_IFMT: \t%d\n", BIN_CHECK(S_IFMT, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tS_IFSOCK: \t%d\n", BIN_CHECK(S_IFSOCK, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tS_IFLNK: \t%d\n", BIN_CHECK(S_IFLNK, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tS_IFREG: \t%d\n", BIN_CHECK(S_IFREG, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tS_IFBLK: \t%d\n", BIN_CHECK(S_IFBLK, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tS_IFDIR: \t%d\n", BIN_CHECK(S_IFDIR, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tS_IFCHR: \t%d\n", BIN_CHECK(S_IFCHR, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tS_IFIFO: \t%d\n", BIN_CHECK(S_IFIFO, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tS_ISUID: \t%d\n", BIN_CHECK(S_ISUID, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tS_ISGID: \t%d\n", BIN_CHECK(S_ISGID, fileStruct_ptr->fileStatFlags));
			fprintf(stdout, "\tS_ISVTX: \t%d\n", BIN_CHECK(S_ISVTX, fileStruct_ptr->fileStatFlags));

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
