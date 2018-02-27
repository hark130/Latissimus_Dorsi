#include "Fileroad.h"
#include "Memoroad.h"
#include <stdbool.h>	// bool, true, false
#include <stdio.h>		// fprintf
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
		path_ptr == "/proc/", join_ptr == "/31337/", isFile == false, returns "/proc/31337/"
		path_ptr == "/proc/", join_ptr == "/31337/", isFile == true, returns "/proc/31337"
char* os_path_join(char* path_ptr, char* join_ptr, bool isFile);
 */

typedef struct opjTestStruct
{
	char* testName;			// Name and number of test
	char* inputPath_ptr;	// 1st argument
	char* inputJoin_ptr;	// 2nd argument
	bool isFileInput;		// 3rd argument
	char* actualRet;		// Actual return value
	char* expectRet;		// Expected return value
} testStruct, *testStruct_ptr;

typedef testStruct_ptr* testStructPtr_arr;
typedef testStructPtr_arr* testArr_arr;

int main(void)
{
	// LOCAL VARIABLES
	testStruct_ptr currTest_ptr = NULL;  // Current test being run

	// Normal Tests
	testStruct normTest1 = { "Normal Test 1", "/home/", "joe", false, "/home/joe/", NULL };

	testStruct_ptr normTest_arr = { &normTest1, NULL };
	int numNormTests = sizeof(normTest_arr) / sizeof(*normTest_arr);


	// RUN TESTS
	// fprintf(stdout, "test_arr ==     %p\n", test_arr);
	// fprintf(stdout, "allTests_ptr == %p\n", allTests_ptr);

	for (int testNum = 0; testNum < numNormTests; testNum++)
	{
		currTest_ptr = (*(normTest_arr + testNum));

		if (currTest_ptr)
		{
			// EXECUTE TESTS
			fprintf(stdout, "%s\n\t", currTest_ptr->testName);
		}
	}	


				




	// DONE
	return 0;
}