#include "Harklemath.h"
#include "Harklerror.h"
#include <stdbool.h>            // bool, true, false

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
/*  Data type  Var Name       Test Name         xIn                   yIn                  p   gtExp  ltExp  eqExp  neqExp gteExp lteExp                */
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
    fpcTest    normTest16 = { "Normal Test 16", (double)10,           (double)-10,         15, true,  false, false, true,  false, true };
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
	
    // DONE
    return 0;
}
