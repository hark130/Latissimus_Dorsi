#include "Harklerror.h"	// HARKLE_ERROR
#include "Memoroad.h"	// copy_a_string
#include <signal.h>		// signal MACROs
#include <stdbool.h>	// bool, true, false
#include <string.h>		// strncpy

#define SR_BUFF_SIZE 128

#ifndef SR_WRAP_IT
#define SR_WRAP_IT(str) #str
#endif  // SR_WRAP_IT

#ifndef SR_WRAP
#define SR_WRAP(str) SR_WRAP_IT(str)
#endif  // SR_WRAP

#define SR_SIGSTR_01 SR_WRAP_IT(SIGHUP)
#define SR_SIGSTR_02 SR_WRAP_IT(SIGINT)
#define SR_SIGSTR_03 SR_WRAP_IT(SIGQUIT)
#define SR_SIGSTR_04 SR_WRAP_IT(SIGILL)
#define SR_SIGSTR_05 SR_WRAP_IT(SIGTRAP)
#define SR_SIGSTR_06 SR_WRAP_IT(SIGABRT)
#define SR_SIGSTR_07 SR_WRAP_IT(SIGBUS)
#define SR_SIGSTR_08 SR_WRAP_IT(SIGFPE)
#define SR_SIGSTR_09 SR_WRAP_IT(SIGKILL)
#define SR_SIGSTR_10 SR_WRAP_IT(SIGUSR1)
#define SR_SIGSTR_11 SR_WRAP_IT(SIGSEGV)
#define SR_SIGSTR_12 SR_WRAP_IT(SIGUSR2)
#define SR_SIGSTR_13 SR_WRAP_IT(SIGPIPE)
#define SR_SIGSTR_14 SR_WRAP_IT(SIGALRM)
#define SR_SIGSTR_15 SR_WRAP_IT(SIGTERM)
#define SR_SIGSTR_16 SR_WRAP_IT(SIGSTKFLT)
#define SR_SIGSTR_17 SR_WRAP_IT(SIGCHLD)
#define SR_SIGSTR_18 SR_WRAP_IT(SIGCONT)
#define SR_SIGSTR_19 SR_WRAP_IT(SIGSTOP)
#define SR_SIGSTR_20 SR_WRAP_IT(SIGTSTP)
#define SR_SIGSTR_21 SR_WRAP_IT(SIGTTIN)
#define SR_SIGSTR_22 SR_WRAP_IT(SIGTTOU)
#define SR_SIGSTR_23 SR_WRAP_IT(SIGURG)
#define SR_SIGSTR_24 SR_WRAP_IT(SIGXCPU)
#define SR_SIGSTR_25 SR_WRAP_IT(SIGXFSZ)
#define SR_SIGSTR_26 SR_WRAP_IT(SIGVTALRM)
#define SR_SIGSTR_27 SR_WRAP_IT(SIGPROF)
#define SR_SIGSTR_28 SR_WRAP_IT(SIGWINCH)
#define SR_SIGSTR_29 SR_WRAP_IT(SIGIO)
#define SR_SIGSTR_30 SR_WRAP_IT(SIGPWR)
#define SR_SIGSTR_31 SR_WRAP_IT(SIGSYS)
// 32
// 33
#define SR_SIGSTR_34 SR_WRAP_IT(SIGRTMIN)
#define SR_SIGSTR_35 SR_WRAP_IT(SIGRTMAX)


char* str_signaleroad(int sigRoad)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	bool success = true;
	size_t retValLen = 0;  // Length of the return value
	char tempBuf[SR_BUFF_SIZE + 1] = { 0 };  // Temporarily hold the signal name
	char* temp_ptr = NULL;  // Return value from string.h function calls
	int sigRtMin = SIGRTMIN;  // Resolving the macro
	int sigRtMax = SIGRTMAX;  // Resolving the macro

	// PARSE SIGNALS
	switch (sigRoad)
	{
		case SIGHUP:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_01, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGINT:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_02, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGQUIT:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_03, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGILL:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_04, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGTRAP:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_05, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGABRT:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_06, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGBUS:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_07, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGFPE:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_08, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGKILL:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_09, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGUSR1:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_10, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGSEGV:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_11, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGUSR2:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_12, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGPIPE:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_13, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGALRM:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_14, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGTERM:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_15, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGSTKFLT:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_16, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGCHLD:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_17, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGCONT:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_18, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGSTOP:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_19, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGTSTP:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_20, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGTTIN:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_21, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGTTOU:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_22, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGURG:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_23, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGXCPU:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_24, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGXFSZ:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_25, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGVTALRM:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_26, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGPROF:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_27, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGWINCH:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_28, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGIO:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_29, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGPWR:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_30, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		case SIGSYS:
			temp_ptr = strncpy(tempBuf, SR_SIGSTR_31, SR_BUFF_SIZE);
			if (temp_ptr != tempBuf)
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
				success = false;
			}
			temp_ptr = NULL;
			break;
		//////////////////////////////////////////////////////////////////////
		////////////////////////// FIX THESE LATER ///////////////////////////
		//////////////////////////////////////////////////////////////////////
		// case SIGRTMIN:
		// 	temp_ptr = strncpy(tempBuf, SR_SIGSTR_34, SR_BUFF_SIZE);
		// 	if (temp_ptr != tempBuf)
		// 	{
		// 		HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
		// 		success = false;
		// 	}
		// 	temp_ptr = NULL;
		// 	break;
		// case SIGRTMAX:
		// 	temp_ptr = strncpy(tempBuf, SR_SIGSTR_35, SR_BUFF_SIZE);
		// 	if (temp_ptr != tempBuf)
		// 	{
		// 		HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
		// 		success = false;
		// 	}
		// 	temp_ptr = NULL;
		// 	break;
		default:
			HARKLE_ERROR(Signaleroad, str_signaleroad, Signal Number Unknown);
			success = false;
			break;
	}

	// fprintf(stdout, "tempBuf is currently == %s\n", tempBuf);  // DEBUGGING

	// ALLOCATE A BUFFER
	if (success == true)
	{
		// 1. Detemine necessary size
		retValLen += strlen(tempBuf);	// SIGBLAHBLAHBLAH
		retValLen += 2;  				// ": "
		temp_ptr = strsignal(sigRoad);	// <string>
		retValLen += strlen(temp_ptr);

		// 2. Allocate the buffer
		retVal = get_me_a_buffer(retValLen);
		if (!retVal)
		{
			HARKLE_ERROR(Signaleroad, str_signaleroad, get_me_a_buffer failed);
			success = false;
		}
		else
		{
			// 3. Copy the SIG name in
			if (retVal != strcpy(retVal, tempBuf))
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, strcpy failed);
				success = false;
			}
			else
			{
				retValLen = strlen(retVal);

				// 4. Add pretty formatting
				if (retValLen + 2 <= SR_BUFF_SIZE)
				{
					(*(retVal + retValLen)) = ':';
					retValLen++;
					(*(retVal + retValLen)) = ' ';
					retValLen++;
				}

				// 5. Copy the description in
				if (retVal != strcpy(retVal, temp_ptr))
				// if (retVal != strncpy(retVal, temp_ptr, SR_BUFF_SIZE - retValLen))
				{
					HARKLE_ERROR(Signaleroad, str_signaleroad, strncpy failed);
					success = false;
				}
			}
		}
	}

	// CLEAN UP
	temp_ptr = NULL;
	if (success == false)
	{
		if (retVal)
		{
			if (false == release_a_string(&retVal))
			{
				HARKLE_ERROR(Signaleroad, str_signaleroad, release_a_string failed);
			}
		}
	}

	// DONE
	return retVal;
}




/*
SIGHUP
SIGINT	 
SIGQUIT	 
SIGILL	 
SIGTRAP
SIGABRT
SIGBUS	 
SIGFPE	 
SIGKILL	
SIGUSR1
SIGSEGV
SIGUSR2	
SIGPIPE	
SIGALRM
SIGTERM
SIGSTKFLT
SIGCHLD	
SIGCONT	
SIGSTOP	
SIGTSTP
SIGTTIN
SIGTTOU	
SIGURG	
SIGXCPU	
SIGXFSZ
SIGVTALRM
SIGPROF	
SIGWINCH	
SIGIO	
SIGPWR
SIGSYS
SIGRTMIN
SIGRTMIN
SIGRTMAX
*/
