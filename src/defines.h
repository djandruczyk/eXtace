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
/* Audio Data Specific definitions */

 
/* General Overall defintions */
#define SENSITIVITY	0x01
#define LAG		0x02
#define LOG		0x03
#define LINEAR		0x04
#define ESD		0x05
#define BACK_PIXMAP	0x06
#define BUFFER_AREA	0x07
#define DIR_AREA	0x08
#define MAIN_DISPLAY	0x09
#define NOISE_FLOOR	0x0a
#define PAUSE_DISP	0x0b


/* Mode Specific definitions */

/* 2D Grphical Equalizer */
#define EQ_2D		0x20
/* 2D Grphical Equalizer */

/* 3D High Res FFT */
#define SPIKE_3D        0x30
#define SPIKEFLIP	0x31
#define SPIKE_PERS_TILT	0x32
/* 3D High Res FFT */

/* About Animation */
#define STARS           0x40
/* About Animation */

/* Options Panel */
#define OPTIONS		0x45

/* Scrolling Spectragrams */
#define VERT_SPECGRAM	0x50
#define HORIZ_SPECGRAM	0x51
#define ON_THE_LINE	0x52
#define OFF_THE_LINE	0x53
#define TAPE_SCROLL	0x54
#define CHANGE_SPEC_START 0x55
#define VERTICAL	0x56
#define HORIZONTAL	0x57
/* Scrolling Spectragrams */

/* 3D Low Res FFT (flying Landform) */
#define LAND_3D		0x60
#define FILL_3D		0x61
#define WIRE_3D		0x62
#define CHANGE_X_START	0x63
#define CHANGE_X_END	0x64
#define LAND_PERS_TILT	0x65
#define LEADING_EDGE	0x66
#define LANDFLIP	0x67
#define OUTLINED	0x68
/* 3D Low Res FFT (flying Landform) */

/* Oscilloscope */
#define SCOPE		0x70
#define DOT_SCOPE	0x71
#define LINE_SCOPE	0x72
#define GRAD_SCOPE	0x73
#define SYNC_LEFT	0x74
#define SYNC_RIGHT	0x75
#define SYNC_INDEP	0x76
#define GRATICULE	0x77
#define STABLE		0x78
/* Oscilloscope */

/* General FFT Options */
#define COMPOSITE	0x80
#define DIFFERENCE	0x81
#define LEFT		0x82
#define RIGHT		0x83
#define FULL		0x84
#define HALF		0x85
#define QUARTER		0x86
#define EIGHTH		0x87
/* General FFT Options */

/* Color Controls */
#define CLOSE		0xa0
#define SAVE		0xa1
#define LOAD		0xa2
#define SET_COLOR	0xa3
/* Color Controls */

/* Low Res Specific defintions */
#define BANDS		0xb0
#define BRIGHTNESS      0xb1
#define PEAK_HOLD	0xb2
#define PEAK_DECAY	0xb3
#define BAR_DECAY	0xb4
/* Low Res Specific defintions */


/* Misc Options */
#define REFRESH_RATE    0xc0
/* Misc Options */

#endif
