#ifndef __HARKLEMATH__
#define __HARKLEMATH__

typedef struct cartesianCoordinate
{
	double xCoord;			// X coordinate
	double yCoord;			// Y coordinate
} cartPnt, *cartPnt_ptr;

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