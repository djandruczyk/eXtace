/*
 * GDK/GNOME sound (esd) system output display program
 * 
 * Copyright (C) 1999 by Dave J. Andruczyk 
 * 
 * Based on the original extace written by The Rasterman and Michael Fulbright
 *  
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#ifndef _ENUMS_H_
#define _ENUMS_H_ 1

typedef enum
{
	HAMMING,
	HANNING,
	BLACKMAN,
	BLACKMAN_HARRIS,
	GAUSSIAN,
	WELCH,
	PARZEN,
	RECTANGULAR
}WindowFunction;

#endif
