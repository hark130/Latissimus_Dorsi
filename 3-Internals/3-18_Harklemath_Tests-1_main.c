#include "Harklemath.h"
#include "Harklerror.h"
#include <stdbool.h>            // bool, true, false
#include <stdlib.h>             // free()
#include <string.h>             // memset()

typedef struct floatingPointComparisonTestStruct
{
	char* testName;         // Name and number of test
	double xIn;             // "x" input
	double yIn;             // "y" input
	int precIn;             // "precision" input
	bool gtExp;             // Expected results for greater_than()
    bool ltExp;             // Expected results for less_than()
    bool eqExp;             // Expected results for equal_to()
    bool neqExp;            // Expected results for not_equal_to()
    bool gteExp;            // Expected results for greater_than_equal()
    bool lteExp;            // Expected results for less_than_equal()
} fpcTest, *fpcTest_ptr;

typedef struct plotEllipsePointsTestStruct
{
    char* testName;         // Name and number of test
    double aIn;             // "a" input
    double bIn;             // "b" input
    int* num_ptr;           // Pointer which holds number of points
    int expNumPts;          // Expected number of points
    double* ellArr;         // Array of ellipse coordinate points
    double* expArr;         // Array of expected ellipse coordinate points
} pepTest, *pepTest_ptr;


int main(void)
{
    /***************************************************************************************************/
    /******************************* FLOATING POINT COMPARISON UNIT TESTS ******************************/
    /***************************************************************************************************/
    // LOCAL VARIABLES
	fpcTest_ptr test = NULL;  // Current test being run
	fpcTest_ptr* currTestArr_ptr = NULL;  // Current test array
	fpcTest_ptr** allTests = NULL;  // All the arrays
	int numTestsRun = 0;
    int numTestsPassed = 0;
    
/******************************************************************************************************************************************/
/***************************************************************** LEGEND *****************************************************************/
/******************************************************************************************************************************************/
/*  Data type  Var Name       Test Name         xIn                   yIn                  p   gtExp  ltExp  eqExp  neqExp gteExp lteExp  */
/******************************************************************************************************************************************/
	// Normal Tests
    fpcTest    normTest00 = { "Normal Test 00", (double)-10,          (double)10,          15, false, true,  false, true,  false, true };
    fpcTest    normTest01 = { "Normal Test 01", -10.1,                10.1,                15, false, true,  false, true,  false, true };
    fpcTest    normTest02 = { "Normal Test 02", -10.01,               10.01,               15, false, true,  false, true,  false, true };
    fpcTest    normTest03 = { "Normal Test 03", -10.001,              10.001,              15, false, true,  false, true,  false, true };
    fpcTest    normTest04 = { "Normal Test 04", -10.0001,             10.0001,             15, false, true,  false, true,  false, true };
    fpcTest    normTest05 = { "Normal Test 05", -10.00001,            10.00001,            15, false, true,  false, true,  false, true };
    fpcTest    normTest06 = { "Normal Test 06", -10.000001,           10.000001,           15, false, true,  false, true,  false, true };
    fpcTest    normTest07 = { "Normal Test 07", -10.0000001,          10.0000001,          15, false, true,  false, true,  false, true };
    fpcTest    normTest08 = { "Normal Test 08", -10.00000001,         10.00000001,         15, false, true,  false, true,  false, true };
    fpcTest    normTest09 = { "Normal Test 09", -10.000000001,        10.000000001,        15, false, true,  false, true,  false, true };
    fpcTest    normTest10 = { "Normal Test 10", -10.0000000001,       10.0000000001,       15, false, true,  false, true,  false, true };
    fpcTest    normTest11 = { "Normal Test 11", -10.00000000001,      10.00000000001,      15, false, true,  false, true,  false, true };
    fpcTest    normTest12 = { "Normal Test 12", -10.000000000001,     10.000000000001,     15, false, true,  false, true,  false, true };
    fpcTest    normTest13 = { "Normal Test 13", -10.0000000000001,    10.0000000000001,    15, false, true,  false, true,  false, true };
    fpcTest    normTest14 = { "Normal Test 14", -10.00000000000001,   10.00000000000001,   15, false, true,  false, true,  false, true };
    fpcTest    normTest15 = { "Normal Test 15", -10.000000000000001,  10.000000000000001,  15, false, true,  false, true,  false, true };
    fpcTest    normTest16 = { "Normal Test 16", (double)10,           (double)-10,         15, true,  false, false, true,  true,  false };
    fpcTest    normTest17 = { "Normal Test 17", 10.1,                 -10.1,               15, true,  false, false, true,  true,  false };
    fpcTest    normTest18 = { "Normal Test 18", 10.01,                -10.01,              15, true,  false, false, true,  true,  false };
    fpcTest    normTest19 = { "Normal Test 19", 10.001,               -10.001,             15, true,  false, false, true,  true,  false };
    fpcTest    normTest20 = { "Normal Test 20", 10.0001,              -10.0001,            15, true,  false, false, true,  true,  false };
    fpcTest    normTest21 = { "Normal Test 21", 10.00001,             -10.00001,           15, true,  false, false, true,  true,  false };
    fpcTest    normTest22 = { "Normal Test 22", 10.000001,            -10.000001,          15, true,  false, false, true,  true,  false };
    fpcTest    normTest23 = { "Normal Test 23", 10.0000001,           -10.0000001,         15, true,  false, false, true,  true,  false };
    fpcTest    normTest24 = { "Normal Test 24", 10.00000001,          -10.00000001,        15, true,  false, false, true,  true,  false };
    fpcTest    normTest25 = { "Normal Test 25", 10.000000001,         -10.000000001,       15, true,  false, false, true,  true,  false };
    fpcTest    normTest26 = { "Normal Test 26", 10.0000000001,        -10.0000000001,      15, true,  false, false, true,  true,  false };
    fpcTest    normTest27 = { "Normal Test 27", 10.00000000001,       -10.00000000001,     15, true,  false, false, true,  true,  false };
    fpcTest    normTest28 = { "Normal Test 28", 10.000000000001,      -10.000000000001,    15, true,  false, false, true,  true,  false };
    fpcTest    normTest29 = { "Normal Test 29", 10.0000000000001,     -10.0000000000001,   15, true,  false, false, true,  true,  false };
    fpcTest    normTest30 = { "Normal Test 30", 10.00000000000001,    -10.00000000000001,  15, true,  false, false, true,  true,  false };
    fpcTest    normTest31 = { "Normal Test 31", 10.000000000000001,   -10.000000000000001, 15, true,  false, false, true,  true,  false };
    fpcTest    normTest32 = { "Normal Test 32", (double)10,           (double)10,          15, false, false, true,  false, true,  true };
    fpcTest    normTest33 = { "Normal Test 33", 10.1,                 10.1,                15, false, false, true,  false, true,  true };
    fpcTest    normTest34 = { "Normal Test 34", 10.01,                10.01,               15, false, false, true,  false, true,  true };
    fpcTest    normTest35 = { "Normal Test 35", 10.001,               10.001,              15, false, false, true,  false, true,  true };
    fpcTest    normTest36 = { "Normal Test 36", 10.0001,              10.0001,             15, false, false, true,  false, true,  true };
    fpcTest    normTest37 = { "Normal Test 37", 10.00001,             10.00001,            15, false, false, true,  false, true,  true };
    fpcTest    normTest38 = { "Normal Test 38", 10.000001,            10.000001,           15, false, false, true,  false, true,  true };
    fpcTest    normTest39 = { "Normal Test 39", 10.0000001,           10.0000001,          15, false, false, true,  false, true,  true };
    fpcTest    normTest40 = { "Normal Test 40", 10.00000001,          10.00000001,         15, false, false, true,  false, true,  true };
    fpcTest    normTest41 = { "Normal Test 41", 10.000000001,         10.000000001,        15, false, false, true,  false, true,  true };
    fpcTest    normTest42 = { "Normal Test 42", 10.0000000001,        10.0000000001,       15, false, false, true,  false, true,  true };
    fpcTest    normTest43 = { "Normal Test 43", 10.00000000001,       10.00000000001,      15, false, false, true,  false, true,  true };
    fpcTest    normTest44 = { "Normal Test 44", 10.000000000001,      10.000000000001,     15, false, false, true,  false, true,  true };
    fpcTest    normTest45 = { "Normal Test 45", 10.0000000000001,     10.0000000000001,    15, false, false, true,  false, true,  true };
    fpcTest    normTest46 = { "Normal Test 46", 10.00000000000001,    10.00000000000001,   15, false, false, true,  false, true,  true };
    fpcTest    normTest47 = { "Normal Test 47", 10.000000000000001,   10.000000000000001,  15, false, false, true,  false, true,  true };
    fpcTest    normTest48 = { "Normal Test 48", (double)-10,          (double)-10,         15, false, false, true,  false, true,  true };
    fpcTest    normTest49 = { "Normal Test 49", -10.1,                -10.1,               15, false, false, true,  false, true,  true };
    fpcTest    normTest50 = { "Normal Test 50", -10.01,               -10.01,              15, false, false, true,  false, true,  true };
    fpcTest    normTest51 = { "Normal Test 51", -10.001,              -10.001,             15, false, false, true,  false, true,  true };
    fpcTest    normTest52 = { "Normal Test 52", -10.0001,             -10.0001,            15, false, false, true,  false, true,  true };
    fpcTest    normTest53 = { "Normal Test 53", -10.00001,            -10.00001,           15, false, false, true,  false, true,  true };
    fpcTest    normTest54 = { "Normal Test 54", -10.000001,           -10.000001,          15, false, false, true,  false, true,  true };
    fpcTest    normTest55 = { "Normal Test 55", -10.0000001,          -10.0000001,         15, false, false, true,  false, true,  true };
    fpcTest    normTest56 = { "Normal Test 56", -10.00000001,         -10.00000001,        15, false, false, true,  false, true,  true };
    fpcTest    normTest57 = { "Normal Test 57", -10.000000001,        -10.000000001,       15, false, false, true,  false, true,  true };
    fpcTest    normTest58 = { "Normal Test 58", -10.0000000001,       -10.0000000001,      15, false, false, true,  false, true,  true };
    fpcTest    normTest59 = { "Normal Test 59", -10.00000000001,      -10.00000000001,     15, false, false, true,  false, true,  true };
    fpcTest    normTest60 = { "Normal Test 60", -10.000000000001,     -10.000000000001,    15, false, false, true,  false, true,  true };
    fpcTest    normTest61 = { "Normal Test 61", -10.0000000000001,    -10.0000000000001,   15, false, false, true,  false, true,  true };
    fpcTest    normTest62 = { "Normal Test 62", -10.00000000000001,   -10.00000000000001,  15, false, false, true,  false, true,  true };
    fpcTest    normTest63 = { "Normal Test 63", -10.000000000000001,  -10.000000000000001, 15, false, false, true,  false, true,  true };
	// Special Tests
    fpcTest    specTest00 = { "Special Test 00", (double)-10,          (double)10,          3, false, true,  false, true,  false, true };
    fpcTest    specTest01 = { "Special Test 01", -10.1,                10.1,                3, false, true,  false, true,  false, true };
    fpcTest    specTest02 = { "Special Test 02", -10.01,               10.01,               3, false, true,  false, true,  false, true };
    fpcTest    specTest03 = { "Special Test 03", -10.001,              10.001,              3, false, true,  false, true,  false, true };
    fpcTest    specTest04 = { "Special Test 04", -10.0001,             10.0001,             3, false, true,  false, true,  false, true };
    fpcTest    specTest05 = { "Special Test 05", -10.00001,            10.00001,            3, false, true,  false, true,  false, true };
    fpcTest    specTest06 = { "Special Test 06", -10.000001,           10.000001,           3, false, true,  false, true,  false, true };
    fpcTest    specTest07 = { "Special Test 07", -10.0000001,          10.0000001,          3, false, true,  false, true,  false, true };
    fpcTest    specTest08 = { "Special Test 08", -10.00000001,         10.00000001,         3, false, true,  false, true,  false, true };
    fpcTest    specTest09 = { "Special Test 09", -10.000000001,        10.000000001,        3, false, true,  false, true,  false, true };
    fpcTest    specTest10 = { "Special Test 10", -10.0000000001,       10.0000000001,       3, false, true,  false, true,  false, true };
    fpcTest    specTest11 = { "Special Test 11", -10.00000000001,      10.00000000001,      3, false, true,  false, true,  false, true };
    fpcTest    specTest12 = { "Special Test 12", -10.000000000001,     10.000000000001,     3, false, true,  false, true,  false, true };
    fpcTest    specTest13 = { "Special Test 13", -10.0000000000001,    10.0000000000001,    3, false, true,  false, true,  false, true };
    fpcTest    specTest14 = { "Special Test 14", -10.00000000000001,   10.00000000000001,   3, false, true,  false, true,  false, true };
    fpcTest    specTest15 = { "Special Test 15", -10.000000000000001,  10.000000000000001,  3, false, true,  false, true,  false, true };
    fpcTest    specTest16 = { "Special Test 16", (double)10,           (double)-10,         3, true,  false, false, true,  true,  false };
    fpcTest    specTest17 = { "Special Test 17", 10.1,                 -10.1,               3, true,  false, false, true,  true,  false };
    fpcTest    specTest18 = { "Special Test 18", 10.01,                -10.01,              3, true,  false, false, true,  true,  false };
    fpcTest    specTest19 = { "Special Test 19", 10.001,               -10.001,             3, true,  false, false, true,  true,  false };
    fpcTest    specTest20 = { "Special Test 20", 10.0001,              -10.0001,            3, true,  false, false, true,  true,  false };
    fpcTest    specTest21 = { "Special Test 21", 10.00001,             -10.00001,           3, true,  false, false, true,  true,  false };
    fpcTest    specTest22 = { "Special Test 22", 10.000001,            -10.000001,          3, true,  false, false, true,  true,  false };
    fpcTest    specTest23 = { "Special Test 23", 10.0000001,           -10.0000001,         3, true,  false, false, true,  true,  false };
    fpcTest    specTest24 = { "Special Test 24", 10.00000001,          -10.00000001,        3, true,  false, false, true,  true,  false };
    fpcTest    specTest25 = { "Special Test 25", 10.000000001,         -10.000000001,       3, true,  false, false, true,  true,  false };
    fpcTest    specTest26 = { "Special Test 26", 10.0000000001,        -10.0000000001,      3, true,  false, false, true,  true,  false };
    fpcTest    specTest27 = { "Special Test 27", 10.00000000001,       -10.00000000001,     3, true,  false, false, true,  true,  false };
    fpcTest    specTest28 = { "Special Test 28", 10.000000000001,      -10.000000000001,    3, true,  false, false, true,  true,  false };
    fpcTest    specTest29 = { "Special Test 29", 10.0000000000001,     -10.0000000000001,   3, true,  false, false, true,  true,  false };
    fpcTest    specTest30 = { "Special Test 30", 10.00000000000001,    -10.00000000000001,  3, true,  false, false, true,  true,  false };
    fpcTest    specTest31 = { "Special Test 31", 10.000000000000001,   -10.000000000000001, 3, true,  false, false, true,  true,  false };
    fpcTest    specTest32 = { "Special Test 32", (double)10,           (double)10,          3, false, false, true,  false, true,  true };
    fpcTest    specTest33 = { "Special Test 33", 10.1,                 10.1,                3, false, false, true,  false, true,  true };
    fpcTest    specTest34 = { "Special Test 34", 10.01,                10.01,               3, false, false, true,  false, true,  true };
    fpcTest    specTest35 = { "Special Test 35", 10.001,               10.001,              3, false, false, true,  false, true,  true };
    fpcTest    specTest36 = { "Special Test 36", 10.0001,              10.0001,             3, false, false, true,  false, true,  true };
    fpcTest    specTest37 = { "Special Test 37", 10.00001,             10.00001,            3, false, false, true,  false, true,  true };
    fpcTest    specTest38 = { "Special Test 38", 10.000001,            10.000001,           3, false, false, true,  false, true,  true };
    fpcTest    specTest39 = { "Special Test 39", 10.0000001,           10.0000001,          3, false, false, true,  false, true,  true };
    fpcTest    specTest40 = { "Special Test 40", 10.00000001,          10.00000001,         3, false, false, true,  false, true,  true };
    fpcTest    specTest41 = { "Special Test 41", 10.000000001,         10.000000001,        3, false, false, true,  false, true,  true };
    fpcTest    specTest42 = { "Special Test 42", 10.0000000001,        10.0000000001,       3, false, false, true,  false, true,  true };
    fpcTest    specTest43 = { "Special Test 43", 10.00000000001,       10.00000000001,      3, false, false, true,  false, true,  true };
    fpcTest    specTest44 = { "Special Test 44", 10.000000000001,      10.000000000001,     3, false, false, true,  false, true,  true };
    fpcTest    specTest45 = { "Special Test 45", 10.0000000000001,     10.0000000000001,    3, false, false, true,  false, true,  true };
    fpcTest    specTest46 = { "Special Test 46", 10.00000000000001,    10.00000000000001,   3, false, false, true,  false, true,  true };
    fpcTest    specTest47 = { "Special Test 47", 10.000000000000001,   10.000000000000001,  3, false, false, true,  false, true,  true };
    fpcTest    specTest48 = { "Special Test 48", (double)-10,          (double)-10,         3, false, false, true,  false, true,  true };
    fpcTest    specTest49 = { "Special Test 49", -10.1,                -10.1,               3, false, false, true,  false, true,  true };
    fpcTest    specTest50 = { "Special Test 50", -10.01,               -10.01,              3, false, false, true,  false, true,  true };
    fpcTest    specTest51 = { "Special Test 51", -10.001,              -10.001,             3, false, false, true,  false, true,  true };
    fpcTest    specTest52 = { "Special Test 52", -10.0001,             -10.0001,            3, false, false, true,  false, true,  true };
    fpcTest    specTest53 = { "Special Test 53", -10.00001,            -10.00001,           3, false, false, true,  false, true,  true };
    fpcTest    specTest54 = { "Special Test 54", -10.000001,           -10.000001,          3, false, false, true,  false, true,  true };
    fpcTest    specTest55 = { "Special Test 55", -10.0000001,          -10.0000001,         3, false, false, true,  false, true,  true };
    fpcTest    specTest56 = { "Special Test 56", -10.00000001,         -10.00000001,        3, false, false, true,  false, true,  true };
    fpcTest    specTest57 = { "Special Test 57", -10.000000001,        -10.000000001,       3, false, false, true,  false, true,  true };
    fpcTest    specTest58 = { "Special Test 58", -10.0000000001,       -10.0000000001,      3, false, false, true,  false, true,  true };
    fpcTest    specTest59 = { "Special Test 59", -10.00000000001,      -10.00000000001,     3, false, false, true,  false, true,  true };
    fpcTest    specTest60 = { "Special Test 60", -10.000000000001,     -10.000000000001,    3, false, false, true,  false, true,  true };
    fpcTest    specTest61 = { "Special Test 61", -10.0000000000001,    -10.0000000000001,   3, false, false, true,  false, true,  true };
    fpcTest    specTest62 = { "Special Test 62", -10.00000000000001,   -10.00000000000001,  3, false, false, true,  false, true,  true };
    fpcTest    specTest63 = { "Special Test 63", -10.000000000000001,  -10.000000000000001, 3, false, false, true,  false, true,  true };
    fpcTest    specTest64 = { "Special Test 64", (double)10,           (double)10,          3, false, false, true,  false, true,  true };
    fpcTest    specTest65 = { "Special Test 65", 10.1,                 10.2,                3, false, true,  false, true,  false, true };
    fpcTest    specTest66 = { "Special Test 66", 10.01,                10.02,               3, false, true,  false, true,  false, true };
    fpcTest    specTest67 = { "Special Test 67", 10.001,               10.002,              3, false, true,  false, true,  false, true };
    fpcTest    specTest68 = { "Special Test 68", 10.0001,              10.0002,             3, false, false, true,  false, true,  true };
    fpcTest    specTest69 = { "Special Test 69", 10.00001,             10.00002,            3, false, false, true,  false, true,  true };
    fpcTest    specTest70 = { "Special Test 70", 10.000001,            10.000002,           3, false, false, true,  false, true,  true };
    fpcTest    specTest71 = { "Special Test 71", 10.0000001,           10.0000002,          3, false, false, true,  false, true,  true };
    fpcTest    specTest72 = { "Special Test 72", 10.00000001,          10.00000002,         3, false, false, true,  false, true,  true };
    fpcTest    specTest73 = { "Special Test 73", 10.000000001,         10.000000002,        3, false, false, true,  false, true,  true };
    fpcTest    specTest74 = { "Special Test 74", 10.0000000001,        10.0000000002,       3, false, false, true,  false, true,  true };
    fpcTest    specTest75 = { "Special Test 75", 10.00000000001,       10.00000000002,      3, false, false, true,  false, true,  true };
    fpcTest    specTest76 = { "Special Test 76", 10.000000000001,      10.000000000002,     3, false, false, true,  false, true,  true };
    fpcTest    specTest77 = { "Special Test 77", 10.0000000000001,     10.0000000000002,    3, false, false, true,  false, true,  true };
    fpcTest    specTest78 = { "Special Test 78", 10.00000000000001,    10.00000000000002,   3, false, false, true,  false, true,  true };
    fpcTest    specTest79 = { "Special Test 79", 10.000000000000001,   10.000000000000002,  3, false, false, true,  false, true,  true };
/******************************************************************************************************************************************/
/*  Data type  Var Name       Test Name          xIn                   yIn                 p   gtExp  ltExp  eqExp  neqExp gteExp lteExp  */
/******************************************************************************************************************************************/
/***************************************************************** LEGEND *****************************************************************/
/******************************************************************************************************************************************/

	fpcTest_ptr nrmTest_arr[] = { \
		&normTest00, &normTest01, &normTest02, &normTest03, &normTest04, \
		&normTest05, &normTest06, &normTest07, &normTest08, &normTest09, \
		&normTest10, &normTest11, &normTest12, &normTest13, &normTest14, \
		&normTest15, &normTest16, &normTest17, &normTest18, &normTest19, \
		&normTest20, &normTest21, &normTest22, &normTest23, &normTest24, \
		&normTest25, &normTest26, &normTest27, &normTest28, &normTest29, \
		&normTest30, &normTest31, &normTest32, &normTest33, &normTest34, \
		&normTest35, &normTest36, &normTest37, &normTest38, &normTest39, \
		&normTest40, &normTest41, &normTest42, &normTest43, &normTest44, \
		&normTest45, &normTest46, &normTest47, &normTest48, &normTest49, \
		&normTest50, &normTest51, &normTest52, &normTest53, &normTest54, \
		&normTest55, &normTest56, &normTest57, &normTest58, &normTest59, \
		&normTest60, &normTest61, &normTest62, &normTest63, NULL };
		
	fpcTest_ptr spcTest_arr[] = { \
		&specTest00, &specTest01, &specTest02, &specTest03, &specTest04, \
		&specTest05, &specTest06, &specTest07, &specTest08, &specTest09, \
		&specTest10, &specTest11, &specTest12, &specTest13, &specTest14, \
		&specTest15, &specTest16, &specTest17, &specTest18, &specTest19, \
		&specTest20, &specTest21, &specTest22, &specTest23, &specTest24, \
		&specTest25, &specTest26, &specTest27, &specTest28, &specTest29, \
		&specTest30, &specTest31, &specTest32, &specTest33, &specTest34, \
		&specTest35, &specTest36, &specTest37, &specTest38, &specTest39, \
		&specTest40, &specTest41, &specTest42, &specTest43, &specTest44, \
		&specTest45, &specTest46, &specTest47, &specTest48, &specTest49, \
		&specTest50, &specTest51, &specTest52, &specTest53, &specTest54, \
		&specTest55, &specTest56, &specTest57, &specTest58, &specTest59, \
		&specTest60, &specTest61, &specTest62, &specTest63, &specTest64, \
		&specTest65, &specTest66, &specTest67, &specTest68, &specTest69, \
		&specTest70, &specTest71, &specTest72, &specTest73, &specTest74, \
		&specTest75, &specTest76, &specTest77, &specTest78, &specTest79, \
        NULL };

	fpcTest_ptr* testArrays_arr[] = { nrmTest_arr, spcTest_arr, NULL };
	
	// RUN TESTS
	allTests = testArrays_arr;
    fprintf(stdout, "\n\t");

	while(*allTests)
	{
		currTestArr_ptr = *allTests;

		while (*currTestArr_ptr)
		{
			test = *currTestArr_ptr;

			if (test)
			{
				// EXECUTE TESTS
				fprintf(stdout, "%s\n\t\t", test->testName);
				// 1. dble_greater_than
				fprintf(stdout, "dble_greater_than()           ");
				numTestsRun++;
				if (test->gtExp == dble_greater_than(test->xIn, test->yIn, test->precIn))
				{
					fprintf(stdout, "[X] Success\n\t\t");
					numTestsPassed++;
				}
				else
				{		
					fprintf(stdout, "[ ] FAIL    ");
					fprintf(stdout, "Expected:\t%s\n\t\t", (true == test->gtExp) ? "true" : "false");
				}
				// 2. dble_less_than
				fprintf(stdout, "dble_less_than()              ");
				numTestsRun++;
				if (test->ltExp == dble_less_than(test->xIn, test->yIn, test->precIn))
				{
					fprintf(stdout, "[X] Success\n\t\t");
					numTestsPassed++;
				}
				else
				{		
					fprintf(stdout, "[ ] FAIL    ");
					fprintf(stdout, "Expected:\t%s\n\t\t", (true == test->ltExp) ? "true" : "false");
				}
				// 3. dble_equal_to
				fprintf(stdout, "dble_equal_to()               ");
				numTestsRun++;
				if (test->eqExp == dble_equal_to(test->xIn, test->yIn, test->precIn))
				{
					fprintf(stdout, "[X] Success\n\t\t");
					numTestsPassed++;
				}
				else
				{		
					fprintf(stdout, "[ ] FAIL    ");
					fprintf(stdout, "Expected:\t%s\n\t\t", (true == test->eqExp) ? "true" : "false");
				}
				// 4. dble_not_equal
				fprintf(stdout, "dble_not_equal()              ");
				numTestsRun++;
				if (test->neqExp == dble_not_equal(test->xIn, test->yIn, test->precIn))
				{
					fprintf(stdout, "[X] Success\n\t\t");
					numTestsPassed++;
				}
				else
				{		
					fprintf(stdout, "[ ] FAIL    ");
					fprintf(stdout, "Expected:\t%s\n\t\t", (true == test->neqExp) ? "true" : "false");
				}
				// 5. dble_greater_than_equal_to
				fprintf(stdout, "dble_greater_than_equal_to()  ");
				numTestsRun++;
				if (test->gteExp == dble_greater_than_equal_to(test->xIn, test->yIn, test->precIn))
				{
					fprintf(stdout, "[X] Success\n\t\t");
					numTestsPassed++;
				}
				else
				{		
					fprintf(stdout, "[ ] FAIL    ");
					fprintf(stdout, "Expected:\t%s\n\t\t", (true == test->gteExp) ? "true" : "false");
				}
				// 6. dble_less_than_equal_to
				fprintf(stdout, "dble_less_than_equal_to()     ");
				numTestsRun++;
				if (test->lteExp == dble_less_than_equal_to(test->xIn, test->yIn, test->precIn))
				{
					fprintf(stdout, "[X] Success\n\t");
					numTestsPassed++;
				}
				else
				{		
					fprintf(stdout, "[ ] FAIL    ");
					fprintf(stdout, "Expected:\t%s\n\t", (true == test->lteExp) ? "true" : "false");
				}
			}

			currTestArr_ptr++;
		}

		allTests++;
	}

    /***************************************************************************************************/
    /********************************** PLOT ELLIPSE POINT UNIT TESTS **********************************/
    /***************************************************************************************************/
    // LOCAL VARIABLES
    pepTest_ptr pepUnitTest = NULL;  // Current test being run
    pepTest_ptr* pepTestArr_ptr = NULL;  // Current test array
    pepTest_ptr** pepTests = NULL;  // All the arrays
    int pepTmpInt = 0;  // Temp integer to store numPoints in
    int xPnt = 0;  // Index for a x coordinate
    int yPnt = 1;  // Index for a y coordinate
    double xCalc = 0;  // A calculated x coordinate
    double yCalc = 0;  // A calculated y coordinate
    bool pepPassed = true;  // Make this false if anything fails
    void* tmp_ptr = NULL;  // Return value from memset
	// (-a, 0), (0, b), (+a, 0), (0, -b), (-a + 1, y)
/******************************************************************************************************************************************/
/***************************************************************** LEGEND *****************************************************************/
/******************************************************************************************************************************************/
/*                                (-a, 0)            (0, b)           (+a, 0)            (0, -b)                                          */
/*  Data type  Var Name          Test Name         aIn             bIn           num_ptr      expNum    ellArr  expArr                    */
/******************************************************************************************************************************************/
    // Normal Tests
    double     pepNormArry00[] = { -3, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, -2, 0, 0, 0, 0 };
    pepTest    pepNormTest00 = { "Normal Test 00", (double)3,     (double)2,     &pepTmpInt,  24,        NULL,  pepNormArry00 };


    pepTest_ptr pepNrmTest_arr[] = { \
        &pepNormTest00, NULL };

    pepTest_ptr* pepTestArrays_arr[] = { pepNrmTest_arr, NULL };

    // RUN TESTS
    pepTests = pepTestArrays_arr;
    fprintf(stdout, "\n");

    while(*pepTests)
    {
        pepTestArr_ptr = *pepTests;

        while (*pepTestArr_ptr)
        {
            pepUnitTest = *pepTestArr_ptr;

            if (pepUnitTest)
            {
                // EXECUTE TEST
                fprintf(stdout, "%s\n\t", pepUnitTest->testName);
                pepTmpInt = 0;  // Reset temp variable
                pepPassed = true;  // Reset temp variable
                // Function call
                pepUnitTest->ellArr = plot_ellipse_points(pepUnitTest->aIn, pepUnitTest->bIn, pepUnitTest->num_ptr);
                numTestsRun++;
                // 1. Verify pointer received
                if (NULL == pepUnitTest->ellArr)
                {
                    fprintf(stdout, "[ ] FAIL    NULL pointer returned\n");
                    pepPassed = false;
                }
                else
                {
                    // 2. Verify number of points is correct
                    if (*(pepUnitTest->num_ptr) != pepUnitTest->expNumPts)
                    {
                        fprintf(stdout, "[ ] FAIL    Number of points\n\t\t");
                        fprintf(stdout, "Expected: %d\n\t\tReceived: %d\n", pepUnitTest->expNumPts, *(pepUnitTest->num_ptr));
                        pepPassed = false;
                    }
                    else
                    {
                        // 3. Parse points in array
                        while (yPnt < pepUnitTest->expNumPts)
                        {
                            // 3.1. Verify calculations
                            xCalc = calc_ellipse_x_coord(pepUnitTest->aIn, pepUnitTest->bIn, pepUnitTest->ellArr[yPnt]);
                            yCalc = calc_ellipse_y_coord(pepUnitTest->aIn, pepUnitTest->bIn, pepUnitTest->ellArr[xPnt]);

                            if (false == dble_equal_to(xCalc, pepUnitTest->ellArr[xPnt], DBL_PRECISION) \
                                || false == dble_equal_to(yCalc, pepUnitTest->ellArr[yPnt], DBL_PRECISION))
                            {
                                fprintf(stdout, "[ ] FAIL    Point calculation mismatch\n\t\t");
                                fprintf(stdout, "Expected: (%.15f,%.15f)\n\t\tReceived: (%.15f,%.15f)\n", xCalc, yCalc, \
                                        pepUnitTest->ellArr[xPnt], pepUnitTest->ellArr[yPnt]);
                                pepPassed = false;
                                break;
                            }
                            
                            // 3.2. Verify against expected value (if any)
                            if (pepUnitTest->expArr[xPnt] && pepUnitTest->expArr[yPnt])
                            {
                                if (pepUnitTest->expArr[xPnt] != pepUnitTest->ellArr[xPnt] \
                                    || pepUnitTest->expArr[yPnt] != pepUnitTest->ellArr[yPnt])
                                {
                                    fprintf(stdout, "[ ] FAIL    Expected value mismatch\n\t\t");
                                    fprintf(stdout, "Expected: (%.15f,%.15f)\n\t\tReceived: (%.15f,%.15f)\n", \
                                            pepUnitTest->expArr[xPnt], pepUnitTest->expArr[yPnt], \
                                            pepUnitTest->ellArr[xPnt], pepUnitTest->ellArr[yPnt]);
                                    pepPassed = false;
                                    break;
                                }
                            }                            

                            // Iterate to the next points
                            xPnt += 2;
                            yPnt += 2;
                        }

                        // Determine pass/fail
                        if (true == pepPassed)
                        {
                            numTestsPassed++;
                        }
                    }
                }

                // CLEAN UP TEST RESULT
                if (pepUnitTest->ellArr)
                {
                    if (pepTmpInt > 0)
                    {
                        // Zeroize the array
                        tmp_ptr = memset(pepUnitTest->ellArr, 0x0, sizeof(double) * pepTmpInt);
                    }

                    // Free the array
                    free(pepUnitTest->ellArr);

                    // NULL the array
                    pepUnitTest->ellArr = NULL;
                }
            }

            pepTestArr_ptr++;
        }

        pepTests++;
    }

	// REPORT RESULTS
	fprintf(stdout, "\n\nTests Run:   \t%d\n", numTestsRun);
	fprintf(stdout,     "Tests Passed:\t%d\n\n", numTestsPassed);
	
    // DONE
    return 0;
}

/*
typedef struct plotEllipsePointsTestStruct
{
    char* testName;         // Name and number of test
    double aIn;             // "a" input
    double bIn;             // "b" input
    int* numPts;            // Pointer which holds number of points
    int expNumPts;          // Expected number of points
    double* ellArr;         // Array of ellipse coordinate points
    double* expArr;         // Array of expected ellipse coordinate points
} pepTest, *pepTest_ptr;
 */