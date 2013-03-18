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
	RECTANGULAR,
	NUM_WINDOW_FUNCTIONS
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
	HIGH_LIMIT,	
	SCOPE_ZOOM,
	NUM_SLIDERS
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
	PAUSE_DISP,
	NUM_TOGGLEBUTTONS
}ToggleButton;
		
typedef enum
{
	SYNC_LEFT,
	SYNC_RIGHT,	
	SYNC_INDEP,
	NUM_SCOPE_SYNCSOURCES
}ScopeSyncSource;

typedef enum
{
	LEFT,
	RIGHT,
	LEFT_PLUS_RIGHT,
	LEFT_MINUS_RIGHT,
	NUM_FFT_PACKING_METHODS
}FftDataPacking;

typedef enum
{
	LOG,
	LINEAR,
	NUM_AXIS_TYPES
}AxisType;

/* We are certainly ambitious :-)  */

typedef enum
{
	PULSEAUDIO,
	ESD,
	NUM_DATASOURCES
}DataSource;

typedef enum
{	
	FULL,
	HALF,
	QUARTER,
	EIGHTH,
	NUM_WINDOW_WIDTHS
}WindowWidth;

typedef enum
{
	S_512,
	S_1024,
	S_2048,
	S_4096,
	S_8192,
	S_16384,
	S_32768,
	NUM_FFT_SIZES
}FftSize;
	
typedef enum
{
	LAND_3D,
	WIRE_3D,
	FILL_3D,
	LINE_EQ,
	EQ_2D,
	SCOPE,
	SPIKE_3D,
	HORIZ_SPECGRAM,
	VERT_SPECGRAM,
	VERT_SPECGRAM2,
	STARS,
	NUM_DISPLAY_MODES
}DisplayMode;

typedef enum
{
	DOT_SCOPE,
	LINE_SCOPE,
	GRAD_SCOPE,
	NUM_SCOPE_MODES
}ScopeMode;

typedef enum
{
	MAIN_DISPLAY,
	BUFFER_AREA,
	DIR_AREA,
	NUM_DRAWABLE_AREAS
}DrawableArea;

typedef enum
{	
	CHANGE_SPEC_START,
	CHANGE_X_START,
	CHANGE_X_END,
	NUM_EVENT_OPERATIONS
}EventOperation;

typedef enum
{	
	ON_THE_LINE,
	OFF_THE_LINE,
	NUM_EVENT_STATUSES
}EventStatus;

typedef enum
{
	CLOSE,
	SAVE,
	LOAD,
	SET_COLOR,
	NUM_COLOR_OPERATIONS
}ColorOperation;

#endif
