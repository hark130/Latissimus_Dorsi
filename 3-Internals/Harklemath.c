#include <fenv.h>				// fegetround(), fesetround()
#include <float.h>				// DBL_MANT_DIG, DBL_MIN_EXP
#include "Harklecurse.h"		// hcCartCoord_ptr
#include "Harklemath.h"			// HM_* MACROs
#include "Harklerror.h"			// HARKLE_ERROR
#include <limits.h>				// INT_MIN, INT_MAX
#include <math.h>				// sqrt()
#include <stdbool.h>			// bool, true, false
#include <stdio.h>				// sprintf()
#include <stdlib.h>				// atof()
#include <string.h>             // memset

// PLACEHOLDERS FOR DOUBLE COMPARISON FUNCTIONS
#define DBL_GRTR(x, y) ((x > y) ? true : false)
#define DBL_GRTR_EQ(x, y) ((x >= y) ? true : false)
#define DBL_EQUAL(x, y) ((x == y) ? true : false)
#define DBL_LESS(x, y) ((x < y) ? true : false)
#define DBL_LESS_EQ(x, y) ((x <= y) ? true : false)

#ifndef HARKLEMATH_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define HARKLEMATH_MAX_TRIES 3
#endif  // HARKLEMATH_MAX_TRIES


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


/*
	PURPOSE - Translate plot points that are relative to the center of a windows
		into plot points that are absolute with relation to the upper left corner
		of a window
	INPUT
		relX - x point relative to the specified center coordinate
		relY - y point relative to the specified center coordinate
		cntX - x point of the center coordinate
		cntY - y point of the center coordinate
		absX - [OUT] translation of relX into an absolute reference point
			starting at (0, 0) if that represents the upper left hand corner
			of the window
		absY - [OUT] translation of relY into an absolute reference point
			starting at (0, 0) if that represents the upper left hand corner
			of the window
	OUTPUT
		On success, true (and absX/absY have the real results)
		On failure, false
	NOTES
		This function, were it not a local helper function, might best be served
			inside Harklecurse as it has been written to help translate plot
			points into a format easily digestible by ncurses window plotting
			functions.  This is because ncurses windows treat "home" (see: (0, 0))
			as the upper left hand corner of the window.
		As of now, this function is exclusively called by build_geometric_list()
 */
bool translate_plot_points(int relX, int relY, int cntX, int cntY, int* absX, int* absY);


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
		// HM_UP is handled with ceil() below
		// HM_DWN is handled with floor() below
		// HM_RND and HM_IN are handled with the combination of environment variables
		//	and round()
		if (rndDir == HM_RND || rndDir == HM_IN)
		// if (rndDir == HM_RND || rndDir == HM_UP || rndDir == HM_DWN || rndDir == HM_IN)
		{
			// Turn on the "clean up later" flag
			restoreNeeded = true;
			// Get the current rounding environment variable
			currState = fegetround();
			// Set the rounding environment variable
			if (fesetround(rndDir) || fegetround() != rndDir)
			{
				HARKLE_ERROR(Harklemath, round_a_dble, fesetround failed);
				success = false;
			}
		}
	}
	
	// ROUND
	if (true == success)
	{
		if (rndDir == HM_UP)
		{
			retVal = round(ceil(roundMe));
		}
		else if (rndDir == HM_DWN)
		{
			retVal = round(floor(roundMe));
		}
		else
		{
			retVal = round(roundMe);
		}
		fprintf(stdout, "\nroundMe == %.15f\trounded == %d\n", roundMe, retVal);  // DEBUGGING
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
		printf("\nxVal == %.15f\taVal == %.15f\n\t", xVal, aVal);  // DEBUGGING
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


/*
	PURPOSE - Calculate a set of points on an ellipse given the form:
			x²   y²
			─  + ─  = 1
			a²   b²
	INPUT
		aVal - "a" from the standard equation above
		bVal - "b" from the standard equation above
		numPnts - Pointer to a size_t variable in which to store the size
			of the returned array.
	OUTPUT
		On success, a heap-allocated array with the size of the
		On failure, 0 (since x can never be zero for a centered ellipse)
	NOTES
		The returned array is NOT terminated.  Take care to use numPnts.
		This function assumes origin of (0, 0) is the center of the ellipse.
		This function iterates through (0, y), (+x, 0), (0, -y), (-x, 0), and
			finally to (-1, y).
		Coordinate values chosen by this function are whole numbers
 */
double* plot_ellipse_points(double aVal, double bVal, int* numPnts)
{
	// LOCAL VARIABLES
	double* retVal = NULL;  // Array of double values for ellipse plot points
	bool success = true;  // If anything fails, make this false
	double aAbs = 0;  // Rounded absolute value of aVal
	double bAbs = 0;  // Rounded absolute value of bVal
	int majAbs = 0;  // Rounded absolute value of the major axis
	double majPnt = 0;  // Point along the major axis
	int numTries = 0;  // Keeps count of allocation attempts
	int numPoints = 0;  // Local count of the number of points in the array
	bool chooseX = true;  // Indicates x is the major axis
	void* tmp_ptr = NULL;  // Return value from memset
	int count = 0;  // Iterating variable
	// This variable is used to reflect points across the major axis
	double flipIt = 1;  // Multiple this by ellipse point function calls

	// INPUT VALIDATION
	if (true == dble_equal_to(aVal, 0, DBL_PRECISION))
	{
		HARKLE_ERROR(Harklemath, plot_ellipse_points, aVal is zero);
		success = false;
	}
	else if (true == dble_equal_to(bVal, 0, DBL_PRECISION))
	{
		HARKLE_ERROR(Harklemath, plot_ellipse_points, bVal is zero);
		success = false;
	}
	else if (NULL == numPnts)
	{
		HARKLE_ERROR(Harklemath, plot_ellipse_points, NULL pointer);
		success = false;
	}
	else
	{
		*numPnts = 0;
	}

	// DETERMINE MAJOR AXIS
	if (true == dble_less_than(aVal, bVal, DBL_PRECISION))
	{
		chooseX = false;  // Y holds the major axis
	}

	// DETERMINE NUMBER OF POINTS
	if (true == success)
	{
		if (true == dble_not_equal(fabs(aVal), ((int)fabs(aVal)) * 1.0, DBL_PRECISION))
		{
			aAbs = fabs(aVal);
			// aAbs = round_a_dble(fabs(aVal), HM_UP);
		}
		else
		{
			aAbs = fabs(aVal);
			// aAbs = ((int)fabs(aVal));
		}

		if (true == dble_not_equal(fabs(bVal), ((int)fabs(bVal)) * 1.0, DBL_PRECISION))
		{
			bAbs = fabs(bVal);
			// bAbs = round_a_dble(fabs(bVal), HM_UP);
		}
		else
		{
			bAbs = fabs(bVal);
			// bAbs = ((int)fabs(bVal));
		}
		fprintf(stdout, "aVal == %.15f\tbVal == %.15f\taAbs == %.15f\tbAbs == %.15f\n", aVal, bVal, aAbs, bAbs);  // DEBUGGING

		if (true == chooseX)
		{
			majAbs = aAbs;
		}
		else
		{
			majAbs = bAbs;
		}

		// if (0 == aAbs || 0 == bAbs)
		// {
		// 	// fprintf(stdout, "aVal == %.15f\tbVal == %.15f\taAbs == %d\tbAbs == %d\n", aVal, bVal, aAbs, bAbs);  // DEBUGGING
		// 	HARKLE_ERROR(Harklemath, plot_ellipse_points, round_a_dble failed);
		// 	success = false;
		// }
		// else
		// {
		// 	// majAbs is 1/2 the major axis
		// 	// 4 for the four quadrants
		// 	// 2 because each coordinate pair is represented by two doubles
		// 	numPoints = majAbs * 4 * 2;

		// 	if (numPoints < 8 || 0 != numPoints % 4)
		// 	{
		// 		HARKLE_ERROR(Harklemath, plot_ellipse_points, Number of points miscalculated);
		// 		success = false;
		// 	}
		// }


		// majAbs is 1/2 the major axis
		// 4 for the four quadrants
		// 2 because each coordinate pair is represented by two doubles
		numPoints = majAbs * 4 * 2;

		if (numPoints < 8 || 0 != numPoints % 4)
		{
			HARKLE_ERROR(Harklemath, plot_ellipse_points, Number of points miscalculated);
			success = false;
		}
	}

	// ALLOCATE BUFFER
	if (true == success)
	{
		while (numTries < HARKLEMATH_MAX_TRIES && !retVal)
		{
			retVal = (double*)calloc(numPoints, sizeof(double));
			numTries++;
		}

		if (!retVal)
		{
			HARKLE_ERROR(Harklemath, plot_ellipse_points, calloc failed);
			success = false;
		}
	}

	// CALCULATE COORDINATE PAIRS
	while (count < numPoints)
	{
		if (true == chooseX)
		{
			// Starting point on the major axis
			if (0 == count)  // (-a, 0)
			{
				majPnt = -1 * majAbs;
			}
			// Set the x-point for this coordinate in the array
			retVal[count] = majPnt;
			// fprintf(stdout, "%d x == %.15f\n", count, retVal[count]);  // DEBUGGING
			count++;  // Next point
			// Set y-pont for this coordinate in the array
			// fprintf(stdout, "\naAbs == %d\tbAbs == %d\tmajPnt == %.15f\n", aAbs, bAbs, majPnt);  // DEBUGGING
			retVal[count] = flipIt * calc_ellipse_y_coord(aAbs, bAbs, majPnt);
			// fprintf(stdout, "%d y == %.15f\n", count, retVal[count]);  // DEBUGGING
			// Error check calc_ellipse_y_coord()
			// if (0 == retVal[count])
			// {
			// 	HARKLE_ERROR(Harklemath, plot_ellipse_points, calc_ellipse_y_coord failed);
			// 	success = false;
			// 	break;
			// }
			count++;  // Next point

			// Continue incrementing along the major axis
			// fprintf(stdout, "\ncount == %d and numPoints == %d\n", count, numPoints);  // DEBUGGING
			///////////////////////////////////// BUG IS HERE //////////////////////////////////////
			if (count > numPoints / 2)  // (a, 0)
			{
				majPnt--;
				flipIt = -1;
			}
			else
			{
				majPnt++;
				flipIt = 1;
			}
		}
		else
		{
			// Starting point on the major axis
			if (0 == count)  // (-a, 0)
			{
				majPnt = 0;
				flipIt = -1;
			}
			// Set x in the array
			retVal[count] = flipIt * calc_ellipse_x_coord(aAbs, bAbs, majPnt);
			// Error check calc_ellipse_x_coord()
			// if (0 == retVal[count])
			// {
			// 	HARKLE_ERROR(Harklemath, plot_ellipse_points, calc_ellipse_x_coord failed);
			// 	success = false;
			// 	break;
			// }
			count++;  // Next point
			// Set y in the array
			retVal[count] = majPnt;

			// Continue incrementing along the major axis
			// Quadrant II
			if (count <= numPoints / 4)
			{
				majPnt++;
				flipIt = -1;
			}
			// Quadrant I
			// else if (count <= numPoints / 2)
			// {
			// 	majPnt--;
			// 	flipIt = 1;
			// }
			// Quadrant I through Quadrant IV
			else if (count <= (3 * numPoints / 4))
			{
				majPnt--;
				flipIt = 1;				
			}
			// Quadrant III
			else
			{
				majPnt++;
				flipIt = -1;
			}
		}
	}

	// WRAP UP/CLEAN UP
	// Wrap up
	if (true == success)
	{
		*numPnts = numPoints;
	}
	// Clean up
	else if (retVal)
	{
		// Zeroize retVal
		if (numPoints > 0)
		{
			tmp_ptr = memset(retVal, 0x0, numPoints);

			if (tmp_ptr != retVal)
			{
				HARKLE_ERROR(Harklemath, plot_ellipse_points, memset failed);
			}
		}
		// Free retVal
		free(retVal);
		// NULL retVal
		retVal = NULL;
	}

	// DONE
	return retVal;
}


/*
	PURPOSE - Determine the center coordinates of a rectangle given it's width
		and height.
	INPUT
		width - Width of the window
		height - Height of the window
		xCoord - Pointer to an int variable to store the "x" coordinate of the center
		yCoord - Pointer to an int variable to store the "y" coordinate of the center
		orientWin - MACRO representation of window orientation if ever the center isn't
			the perfect center.  Function defaults to HM_UP_LEFT if orientWin is
			invalid.
	OUTPUT
		On success, true;
		On failure, false;
 */
bool determine_center(int width, int height, int* xCoord, int* yCoord, int orientWin)
{
	// LOCAL VARIABLES
	bool retVal = true;  // If anything fails, make this false
	int realOrientWin = 0;  // Translate orientWin into this variable
	int realWidth = 0;  // Adjusted width if center isn't exactly center
	int realHeight = 0;  // Adjusted height if center isn't exactly center

	// INPUT VALIDATION
	if (width < 3)
	{
		HARKLE_ERROR(Harklemath, determine_center, Invalid width);
		retVal = false;
	}
	else if (height < 3)
	{
		HARKLE_ERROR(Harklemath, determine_center, Invalid height);
		retVal = false;
	}
	else if (!xCoord || !yCoord)
	{
		HARKLE_ERROR(Harklemath, determine_center, NULL pointer);
		retVal = false;
	}
	else
	{
		*xCoord = 0;
		*yCoord = 0;

		if (orientWin == HM_UP_LEFT  || \
			orientWin == HM_UP_RIGHT || \
			orientWin == HM_LOW_LEFT || \
			orientWin == HM_LOW_RIGHT)
		{
			realOrientWin = orientWin;
		}
		else
		{
			realOrientWin = HM_UP_LEFT;
		}
	}

	// CALCUALTE CENTER
	if (true == retVal)
	{
		// Width
		if (width & 1)  // Width is odd
		{
			realWidth = width;
		}
		else  // Width is even
		{
			if (realOrientWin == HM_UP_RIGHT || \
				realOrientWin == HM_LOW_RIGHT)
			{
				realWidth = width + 1;
			}
			else
			{
				realWidth = width - 1;
			}
		}

		*xCoord = ((realWidth - 1) / 2) + 1;

		// Height
		if (height & 1)  // Height is odd
		{
			realHeight = height;
		}
		else  // Height is even
		{
			if (realOrientWin == HM_LOW_LEFT || \
				realOrientWin == HM_LOW_RIGHT)
			{
				realHeight = height + 1;
			}
			else
			{
				realHeight = height - 1;
			}
		}

		*yCoord = ((realHeight - 1) / 2) + 1;
	}


	// DONE
	return retVal;
}


hcCartCoord_ptr build_geometric_list(double* relEllipseCoords, int numPnts, int centX, int centY)
{
	// LOCAL VARIABLES
	hcCartCoord_ptr retVal = NULL;
	hcCartCoord_ptr newNode = NULL;  // Newly created node prior to linking
	hcCartCoord_ptr tmpNode = NULL;  // Return value from add_cartCoord_node()
	int tmpX = 0;  // Temporary x value translated from the double array
	int tmpY = 0;  // Temporary y value translated from the double array
	bool success = true;  // Set this to false if anything fails
	int i = 0;  // Iterating variable

	// INPUT VALIDATION
	if (NULL == relEllipseCoords)
	{
		HARKLE_ERROR(Harklemath, build_geometric_list, NULL pointer);
		success = false;
	}
	else if (numPnts < 2 || numPnts % 2)
	{
		HARKLE_ERROR(Harklemath, build_geometric_list, Invalid numPnts);
		success = false;
	}
	else if (centX < 0)
	{
		HARKLE_ERROR(Harklemath, build_geometric_list, Invalid centX);
		success = false;
	}
	else if (centY < 0)
	{
		HARKLE_ERROR(Harklemath, build_geometric_list, Invalid centY);
		success = false;
	}

	// BUILD LINKED LIST
	if (true == success)
	{
		for (i = 1; i < numPnts; i += 2)
		{
			// Round the doubles to ints
			printf("\ncentX == %d\tcentY == %d\n", centX, centY);  // DEBUGGING
			printf("\nrelEllipseCoords[%d - 1] == %.15f\trelEllipseCoords[%d] == %.15f\n", i, relEllipseCoords[i - 1], i, relEllipseCoords[i]);  // DEBUGGING
			tmpX = round_a_dble(relEllipseCoords[i - 1], HM_UP);
			tmpY = round_a_dble(relEllipseCoords[i], HM_UP);
			printf("\ntmpX == %d\ttmpY == %d\n", tmpX, tmpY);  // DEBUGGING

			// Build a node
			if (NULL == retVal)
			{
				// Build the head node
				retVal = build_new_cartCoord_struct(tmpX, \
						                            tmpY, '*', 0);

				if (NULL == retVal)
				{
					HARKLE_ERROR(Harklemath, build_geometric_list, build_new_cartCoord_struct failed);
					success = false;
					break;
				}
			}
			else
			{
				// Build a child node		
				newNode = build_new_cartCoord_struct(tmpX, tmpY, '*', 0);

				if (NULL == newNode)
				{
					HARKLE_ERROR(Harklemath, build_geometric_list, build_new_cartCoord_struct failed);
					success = false;
					break;
				}
				else
				{
					// Add the child node to the existing linked list
					tmpNode = add_cartCoord_node(retVal, newNode, 0);

					if (NULL == tmpNode)
					{
						HARKLE_ERROR(Harklemath, build_geometric_list, add_cartCoord_node failed);
						success = false;
						break;
					}
					else if (tmpNode != retVal)
					{
						retVal = tmpNode;
					}

					newNode = NULL;  // Reset the temp variable as an indication of successful linking
				}				
			}
		}
	}

	// CLEAN UP
	if (false == success)
	{
		// newNode
		if (newNode)
		{
			if (false == free_cartCoord_struct(&newNode))
			{
				HARKLE_ERROR(Harklemath, build_geometric_list, free_cartCoord_struct failed);
			}
		}
		
		// retVal
		if (retVal)
		{
			if (false == free_cardCoord_linked_list(&retVal))
			{
				HARKLE_ERROR(Harklemath, build_geometric_list, free_cardCoord_linked_list failed);
			}
		}
	}

	// DONE
	return retVal;
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


/*
	PURPOSE - Translate plot points that are relative to the center of a windows
		into plot points that are absolute with relation to the upper left corner
		of a window
	INPUT
		relX - x point relative to the specified center coordinate
		relY - y point relative to the specified center coordinate
		cntX - x point of the center coordinate
		cntY - y point of the center coordinate
		absX - [OUT] translation of relX into an absolute reference point
			starting at (0, 0) if that represents the upper left hand corner
			of the window
		absY - [OUT] translation of relY into an absolute reference point
			starting at (0, 0) if that represents the upper left hand corner
			of the window
	OUTPUT
		On success, true (and absX/absY have the real results)
		On failure, false
	NOTES
		This function, were it not a local helper function, might best be served
			inside Harklecurse as it has been written to help translate plot
			points into a format easily digestible by ncurses window plotting
			functions.  This is because ncurses windows treat "home" (see: (0, 0))
			as the upper left hand corner of the window.
		As of now, this function is exclusively called by build_geometric_list()
 */
bool translate_plot_points(int relX, int relY, int cntX, int cntY, int* absX, int* absY)
{
	// LOCAL VARIABLES
	bool retVal = true;  // If anything fails, set this to false
	
	// INPUT VALIDATION
	if (!absX || !absY)
	{
		HARKLE_ERROR(Harklemath, translate_plot_points, NULL pointer);
		retVal = false;
	}
	else if (cntX < 1 || cntY < 1)
	{
		HARKLE_ERROR(Harklemath, translate_plot_points, Invalid center coordinates);
		retVal = false;
	}
	else if ((cntX + relX) < 0 || (cntY - relY) < 0)
	{
		HARKLE_ERROR(Harklemath, translate_plot_points, Invalid relative coordinates);
		retVal = false;
	}
	else
	{
		*absX = 0;
		*absY = 0;
	}
	
	// TRANSLATE COORDINATES
	if (true == retVal)
	{
		*absX = cntX + relX;
		*absY = cntY - relY;
	}
	
	// DONE
	return retVal;
}
	

//////////////////////////////////////////////////////////////////////////////
//////////////////////// LOCAL HELPER FUNCTIONS STOP /////////////////////////
//////////////////////////////////////////////////////////////////////////////
