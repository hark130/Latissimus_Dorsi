#include <fenv.h>				// fegetround(), fesetround()
#include <float.h>				// DBL_MANT_DIG, DBL_MIN_EXP
#include "Harklemath.h"			// HM_RND, HM_UP, HM_DWN, HM_IN
#include "Harklerror.h"			// HARKLE_ERROR
#include <limits.h>				// INT_MIN, INT_MAX
#include <math.h>				// sqrt()
#include <stdbool.h>			// bool, true, false
#include <stdio.h>				// sprintf()
#include <stdlib.h>				// atof()

// PLACEHOLDERS FOR DOUBLE COMPARISON FUNCTIONS
// double is a 64 bit IEEE 754 double precision Floating Point Number (1 bit for the sign, 11 bits for the exponent, and 52* bits for the value), i.e. double has 15 decimal digits of precision.
#define DBL_PRECISION 15
#define DBL_GRTR(x, y) ((x > y) ? true : false)
#define DBL_GRTR_EQ(x, y) ((x >= y) ? true : false)
#define DBL_EQUAL(x, y) ((x == y) ? true : false)
#define DBL_LESS(x, y) ((x < y) ? true : false)
#define DBL_LESS_EQ(x, y) ((x <= y) ? true : false)


/*
	PURPOSE - A local helper function, local to Harklemath.c, to calculate
		*this* machine's maximum level of precision at runtime
	INPUT - None
	OUTPUT
		On success, maximum level of decimal places supported
		On failure, 0
	NOTES
		This funciton will attempt to calculate both "accuracy in
			calculations" and "accuracy in storage" then return the lowest
			value between the two
 */
int calc_max_precision(void);


/*
	PURPOSE - Quick/dirty method of removing certain decimal places from
		a double
	INPUT
		val - Double to truncate
		digits - Number of decimals places to keep, truncate everything else
	OUTPUT
		On success, val with only digits-number of decimal places
		On failure, 0
 */
double truncate_double(double val, int digits);


//////////////////////////////////////////////////////////////////////////////
/////////////////////// FLOATING POINT FUNCTIONS START ///////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	PURPOSE - Abstract the process of rounding a double to an int
	INPUT
		roundMe - The double that needs to be rounded
		rndDir - The direction to round the double
	OUTPUT
		On success, the integer representation of the rounded double
		On failure, 0
	NOTES
		If rndDir is invalid, round_a_dble() will default to the current env
 */
int round_a_dble(double roundMe, int rndDir)
{
	// LOCAL VARIABLES
	int retVal = 0;
	bool success = true;  // If anything fails, make this false
	bool restoreNeeded = false;  // Set this to true if environment variable is changed
	int currState = 0;  // Current rounding environment variable to enable restoration
	
	// INPUT VALIDATION
	if (roundMe > (double)(INT_MAX * 1.0))
	{
		HARKLE_ERROR(Harklemath, round_a_dble, int overflow);
		success = false;
	}
	else if (roundMe < (double)(INT_MIN * 1.0))
	{
		HARKLE_ERROR(Harklemath, round_a_dble, int underflow);
		success = false;
	}
	
	// DETERMINE ROUNDING DIRECTION
	if (true == success)
	{
		if (rndDir == HM_RND || rndDir == HM_UP || rndDir == HM_DWN || rndDir == HM_IN)
		{
			// Turn on the "clean up later" flag
			restoreNeeded = true;
			// Get the current rounding environment variable
			currState = fegetround();
			// Set the rounding environment variable
			if (fesetround(rndDir))
			{
				HARKLE_ERROR(Harklemath, round_a_dble, fesetround failed);
				success = false;
			}
		}
	}
	
	// ROUND
	if (true == success)
	{
		retVal = round(roundMe);
	}
	
	// CLEAN UP
	if (true == restoreNeeded)
	{
		// Restore the rounding environment variable
		if (fesetround(currState))
		{
			HARKLE_ERROR(Harklemath, round_a_dble, fesetround failed);
		}
	}
	
	// DONE
	return retVal;
}
	
	
bool dble_greater_than(double x, double y, int precision)
{
	// LOCAL VARIABLES
	bool retVal = false;  // Prove this wrong
	bool success = true;  // Set this to false if anything fails
	double dbleMask = 0;  // "Mask" to remove undesired values of doubles
	
	// INPUT VALIDATION
	if (precision < 1)
	{
		HARKLE_ERROR(Harklemath, dble_greater_than, Invalid precision);
		success = false;
	}
	
	// CALC PRECISION
	if (true == success)
	{
		dbleMask = calc_precision(precision);
		
		if (!dbleMask)
		{
			HARKLE_ERROR(Harklemath, dble_greater_than, calc_precision failed);
			success = false;
		}
	}
	
	// COMPARE DOUBLES
	if (true == success)
	{
		if (x > y && (x + dbleMask) > (y + dbleMask) && (x - dbleMask) > (y - dbleMask))
		{
			retVal = true;	
		}		
	}	
	
	// DONE
	return retVal;
}


bool dble_less_than(double x, double y, int precision)
{
	// LOCAL VARIABLES
	bool retVal = false;  // Prove this wrong
	bool success = true;  // Set this to false if anything fails
	double dbleMask = 0;  // "Mask" to remove undesired values of doubles
	double xVal = truncate_double(x, precision);
	double yVal = truncate_double(y, precision);
	
	// INPUT VALIDATION
	if (precision < 1)
	{
		HARKLE_ERROR(Harklemath, dble_greater_than, Invalid precision);
		success = false;
	}
	
	// CALC PRECISION
	if (true == success)
	{
		dbleMask = calc_precision(precision);
		
		if (!dbleMask)
		{
			HARKLE_ERROR(Harklemath, dble_greater_than, calc_precision failed);
			success = false;
		}
		// fprintf(stdout, "\nx == %.15f\ty == %.15f\tdbleMask == %.15f\n", x, y, dbleMask);  // DEBUGGING
		// fprintf(stdout, "\nxTrunc == %.15f\tyTrunc == %.15f\tprecision == %d\n\t\t", xVal, yVal, precision);  // DEBUGGING
	}
	
	// COMPARE DOUBLES
	if (true == success)
	{
		// fprintf(stdout, "\nx == %.15f\ty == %.15f\tdbleMask == %.15f\n", x, y, dbleMask);  // DEBUGGING
		// fprintf(stdout, "x < y == %.15f < %.15f == %s\n", x, y, (x < y) ? "true" : "false");  // DEBUGGING
		// fprintf(stdout, "(x + dbleMask) < (y + dbleMask) == %.15f < %.15f == %s\n", \
		// 	x + dbleMask, y + dbleMask, ((x + dbleMask) < (y + dbleMask)) ? "true" : "false");  // DEBUGGING
		// fprintf(stdout, "(x - dbleMask) < (y - dbleMask) == %.15f < %.15f == %s\n\t\t", \
		// 	x - dbleMask, y - dbleMask, ((x - dbleMask) < (y - dbleMask)) ? "true" : "false");  // DEBUGGING
		if (xVal < yVal)
		// if (x < y && (x + dbleMask) < (y + dbleMask) && (x - dbleMask) < (y - dbleMask))
		{
			retVal = true;	
		}		
	}	
	
	// DONE
	return retVal;
}


bool dble_equal_to(double x, double y, int precision)
{
	// LOCAL VARIABLES
	bool retVal = false;  // Prove this wrong
	bool success = true;  // Set this to false if anything fails
	double dbleMask = 0;  // "Mask" to remove undesired values of doubles
	
	// INPUT VALIDATION
	if (precision < 1)
	{
		HARKLE_ERROR(Harklemath, dble_greater_than, Invalid precision);
		success = false;
	}
	
	// CALC PRECISION
	if (true == success)
	{
		dbleMask = calc_precision(precision);
		
		if (!dbleMask)
		{
			HARKLE_ERROR(Harklemath, dble_greater_than, calc_precision failed);
			success = false;
		}
	}
	
	// COMPARE DOUBLES
	if (true == success)
	{
		if ((x + dbleMask) > y && (x - dbleMask) < y && x < (y + dbleMask) && x > (y - dbleMask))
		{
			retVal = true;	
		}		
	}	
	
	// DONE
	return retVal;
}


bool dble_not_equal(double x, double y, int precision)
{
	// LOCAL VARIABLES
	bool retVal = true;
	
	// CALL dble_equal_to()
	if (true == dble_equal_to(x, y, precision))
	{
		retVal = false;	
	}
	
	// DONE
	return retVal;
}


bool dble_greater_than_equal_to(double x, double y, int precision)
{
	// LOCAL VARIABLES
	bool retVal = false;
	
	// Equal to?
	if (true == dble_equal_to(x, y, precision))
	{
		retVal = true;	
	}
	// Greater than?
	if (true == retVal || true == dble_greater_than(x, y, precision))
	{
		retVal = true;	
	}
	
	// DONE
	return retVal;
}


bool dble_less_than_equal_to(double x, double y, int precision)
{
	// LOCAL VARIABLES
	bool retVal = false;
	
	// Equal to?
	if (true == dble_equal_to(x, y, precision))
	{
		retVal = true;	
	}
	// Less than?
	if (true == retVal || true == dble_less_than(x, y, precision))
	{
		retVal = true;	
	}
	
	// DONE
	return retVal;
}


double calc_precision(int precision)
{
	// LOCAL VARIABLES
	double retVal = 0.0;  
	bool success = true;  // If anything fails, make this false
	int counter = 0;  // Used to count the decimal places
	static int maxPrec = 0;  // Calculate this machine's maximum level of precision once
	int currPrec = 0;  // Level of precision this function uses, either maxPrec or precision
	
	// CALC MAX PRECISION
	if (0 == maxPrec)
	{
		maxPrec = calc_max_precision();
		
		if (0 == maxPrec)
		{
			HARKLE_ERROR(Harklemath, calc_precision, calc_max_precision failed);
			success = false;
		}
	}
	
	// INPUT VALIDATION
	if (true == success)
	{
		if (precision < 1)
		{
			HARKLE_ERROR(Harklemath, calc_precision, Invalid precision);
			success = false;
		}
		else if (precision > maxPrec)
		{
			currPrec = maxPrec;	
		}
		else
		{
			currPrec = precision;	
		}
	}
	
	// CREATE PRECISION MASK
	if (true == success)
	{
		retVal = 1.0;  // Starting point
		
		while (currPrec > 0 && counter < currPrec)
		{
			retVal *= 0.1;  // Shift the decimal
			counter++;
		}
	}
	
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////// FLOATING POINT FUNCTIONS STOP ////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
////////////////////////// GEOMETRIC FUNCTIONS START /////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
		  a
	x = ± ─ √(b² - y²)
		  b
 */
double calc_ellipse_x_coord(double aVal, double bVal, double yVal)
{
	// LOCAL VARIABLES
	double xVal = 0;
	bool success = true;  // If anything fails, set this to false

	// INPUT VALIDATION
	if (true == DBL_EQUAL(aVal, 0))
	{
		HARKLE_ERROR(Harklemath, calc_ellipse_y_coord, aVal is zero);
		success = false;
	}
	else if (true == DBL_EQUAL(bVal, 0))
	{
		HARKLE_ERROR(Harklemath, calc_ellipse_y_coord, bVal is zero);
		success = false;
	}
	else if (true == DBL_GRTR(yVal, bVal))
	{
		HARKLE_ERROR(Harklemath, calc_ellipse_y_coord, xVal is greater than aVal);
		success = false;
	}

	// CALC Y COORD
	if (true == success)
	{
		xVal = bVal * bVal;		// b²
		xVal -= yVal * yVal;	// b² - y²
		xVal = sqrt(xVal);		// √(b² - y²)
		xVal *= aVal;			// a * √(b² - y²)
		xVal /= bVal;			// (a / b) * √(b² - y²)
		xVal = fabs(xVal);		// | (a / b) * √(b² - y²) |
	}

	// DONE
	return xVal;
}


/*
		  b
	y = ± ─ √(a² - x²)
		  a
 */
double calc_ellipse_y_coord(double aVal, double bVal, double xVal)
{
	// LOCAL VARIABLES
	double yVal = 0;
	bool success = true;  // If anything fails, set this to false

	// INPUT VALIDATION
	if (true == DBL_EQUAL(aVal, 0))
	{
		HARKLE_ERROR(Harklemath, calc_ellipse_y_coord, aVal is zero);
		success = false;
	}
	else if (true == DBL_EQUAL(bVal, 0))
	{
		HARKLE_ERROR(Harklemath, calc_ellipse_y_coord, bVal is zero);
		success = false;
	}
	else if (true == DBL_GRTR(xVal, aVal))
	{
		HARKLE_ERROR(Harklemath, calc_ellipse_y_coord, xVal is greater than aVal);
		success = false;
	}

	// CALC Y COORD
	if (true == success)
	{
		yVal = aVal * aVal;		// a²
		yVal -= xVal * xVal;	// a² - x²
		yVal = sqrt(yVal);		// √(a² - x²)
		yVal *= bVal;			// b * √(a² - x²)
		yVal /= aVal;			// (b / a) * √(a² - x²)
		yVal = fabs(yVal);		// | (b / a) * √(a² - x²) |
	}

	// DONE
	return yVal;
}


//////////////////////////////////////////////////////////////////////////////
////////////////////////// GEOMETRIC FUNCTIONS STOP //////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//////////////////////// LOCAL HELPER FUNCTIONS START ////////////////////////
//////////////////////////////////////////////////////////////////////////////

int calc_max_precision(void)
{
	// LOCAL VARIABLES
	double num1 = 1.0;  // Used to help calculate accuracy in calcualtion and storage
	double num2 = 1.0;  // Used to help calculate accuracy in calcualtion and storage
	double result = 0;  // Used to help calculate accuracy in storage
	int counter = 0;  // Counts supported accuracy
	static int retVal = 0;  // Only calculate this once for the life of the program
	
	// CALCULATION
	if (!retVal)
	{
		// Determine digits of accuracy in calculation
		while (num1 + num2 != num1)
		{
			++counter;
			num2 = num2 / 10.0;
		}
		// printf("%2d digits accuracy in calculations\n", counter);  // DEBUGGING

		// Reset temp variables
		retVal = counter;  // Store this result
		num2 = 1.0;  // Reset temp variable
		counter = 0;  // Reset temp variable

		// Determine digits of accuracy in storage
		while (1)
		{
			result = num1 + num2;
			if (result == num1)
			{
				break;
			}
			++counter;
			num2 = num2 / 10.0;
		}
		// printf("%2d digits accuracy in storage\n", counter);  // DEBUGGING
		
		// Determine which accuracy count is least-specific
		if (counter < retVal)
		{
			retVal = counter;	
		}
	}

	// DONE
	return retVal;
}


double truncate_double(double val, int digits)
{
	// LOCAL VARIABLES
	double retVal = 0.0;
	char format[9] = { 0 };  // Big enough for "%%.1074f\0"
	char dbleBuff[3 + DBL_MANT_DIG - DBL_MIN_EXP + 1] = { 0 };


	// INPUT VALIDATION
	if (0 == digits)
	{
		retVal = val;
	}
	else if (digits < 0 || digits > 1074)
	{
		HARKLE_ERROR(Harklemath, truncate_double, Invalid number of digits);
	}
	else
	{
		// TRUNCATE
		// fprintf(stdout, "\nval == %.15f\ndigits == %d\n", val, digits);  // DEBUGGING
		// Dynamically build a sprintf format string
		sprintf(format, "%%.%df", digits);
		// Use that format string to truncate val into a string
		sprintf(dbleBuff, format, val);
		// Convert that string back to a double
		retVal = atof(dbleBuff);
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////// LOCAL HELPER FUNCTIONS STOP /////////////////////////
//////////////////////////////////////////////////////////////////////////////
