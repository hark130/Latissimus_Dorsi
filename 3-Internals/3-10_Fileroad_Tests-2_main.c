#include "Fileroad.h"
#include "Memoroad.h"
#include <stdbool.h>	// bool, true, false
#include <stdio.h>		// fprintf
#include <string.h>		// strcmp

#ifndef HARKLE_ERROR
#define HARKLE_ERROR(header, funcName, msg) do { fprintf(stderr, "<<<ERROR>>> - %s - %s() - %s!\n", #header, #funcName, #msg); } while (0);
#endif  // HARKLE_ERROR

typedef struct opjTestStruct
{
	char* testName;			// Name and number of test
	char* inputPath_ptr;	// 1st argument
	char* inputJoin_ptr;	// 2nd argument
	bool isFileInput;		// 3rd argument
	char* actualRet;		// Actual return value
	char* expectRet;		// Expected return value
} testStruct, *testStruct_ptr;

int main(void)
{
	// LOCAL VARIABLES
	testStruct_ptr test = NULL;  // Current test being run
	testStruct_ptr* currTestArr_ptr = NULL;  // Current test array
	testStruct_ptr** allTests = NULL;  // All the arrays
	int numTestsRun = 0;
	int numTestsPassed = 0;

	// Normal Tests
	testStruct normTest01 = { "Normal Test 1",  "/home/", "joe", false, NULL, "/home/joe/" };
	testStruct normTest02 = { "Normal Test 2",  "/home/", "joe", true, NULL, "/home/joe" };
	testStruct normTest03 = { "Normal Test 3",  "/home", "joe", false, NULL, "/home/joe/" };
	testStruct normTest04 = { "Normal Test 4",  "/home", "joe", true, NULL, "/home/joe" };
	testStruct normTest05 = { "Normal Test 5",  "/home/joe/", "Documents", false, NULL, "/home/joe/Documents/" };
	testStruct normTest06 = { "Normal Test 6",  "/home/joe/", "Documents", true, NULL, "/home/joe/Documents" };
	testStruct normTest07 = { "Normal Test 7",  "/home/", "/joe/", false, NULL, "/home/joe/" };
	testStruct normTest08 = { "Normal Test 8",  "/home/", "/joe/", true, NULL, "/home/joe" };
	testStruct normTest09 = { "Normal Test 9",  "/home", "/joe", false, NULL, "/home/joe/" };
	testStruct normTest10 = { "Normal Test 10", "/home", "/joe", true, NULL, "/home/joe" };

	testStruct_ptr normTest_arr[] = { &normTest01, &normTest02, &normTest03, &normTest04, &normTest05, &normTest06, &normTest07, &normTest08, &normTest09, &normTest10, NULL };

	testStruct_ptr* testArrays_arr[] = { normTest_arr, NULL };


	// RUN TESTS
	allTests = testArrays_arr;

	while(*allTests)
	{
		currTestArr_ptr = *allTests;

		while (*currTestArr_ptr)
		{
			test = *currTestArr_ptr;

			if (test)
			{
				// EXECUTE TESTS
				fprintf(stdout, "%s\n\t", test->testName);
				test->actualRet = os_path_join(test->inputPath_ptr, test->inputJoin_ptr, test->isFileInput);
				numTestsRun++;

				if (test->actualRet && test->expectRet)
				{
					if (0 == strcmp(test->actualRet, test->expectRet))
					{
						fprintf(stdout, "[X] Success\n");
						numTestsPassed++;
					}
					else
					{		
						fprintf(stdout, "[ ] FAIL\n\t\t");
						fprintf(stdout, "Expect:\t%s\n\t\t", test->expectRet);
						fprintf(stdout, "Actual:\t%s\n\t\t\n", test->actualRet);
					}
				}
				else if (test->actualRet == test->expectRet)
				{
					fprintf(stdout, "[X] Success\n");
					numTestsPassed++;
				}
				else
				{
					fprintf(stdout, "[ ] FAIL\n\t\t");
					fprintf(stdout, "Expect:\t%s\n\t\t", test->expectRet);
					fprintf(stdout, "Actual:\t%s\n\t\t\n", test->actualRet);
				}

				// CLEAN UP THIS TEST
				if (test->actualRet)
				{
					if (false == release_a_string(&(test->actualRet)))
					{
						HARKLE_ERROR(Fileroad Tests, main, release_a_string failed);
					}
				}
			}

			currTestArr_ptr++;
		}

		allTests++;
	}

	// CLEAN UP
	fprintf(stdout, "\n\n");

	// DONE
	return 0;
}
