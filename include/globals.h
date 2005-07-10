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
#ifndef _GLOBALS_H_
#define _GLOBALS_H_ 1

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gdk_imlib.h>
#include <sys/time.h>
#include <convolve.h>
#include <config.h>
#include <defines.h>
#ifdef USING_FFTW2
#include <rfftw.h>
#elif USING_FFTW3
#include <fftw3.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
#error PThreads are required to compile eXtace
#endif


GdkPixmap	*dir_pixmap;	/* directional window pixmap pointer*/
GdkPixmap	*grad_pixmap;	/* color gradient window pixmap pointer*/
GdkPixmap	*main_pixmap;	/* MAIN window backing pixmap pointer */
GdkPixmap	*grad[256];	/* Gradient pointers?? forgot bout this too */
GtkWidget	*main_display;	/* Main Display pointer */
GtkWidget	*optionsbut;	/* Options button pointer */
GtkWidget	*dir_win;	/* Direction window pointer */
GtkWidget	*dir_area;	/* directional window area */
GtkWidget	*dir_win_ptr;	/* Pointer */
GtkWidget	*grad_win_ptr;	/* Pointer */
GtkWidget	*options_win_ptr;/* pointer specific to that button */
GdkGC		*gc;		/* Main graphics context */
GdkGC		*graticule_gc;	/* Graphics context for graticule in scope */
GdkGC		*arc_gc;	/* Graphics context for Arc in dircontrol */
GdkGC		*trace_gc;	/* Graphics context for Trace in scope */
GdkColor	temp_color;	/* Temporary var for testing */
GdkColor	*start,*pt2,*pt3,*pt4,*end; /* Colors for gradient board */
gshort		*audio_left;	/* left channel (scope??) */
gshort		*audio_last_l;	/* last one of above, left channel (scope??) */
gshort		*audio_last_r;	/* last one of above, right channel (scope??) */
gshort		*audio_right;	/* right channel (scope??) */
gdouble		*raw_fft_in;	/* RAW input to fft routine */
gdouble		*raw_fft_out;	/* RAW output of fft routine */
gdouble		*norm_fft;	/* normalized fft data */
gdouble		*datawindow;	/* pointer to window function array */
double		levels[MAXBANDS];/* Levels for 3D and 3D Eq/spectral */
double		plevels[MAXBANDS];/* Levels for 3D and 3D Eq/spectral */
double		trailers[MAXBANDS];/* Levels for "trailers" in 2D EQ */
double		ptrailers[MAXBANDS];/* Levels for "trailers" in 2D EQ */
gint		*pip_arr;	/* array of pip values for screen */
gint		*disp_val;	/* Display level for screen */
gint		nsamp;		/* number of samples */
gint		bands;		/* to start with, should be configurable */
gint 		decimation_factor; /* for sub hertz resolution */
gint 		lag;		/* delay between getting audio and displaying */
gint   		width;		/* Main window width */
gint   		height;		/* Main window height */
gint  		colortab[MAXBANDS][MAXBANDS];/* ugly, statically allocated to up to MAXBANDS bands */
gint		mode;		/* What display mode are we in */
gint		bar_decay_speed;/* ONLY works with bar_decay "on" (TRUE) */
gint		peak_decay_speed;/* ONLY works with peak_decay "on" (TRUE) */
gint		peak_hold_time;/* ONLY works with peak_decay "on" (TRUE) */
gint		border;		/* border around displays */
gint		x_offset;	/* 3D X axis offset for centering */
gint		y_offset;	/* 3D X axis offset for centering */
gint		trail_counter[MAXBANDS];/* Peak/Hold counter for 2D EQ */
gint		window_func;	/* Which window function r we using? */
gint		axis_type;	/* Linear or LOG (NOT SPIKE or SPECT modes) */
gint		dir_x_origin;	/* Coords of direction window */
gint		dir_y_origin;	/* Coords of direction window */
gint		tape_scroll;
gint		display_markers;/* Are markers showing? */
gint		vert_spec_start;/* where vert spectram starts(abs) */
gint		horiz_spec_start;/* where horiz spectram starts(abs) */
gint		color_loc;	/* pixel location in color gradient for color mapper */
gint		cr[MAXBANDS];
gint		cg[MAXBANDS];
gint		cb[MAXBANDS];
gint		scope_sub_mode;	/* Dot, line or gradient sub mode  */
gint		sub_mode_3D;	/* sub mode for 3D modes */
gint		scope_begin_l;	/* Begining point in buffer for scope */
gint		scope_begin_r;	/* Begining point in buffer for scope */
gfloat		scale;
gfloat		multiplier;	/* Level multiplier, for fft routines */
gfloat		freqmark[MAXBANDS];/* Frequency markers for 2D EQ */
gfloat		freq_at_pointer;/* Frequency at mouse pointer */
gint		last_buf_l[CONVOLVE_SMALL];/* Convolve buffer */
gint		last_buf_r[CONVOLVE_SMALL];/* Convolve buffer */
gshort		cur_buf_l[CONVOLVE_BIG];/* Convolve buffer */
gshort		cur_buf_r[CONVOLVE_BIG];/* Convolve buffer */
		
convolve_state	*l_state;	/* Pointer to make convolve happy */
convolve_state	*r_state;	/* Pointer to make convolve happy */
gint		r_count;
glong		frame_cnt;	/* Frame count */
struct		timeval cur_time, last_time;
struct		timeval input_arrival, input_arrival_last;
struct		timeval draw_win_time_last, draw_win_time;
struct		Color_map {
    gint steps;			/* Number of steps in the map */
    gint *triplets;		/* buffer of RGB triples for each step */
    gfloat *locations;		/* buffer of location point in the map */
    gchar *filename;	/* Currently used colormap */
}Color_map;

#ifdef USING_FFTW2
rfftw_plan	plan;		/* fft plan for fftw library */
#elif USING_FFTW3
fftw_plan	plan;		/* FFTW plan for fftw 3.x library */
#endif
gboolean	landtilt;	/* Landform 3D perspective tilt flag */
gboolean	spiketilt;	/* Spike 3D perspective tilt flag */
gboolean	landflip;	/* Invert Y axis on 3D modes */
gboolean	spikeflip;	/* Invert Y axis on 3D modes */
gboolean	outlined;	/* Outlined 3D landform flag */
gboolean	stabilized;	/* scope trace stabilizer option */
gboolean	peak_decay;	/* peak_decay and decay are tied together */
gboolean	bar_decay;	/* bar_decay and decay are tied together */
gboolean	show_graticule;	/* show scope graticule */
gboolean	show_leader;	/* show leading edge on 3D landscape fft */
gboolean	paused;		/* Display Paused Flag */
gboolean	recalc_scale;	/* its NOT fixed YET. (done dynamically) */
gboolean	recalc_markers;	/* flag for marker recalculation routine */
gboolean	clear_display;	/* Flag to signal a clear of the display */
gboolean	dir_win_present;/* Flag */
gboolean	grad_win_present;/* Flag */

gint		fft_signal_source;/* Left right or both channels */
gint		refresh_rate;	/* display refresh rate*/
gfloat 		scope_zoom;	/* Scope zoom factor */
guint 		display_id;	/* display ID for gtk_timeout_* */
gint		convolve_factor;	
gint 		win_width;	/* window function options */	
gint		active_drawing_area;
gfloat 		noise_floor;
gfloat		low_freq;
gfloat		high_freq;

#endif
