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
#ifdef HAVE_LIBRFFTW
#include <rfftw.h>
#endif
#ifdef HAVE_LIBDRFFTW
#include <drfftw.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
#error PThreads are required to compile eXtace
#endif


gint		nsamp;		/* number of samples */
gint		bands;		/* to start with, should be configurable */
GdkPixmap	*dir_pixmap;	/* directional window pixmap pointer*/
GdkPixmap	*grad_pixmap;	/* color gradient window pixmap pointer*/
GdkPixmap	*main_pixmap;	/* MAIN window backing pixmap pointer */
GdkPixmap	*buffer_pixmap;	/* Buffer window backing pixmap pointer */
GdkPixmap	*grad[256];	/* Gradient pointers?? forgot bout this too */
GtkWidget	*main_display;	/* Main Display pointer */
GtkWidget	*optionsbut;	/* Options button pointer */
GtkWidget	*dir_win;	/* Direction window pointer */
GtkWidget	*dir_area;	/* directional window area */
GtkWidget	*buffer_area;	/* Buffer latency display window area */
GdkGC		*gc;		/* Main graphics context */
GdkGC		*graticule_gc;	/* Graphics context for graticule in scope */
GdkGC		*arc_gc;	/* Graphics context for Arc in dircontrol */
GdkGC		*trace_gc;	/* Graphics context for Trace in scope */
GdkGC		*latency_monitor_gc;/* Graphics context for Arc in dircontrol */
gint		ring_pos;	/* place safe to write in ringbuffer */
gint		ring_end;	/* end of ringbuffer in ELEMENTS */
gint		elements_to_get;	/* amount to read in ELEMENTS */
gshort		*audio_ring;	/* Array raw  audio data from input source */
gshort		*audio_left;	/* left channel (scope??) */
gshort		*audio_last_l;	/* last one of above, left channel (scope??) */
gshort		*audio_last_r;	/* last one of above, right channel (scope??) */
gshort		*audio_right;	/* right channel (scope??) */
gdouble		*raw_fft_in;	/* RAW input to fft routine */
gdouble		*raw_fft_out;	/* RAW output of fft routine */
gdouble		*norm_fft;	/* normalized fft data */
gdouble		*datawindow;	/* pointer to window function array */
gint		*pip_arr;	/* array of pip values for screen */
gint		*disp_val;	/* Display level for screen */
gint 		decimation_factor; /* for sub hertz resolution */
gint 		lag;		/* delay between getting audio and displaying */
gint 		fft_lag;	/* delay between getting audio and displaying */
gint   		width;		/* Main window width */
gint   		height;		/* Main window height */
gint  		colortab[MAXBANDS][MAXBANDS];/* ugly, statically allocated to up to MAXBANDS bands */
gint		mode;		/* What display mode are we in */


gint		peak_decay;	/* peak_decay and decay are tied together */
gint		stabilized;	/* scope trace stabilizer option */
gint		bar_decay;	/* bar_decay and decay are tied together */
gint		bar_decay_speed;/* decay_speed ONLY works with bar_decay "on" (1) */
gint		peak_decay_speed;/* decay_val ONLY works with peak_decay "on" (1) */
gint		peak_hold_time;/* peak hold time ONLY works with peak_decay "on" (1) */
gint		time_border;	/* border bottom of spectrogram */
gint		x_border;	/* border on right side of display */
gint		x_offset;	/* 3D X axis offset for centering */

gint		y_border;	/* border on right side of display */
gint		y_offset;	/* 3D X axis offset for centering */
float		scale;
gint		recalc_scale;	/* its NOT fixed YET. (done dynamically) */
gint		recalc_markers;	/* flag for marker recalculation routine */
gint		show_leader;	/* show leading edge on 3D landscape fft */
gfloat		multiplier;	/* Level multiplier, for fft routines */
gint		dir_win_present;/* Flag */
gint		grad_win_present;/* Flag */

GtkWidget	*dir_win_ptr;	/* Pointer */
GtkWidget	*grad_win_ptr;	/* Pointer */
GtkWidget	*options_win_ptr;/* pointer specific to that button */

double		levels[MAXBANDS];/* Levels on screen for 3D and 3D Eq/spectral */
double		plevels[MAXBANDS];/* Levels on screen for 3D and 3D Eq/spectral */
gint		trail_counter[MAXBANDS];/* Peak/Hold counter for 2D EQ */
double		trailers[MAXBANDS];/* Levels for "trailers" in 2D EQ */
double		ptrailers[MAXBANDS];/* Levels for "trailers" in 2D EQ */
gfloat		freqmark[MAXBANDS];/* Frequency markers for 2D EQ */
gfloat		freq_at_pointer;/* Frequency at mouse pointer */
gint		window_func;	/* Which window function r we using? */
gint		axis_type;	/* Linear or LOG (NOT SPIKE or SPECT modes) */
gint		dir_x_origin;	/* Coords of direction window */
gint		dir_y_origin;	/* Coords of direction window */
gint		tape_scroll;
gint		display_markers;/* Are markers showing? */
gint		vert_spec_start;/* place on screen where spectram starts(abs) */
gint		horiz_spec_start;/* place on screen where spectram starts(abs) */
gint		color_loc;	/* pixel location in color gradient for color mapper */
//gint		spec_drag; 	/* spectrogram is being dragged */
gint		cr[MAXBANDS],cg[MAXBANDS],cb[MAXBANDS];
gint		scope_sub_mode;	/* Dot, line or gradient sub mode  */
gint		sub_mode_3D;	/* sub mode for 3D modes */
gint		data_source;	/* ESD (work in progress) */
int		last_buf_l[CONVOLVE_SMALL];/* Convolve buffer */
short		cur_buf_l[CONVOLVE_BIG];/* Convolve buffer */
int		last_buf_r[CONVOLVE_SMALL];/* Convolve buffer */
short		cur_buf_r[CONVOLVE_BIG];/* Convolve buffer */
int		scope_begin_l;	/* Begining point in buffer to start displaying scope */
int		old_scope_begin_l;/* (for Convolve routines..) */
int		scope_begin_r;	/* Begining point in buffer to start displaying scope */
int		old_scope_begin_r;/* (for Convolve routines..) */
		
convolve_state	*l_state;	/* Pointer to make convolve happy */
convolve_state	*r_state;	/* Pointer to make convolve happy */

GdkColor	temp_color;	/* Temporary var for testing */
GdkColor	*start,*pt2,*pt3,*pt4,*end; /* Colors for gradient board */
gint		sync_to_left;	/* Scope displayt to sync on left channel */
gint		sync_to_right;	/* Scope displayt to sync on left channel */
gint		sync_independant;/* Each channel self synchronizes.. */
gint		r_count;
gint		show_graticule;	/* show scope graticule */
glong		frame_cnt;	/* Frame count */
struct		timeval cur_time, last_time;
struct		timeval audio_arrival, audio_display, latency;
struct		timeval audio_arrival_last, audio_display_last;
struct		timeval draw_win_time_last, draw_win_time;
struct		Color_map {
    gint steps;			/* Number of steps in the map */
    gint *triplets;		/* buffer of RGB triples for each step */
    gfloat *locations;		/* buffer of location point in the map */
    gchar *filename;	/* Currently used colormap */
}Color_map;

fftw_plan	plan;		/* fft plan for fftw library */
gint		landtilt;	/* flag */
gint		spiketilt;	/* flag */
gint		tag;		/* Used by gdk_input_* */
gint		landflip;	/* Invert Y axis on 3D modes */
gint		spikeflip;	/* Invert Y axis on 3D modes */
gint		outlined;	/* Outlined 3D landform flag */
gint		fft_signal_source;/* Left right or both channels */
gint		refresh_rate;	/* display refresh rate*/
guint 		display_id;	/* display ID for gtk_timeout_* */
gint		convolve_factor;	
gshort 		*raw_ptr;
gshort 		copywindow;	/* size of intermediate buffer for processing */
gint 		copy_window;	/* intermediate buffer size */
gfloat		update_factor;
gint 		draw_running;
gint 		win_width;	/* window function options */	
gint		active_drawing_area;
gfloat 		noise_floor;
gint 		paused;			/* Flag */
gfloat		low_freq;
gfloat		high_freq;
gint 		bandwidth_change;	/* Flag */
gint 		clear_display;		/* Flag */

#endif