#include "Rando.h"				// rando_me()
#include <stdio.h>				// fprintf()

#define LINE_LEN 200
#define MAX_AMP LINE_LEN
#define LOWER_CHAR ' '
#define UPPER_CHAR '~'
#define CHANGE_CHAR 30			// X out of 100 chance to change the character
#define CHANGE_HGHT 40			// X out of 100 chance to change the "amplitutde"

int main(void)
{
	// LOCAL VARIABLES
	int amplitude = MAX_AMP / 2;  // Number of characters to print on a line
	char currChar = ((UPPER_CHAR - LOWER_CHAR) / 2) + LOWER_CHAR;  // Current character to print
	int i = 0;  // Iterating variable

	// PRINT
	fprintf(stdout, "\n");

	while (1)
	{
		// 1.1. Print the character
		for (i = 0; i < amplitude; i++)
		{
			fprintf(stdout, "%c", currChar);
		}
		fprintf(stdout, "\n");

		// 1.2. Change the character?
		if (rando_me(1, 100) <= CHANGE_CHAR)
		{
			// Yes.  Increment or decrement?
			if (rando_me(1, 100) <= 50)
			{
				// Decrement.  Have we hit the bottom?
				if (LOWER_CHAR >= currChar)
				{
					// Yes.  Increment instead.
					currChar++;
				}
				else
				{
					// No.  Decrement.
					currChar--;
				}
			}
			else
			{
				// Increment.  Have we hit the top?
				if (UPPER_CHAR <= currChar)
				{
					// Yes.  Decrement instead.
					currChar--;
				}
				else
				{
					// No.  Increment.
					currChar++;
				}
			}
		}

		// 1.3. Change the amplitude?
		if (rando_me(1, 100) <= CHANGE_HGHT)
		{
			// Yes.  Increment or decrement?
			if (rando_me(1, 100) <= 50)
			{
				// Decrement.  Have we hit the bottom?
				if (0 >= amplitude)
				{
					// Yes.  Increment instead.
					amplitude++;
				}
				else
				{
					// No.  Decrement.
					amplitude--;
				}
			}
			else
			{
				// Increment.  Have we hit the top?
				if (MAX_AMP <= amplitude)
				{
					// Yes.  Decrement instead.
					amplitude--;
				}
				else
				{
					// No.  Increment.
					amplitude++;
				}
			}
		}
	}

	// DONE
	return 0;
}
