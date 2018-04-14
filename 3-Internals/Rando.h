#ifndef __RANDO_H__
#define __RANDO_H__


/*
	PURPOSE - Seed the psuedo-randomizer once with the clock value.
	INPUT - None
	OUTPUT - None
	NOTES
		This function stores a static int used to avoid multiple
			calls to srand()
 */
void rando_init(void);


/*
	PURPOSE - Psueo-randomize a number from "low" to "high", inclusive
	INPUT
		low - Lower bound of the random number
		high - Upper bound of the random number
	OUTPUT
		On success, a psuedo-random value that is both greater than or
			equal to "low" and less than or equal to "high"
		On failure, -1
	NOTES
		This function will call rando_init() to initially seed the 
			generator just once
 */
int rando_me(int low, int high);


#endif  // __RANDO_H__