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
		xVal = aVal * aVal;		// a²
		xVal -= xVal * xVal;	// a² - x²
		xVal = sqrt(yVal);		// √(a² - x²)
		xVal *= bVal;			// b * √(a² - x²)
		xVal /= aVal;			// (b / a) * √(a² - x²)
		xVal = fabs(yVal);		// | (b / a) * √(a² - x²) |
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