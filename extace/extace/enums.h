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

typedef enum
{
    HAMMING	= 1 << 1,
    HANNING	= 1 << 2,
    BLACKMAN	= 1 << 3,
    BLACKMAN_HARRIS	= 1 << 4,
    GAUSSIAN	= 1 << 5,
    WELCH	= 1 << 6,
    PARZEN	= 1 << 7,
    RECTANGULAR	= 1 << 8
}WindowFunction;

