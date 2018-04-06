#ifndef __HARKLEMATH__
#define __HARKLEMATH__

#include <stdbool.h>		// bool, true, false

typedef struct cartesianCoordinate
{
	double xCoord;			// X coordinate
	double yCoord;			// Y coordinate
} cartPnt, *cartPnt_ptr;

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
		This function will attempt to determine/calculate the maximum level
			of precision for the system it is compiled on.
		I haven't yet determined how I want to handle TOO much desired
			precision.
		Also, I haven't yet determined how I want to handle the system's
			maximum level of precision if the caller's desired level of
			precision exceeds the system's maximum precision.
 */
double calc_precision(int precision);


//////////////////////////////////////////////////////////////////////////////
/////////////////////// FLOATING POINT FUNCTIONS STOP ////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
////////////////////////// GEOMETRIC FUNCTIONS START /////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	PURPOSE - Calculate a point on an elipse given a, b, and y for the
		standard equation of an elipse:
			x²   y²
			─  + ─  = 1
			a²   b²
	INPUT
		aVal - "a" from the standard equation above
		bVal - "b" from the standard equation above
		yVal - "y" from the standard equation above
	OUTPUT
		On success, "x" from the equation above
		On failure, 0 (since x can never be zero for a centered ellipse)
	NOTES
		This function only returns the positive result.  Return value
			could also be negative.
 */
double calc_ellipse_x_coord(double aVal, double bVal, double yVal);


/*
	PURPOSE - Calculate a point on an elipse given a, b, and y for the
		standard equation of an elipse:
			x²   y²
			─  + ─  = 1
			a²   b²
	INPUT
		aVal - "a" from the standard equation above
		bVal - "b" from the standard equation above
		xVal - "x" from the standard equation above
	OUTPUT
		On success, "y" from the equation above
		On failure, 0 (since x can never be zero for a centered ellipse)
	NOTES
		This function only returns the positive result.  Return value
			could also be negative.
 */
double calc_ellipse_y_coord(double aVal, double bVal, double xVal);


//////////////////////////////////////////////////////////////////////////////
////////////////////////// GEOMETRIC FUNCTIONS STOP //////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __HARKLEMATH__
