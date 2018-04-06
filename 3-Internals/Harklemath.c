#include "Harklerror.h"			// HARKLE_ERROR
#include <math.h>				// sqrt()
#include <stdbool.h>			// bool, true, false

// PLACEHOLDERS FOR DOUBLE COMPARISON FUNCTIONS
// double is a 64 bit IEEE 754 double precision Floating Point Number (1 bit for the sign, 11 bits for the exponent, and 52* bits for the value), i.e. double has 15 decimal digits of precision.
#define DBL_PRECISION 15
#define DBL_GRTR(x, y) ((x > y) ? true : false)
#define DBL_GRTR_EQ(x, y) ((x >= y) ? true : false)
#define DBL_EQUAL(x, y) ((x == y) ? true : false)
#define DBL_LESS(x, y) ((x < y) ? true : false)
#define DBL_LESS_EQ(x, y) ((x <= y) ? true : false)


/*
	PURPOSE - A helper function, local to Harklemath.c, to calculate *this* 
		machine's maximum level of precision at runtime
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

//////////////////////////////////////////////////////////////////////////////
/////////////////////// FLOATING POINT FUNCTIONS START ///////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	PURPOSE - Determine if one double is greater than another to a certain
		level of precision
	INPUT
		x - Is x greater than...
		y - ...y...
		precision - ...considering this many decimal places?
	OUTPUT
		If x > y, true
		Otherwise, false
 */
bool dble_greater_than(double x, double y, int precision);


/*
	PURPOSE - Determine if one double is less than another to a certain
		level of precision
	INPUT
		x - Is x less than...
		y - ...y...
		precision - ...considering this many decimal places?
	OUTPUT
		If x < y, true
		Otherwise, false
 */
bool dble_less_than(double x, double y, int precision);


/*
	PURPOSE - Determine if one double is equal to another to a certain
		level of precision
	INPUT
		x - Is x equal...
		y - ...y...
		precision - ...considering this many decimal places?
	OUTPUT
		If x == y, true
		Otherwise, false
 */
bool dble_equal_to(double x, double y, int precision);


/*
	PURPOSE - Determine if one double is NOT equal to another to a certain
		level of precision
	INPUT
		x - Is x NOT equal...
		y - ...y...
		precision - ...considering this many decimal places?
	OUTPUT
		If x != y, true
		Otherwise, false
	NOTES
		This function calls and returns the opposite of:
			dble_equal_to()
 */
bool dble_not_equal(double x, double y, int precision);


/*
	PURPOSE - Determine if one double is greater than or equal to
		another to a certain level of precision
	INPUT
		x - Is x greater than or equal to...
		y - ...y...
		precision - ...considering this many decimal places?
	OUTPUT
		If x >= y, true
		Otherwise, false
	NOTES
		This function will call the following functions in this order:
			dble_equal_to()
			dble_greater_than()
 */
bool dble_greater_than_equal_to(double x, double y, int precision);


/*
	PURPOSE - Determine if one double is less than another to a certain
		level of precision
	INPUT
		x - Is x less than or equal to...
		y - ...y...
		precision - ...considering this many decimal places?
	OUTPUT
		If x <= y, true
		Otherwise, false
	NOTES
		This function will call the following functions in this order:
			dble_equal_to()
			dble_less_than()
 */
bool dble_less_than_equal_to(double x, double y, int precision);


/*
	PURPOSE - Extricate precision 'mask' calculation from the floating point
		functions
	INPUT
		precision - Number of decimal points of precision desired
	OUTPUT
		On success, a double value to be used as a comparative value
		On failure, 0
	NOTES
		This function does not support levels of precision below 1
		This function will attempt to determine/calculate the maximum level
			of precision for the system it is compiled on.
		I haven't yet determined how I want to handle TOO much desired
			precision.
		Also, I haven't yet determined how I want to handle the system's
			maximum level of precision if the caller's desired level of
			precision exceeds the system's maximum precision.
 */
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
		printf("%2d digits accuracy in calculations\n", counter);  // DEBUGGING

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
		printf("%2d digits accuracy in storage\n", counter);  // DEBUGGING
		
		// Determine which accuracy count is least-specific
		if (counter < retVal)
		{
			retVal = counter;	
		}
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////// LOCAL HELPER FUNCTIONS STOP /////////////////////////
//////////////////////////////////////////////////////////////////////////////
