#include "Fileroad.h"
#include "Memoroad.h"
#include <limits.h>		// PATH_MAX
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
	char bigBuff[PATH_MAX + 1] = { "/" };
	int counter = 0;

	for (counter = 1; counter < PATH_MAX; counter++)
	{
		(*(bigBuff + counter)) = 'H';
	}
	// fprintf(stdout, "Full String:\t%s\n", bigBuff);  // DEBUGGING

/******************************************************************************************************************************************/
/***************************************************************** LEGEND *****************************************************************/
/******************************************************************************************************************************************/
/*	Data type  Var Name       Test Name         	Path            Join            isFile  ret   Expected return string                      */
/******************************************************************************************************************************************/
	// Normal Tests
	testStruct normTest01 = { "Normal Test 1",  	"/home/", 		"joe", 			false, 	NULL, "/home/joe/" };
	testStruct normTest02 = { "Normal Test 2",  	"/home/", 		"joe", 			true, 	NULL, "/home/joe" };
	testStruct normTest03 = { "Normal Test 3",  	"/home", 		"joe", 			false, 	NULL, "/home/joe/" };
	testStruct normTest04 = { "Normal Test 4",  	"/home", 		"joe", 			true, 	NULL, "/home/joe" };
	testStruct normTest05 = { "Normal Test 5",  	"/home/joe/", 	"Documents", 	false, 	NULL, "/home/joe/Documents/" };
	testStruct normTest06 = { "Normal Test 6",  	"/home/joe/", 	"Documents", 	true, 	NULL, "/home/joe/Documents" };
	testStruct normTest07 = { "Normal Test 7",  	"/home/", 		"/joe/", 		false, 	NULL, "/home/joe/" };
	testStruct normTest08 = { "Normal Test 8",  	"/home/", 		"/joe/", 		true, 	NULL, "/home/joe" };
	testStruct normTest09 = { "Normal Test 9",  	"/home", 		"/joe", 		false, 	NULL, "/home/joe/" };
	testStruct normTest10 = { "Normal Test 10", 	"/home", 		"/joe", 		true, 	NULL, "/home/joe" };
	testStruct normTest11 = { "Normal Test 11", 	"home", 		"joe", 			false, 	NULL, "/home/joe/" };
	testStruct normTest12 = { "Normal Test 12", 	"home", 		"joe", 			true, 	NULL, "/home/joe" };
	testStruct normTest13 = { "Normal Test 13", 	"/", 			"home",			false, 	NULL, "/home/" };
	testStruct normTest14 = { "Normal Test 14", 	"/", 			"home", 		true, 	NULL, "/home" };

	// Error Tests
	testStruct errTest01 = { "Error Test 1",  		NULL, 			"joe", 			false, 	NULL, NULL };
	testStruct errTest02 = { "Error Test 2",  		"/home/", 		NULL, 			false, 	NULL, NULL };
	testStruct errTest03 = { "Error Test 3",  		NULL, 			NULL, 			false, 	NULL, NULL };
	testStruct errTest04 = { "Error Test 4",  		"", 			"joe", 			false, 	NULL, NULL };
	testStruct errTest05 = { "Error Test 5",  		"/home/joe/", 	"", 			false, 	NULL, NULL };
	testStruct errTest06 = { "Error Test 6",  		"", 			"", 			false, 	NULL, NULL };
	testStruct errTest07 = { "Error Test 7",  		NULL, 			"joe", 			true, 	NULL, NULL };
	testStruct errTest08 = { "Error Test 8",  		"/home/", 		NULL, 			true, 	NULL, NULL };
	testStruct errTest09 = { "Error Test 9",  		NULL, 			NULL, 			true, 	NULL, NULL };
	testStruct errTest10 = { "Error Test 10",  		"", 			"joe", 			true, 	NULL, NULL };
	testStruct errTest11 = { "Error Test 11",  		"/home/joe/", 	"", 			true, 	NULL, NULL };
	testStruct errTest12 = { "Error Test 12",  		"", 			"", 			true, 	NULL, NULL };

	// Boundary Tests
	testStruct bndTest01 = { "Boundary Test 1",  	"/", 			"a", 			false, 	NULL, "/a/" };		// Pass - Barely big enough
	testStruct bndTest02 = { "Boundary Test 2",  	"/", 			"a", 			true, 	NULL, "/a" };		// Pass - Barely big enough
	testStruct bndTest03 = { "Boundary Test 3",  	bigBuff, 		bigBuff, 		false, 	NULL, NULL };		// Fail - Too big
	testStruct bndTest04 = { "Boundary Test 4",  	bigBuff, 		bigBuff, 		true, 	NULL, NULL };		// Fail - Too big
	testStruct bndTest05 = { "Boundary Test 5",  	"a", 			"b", 			false, 	NULL, "/a/b/" };	// Pass - Barely big enough
	testStruct bndTest06 = { "Boundary Test 6",  	"a", 			"b", 			true, 	NULL, "/a/b" };		// Pass - Barely big enough
	testStruct bndTest07 = { "Boundary Test 7",  	"a/", 			"b", 			false, 	NULL, "/a/b/" };	// Pass - Barely big enough
	testStruct bndTest08 = { "Boundary Test 8",  	"a/", 			"b", 			true, 	NULL, "/a/b" };		// Pass - Barely big enough
	testStruct bndTest09 = { "Boundary Test 9",  	"a/", 			"/b", 			false, 	NULL, "/a/b/" };	// Pass - Barely big enough
	testStruct bndTest10 = { "Boundary Test 10",  	"a/", 			"/b", 			true, 	NULL, "/a/b" };		// Pass - Barely big enough
	testStruct bndTest11 = { "Boundary Test 11",  	"a", 			"b/", 			false, 	NULL, "/a/b/" };	// Pass - Barely big enough
	testStruct bndTest12 = { "Boundary Test 12",  	"a", 			"b/", 			true, 	NULL, "/a/b" };		// Pass - Barely big enough

	testStruct_ptr normTest_arr[] = { &normTest01, &normTest02, &normTest03, &normTest04, &normTest05, &normTest06, &normTest07, &normTest08, &normTest09, &normTest10, &normTest11, &normTest12, &normTest13, &normTest14, NULL };
	testStruct_ptr errTest_arr[] = { &errTest01, &errTest02, &errTest03, &errTest04, &errTest05, &errTest06, &errTest07, &errTest08, &errTest09, &errTest10, &errTest11, &errTest12, NULL };
	testStruct_ptr bndTest_arr[] = { &bndTest01, &bndTest02, &bndTest03, &bndTest04, &bndTest05, &bndTest06, &bndTest07, &bndTest08, &bndTest09, &bndTest10, &bndTest11, &bndTest12, NULL };

	testStruct_ptr* testArrays_arr[] = { normTest_arr, errTest_arr, bndTest_arr, NULL };

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
	fprintf(stdout, "Tests Run:   \t%d\n", numTestsRun);
	fprintf(stdout, "Tests Passed:\t%d\n", numTestsPassed);
	fprintf(stdout, "\n\n");

	// DONE
	return 0;
}
