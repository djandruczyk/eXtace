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

typedef enum
{
	BANDS,
	SENSITIVITY,
	BAR_DECAY,
	PEAK_DECAY,
	PEAK_HOLD,
	LAG,
	NOISE_FLOOR,
	TAPE_SCROLL,
	REFRESH_RATE,
	LOW_LIMIT,
	HIGH_LIMIT	
}Slider;

typedef enum
{
	OPTIONS,
	LEADING_EDGE,
	USE_BAR_DECAY,
	USE_PEAK_DECAY,
	OUTLINED,
	STABILIZED,
	GRATICULE,
	LAND_PERS_TILT,
	SPIKE_PERS_TILT,
	LANDFLIP,
	SPIKEFLIP,
	PAUSE_DISP
}ToggleButton;
		
typedef enum
{
	SYNC_LEFT,
	SYNC_RIGHT,	
	SYNC_INDEP
}ScopeSyncSource;

typedef enum
{
	LEFT,
	RIGHT,
	LEFT_PLUS_RIGHT,
	LEFT_MINUS_RIGHT
}FftDataPacking;

typedef enum
{
	LOG,
	LINEAR
}AxisType;

/* We are certainly ambitious :-)  */

typedef enum
{
	ESD,
        OSS,
	ALSA,
	COMEDI,
	ARTS,
	GSTREAMER,
	JACK
}DataSource;

typedef enum
{	
	FULL,
	HALF,
	QUARTER,
	EIGHTH
}WindowWidth;

typedef enum
{
	S_512,
	S_1024,
	S_2048,
	S_4096,
	S_8192,
	S_16384,
	S_32768
}FftSize;
	
typedef enum
{
	LAND_3D,
	WIRE_3D,
	FILL_3D,
	EQ_2D,
	SCOPE,
	SPIKE_3D,
	HORIZ_SPECGRAM,
	VERT_SPECGRAM,
	STARS
}DisplayMode;

typedef enum
{
	DOT_SCOPE,
	LINE_SCOPE,
	GRAD_SCOPE
}ScopeMode;

typedef enum
{
	MAIN_DISPLAY,
	BUFFER_AREA,
	DIR_AREA
}DrawableArea;

typedef enum
{	
	CHANGE_SPEC_START,
	CHANGE_X_START,
	CHANGE_X_END
}EventOperation;

typedef enum
{	
	ON_THE_LINE,
	OFF_THE_LINE
}EventStatus;

typedef enum
{
	CLOSE,
	SAVE,
	LOAD,
	SET_COLOR
}ColorOperation;

#endif
