/*
 * defines.h source file for extace
 * 
 * /GDK/GNOME sound (esd) system output display program
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

#ifndef _DEFINES_H_
#define _DEFINES_H_ 1

/* Definitions i.e. macros/flags, other stuff to make things easier for me.. */
/* default colors */
#define RASTER_COLOR_RED        80
#define RASTER_COLOR_GREEN      20
#define RASTER_COLOR_BLUE       10  

#define TRACE_COLOR_RED         30
#define TRACE_COLOR_GREEN       240
#define TRACE_COLOR_BLUE        60


/* Audio Data Specific definitions */
#define FRAMES		88200  // Audio ring size in audio "frames"
/* 1 frame is a left and right channel of audio, Signed 16 bit LE.
 * thus 1 frame is 32 bits in total size, 16 bits for left and 16 bits for right
 * I'm making the assumption that we are using standard STEREO (2 channel)
 * audio for our source. (that may change in the distant future)
 * A frame for a 4 channel input card would NOT be the same for a stereo input.
 */
#define BUFFER 		FRAMES*2 // Audio ring size (1 seconds at 44100/stereo)
/* Actual buffer is twice that value in size, because its a buffer of "shorts" 
 * 1 "short" = 16 bits, thus 2 bytes, thus total buffer size is 176000 bytes.
 */
#define RATE   		44100	// sample rate (samples/sec) 
#define MAXBANDS        256	// maximum number of low freq display bands 


#endif
