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
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <gdk_imlib.h>
#include <sys/time.h>
#include "convolve.h"
#include "config.h"
#include "defines.h"
#ifdef HAVE_LIBRFFTW
#include <rfftw.h>
#endif
#ifdef HAVE_LIBDRFFTW
#include <drfftw.h>
#endif
#ifdef HAVE_ALSA
#include <sys/asoundlib.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#else
#error PThreads are required to compile eXtace
#endif


gint		nsamp;		/* number of samples */
gint		bands;		/* to start with, should be configurable */
GdkWindow	*win;		/* Duuuuuuhhhh */
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
GtkWidget	*grad_disp;	/* Gradient display pointer */
GtkWidget	*window_function;/* window functions */
//GtkWidget	*options;	/* pointer specific to that button */
GtkWidget	*stars;		/* pointer specific to that button */
GdkGC		*gc;		/* Main graphics context */
GdkGC		*graticule_gc;	/* Graphics context for graticule in scope */
GdkGC		*arc_gc;	/* Graphics context for Arc in dircontrol */
GdkGC		*latency_monitor_gc;/* Graphics context for Arc in dircontrol */
GdkImlibImage	*im;		/* Image for colormap */
gulong		ring_pos;	/* place safe to write in ringbuffer */
gulong		process_ptr;	/* place safe to write in ringbuffer */
gulong		ring_end;	/* end of ringbuffer */
gshort		*audio_ring;	/* Array of pointers to audio data from esd */
gshort		*incoming_buf;	/* Array of pointers to audio data from esd */
gshort		*audio_left;	/* left channel (scope??) */
gshort		*audio_last_l;	/* last one of above, left channel (scope??) */
gshort		*audio_last_r;	/* last one of above, right channel (scope??) */
gshort		*audio_right;	/* right channel (scope??) */
gdouble		*raw_fft_out;	/* RAW output of fft routine */
gdouble		*norm_fft;	/* normalized fft data */
gdouble		*audio_data;	/* pointer to audio buffer being crunched  */
gdouble		*datawindow;	/* pointer to window function array */
gint		*pip_arr;	/* array of pip values for screen */
gint		*disp_val;	/* Display level for screen */
gint		keep_reading;	/* keeps audio loop running */
gint		alsa_processor_running; /* FLAG when function is running */
gint		esd_processor_running; /* FLAG when function is running */
gint 		lag;		/* delay between getting audio and displaying */
gint 		fft_lag;	/* delay between getting audio and displaying */
gint   		width;		/* Main window width */
gint   		height;		/* Main window height */
gint   		buffer_area_width;/* buffer_area window width */
gint   		buffer_area_height;/* buffer_area window height */
gint   		dir_width;	/* Direction control width */
gint   		dir_height;	/* Direction control height */
gint  		colortab[MAXBANDS][MAXBANDS];/* ugly, statically allocated to up to MAXBANDS bands */
gint 		colortab_ready;	/* flag */
gint		mode;		/* What display mode are we in */

/* New stuff (configurables) */

gint		seg_height;	/* height per segment in 2d spectrum analyzer */
gint		seg_space;	/* space between segments in 2d analyzer */
gint		peak_decay;	/* peak_decay and decay are tied together */
gint		stabilized;	/* scope trace stabilizer option */
gint		bar_decay;	/* bar_decay and decay are tied together */
gint		bar_decay_speed;/* decay_speed ONLY works with bar_decay "on" (1) */
gint		peak_decay_speed;/* decay_val ONLY works with peak_decay "on" (1) */
gint		peak_hold_time;/* peak hold time ONLY works with peak_decay "on" (1) */
gint		xdet_scroll;	/* 3D spike scroll in pixels */
gint		zdet_scroll;	/* 3D spike scroll in pixels */
gfloat		xdet_start;	/* The 3D SPIKE fft's amount of horizontal */
gfloat		ydet_start;	/* 3D spike y axis start position (percent) */
gfloat		x3d_start;	/* The 3D X start point of axis (percentage) */
gfloat		y3d_start;	/* The 3D Y start point of axis (percentage) */
gfloat		xdet_end;	/* The 3D DETAILED fft's amount of horizontal */
gfloat		ydet_end;	/* detailed y axis start position (percent) */
gfloat		x3d_end;	/* 3D fft X end point of axis (percentage) */
gfloat		y3d_end;	/* 3D fft Y end point of axis (percentage) */
gint		x3d_scroll;	/* 3D scroll in pixels x axis */
gint		z3d_scroll;	/* 3D scroll in pixels z axis */
gint		time_border;	/* border bottom of spectrogram */
gint		x_border;	/* border on right side of display */
gint		x_offset;	/* 3D X axis offset for centering */
gint		x_shift;	/* 3D shift factor(x axis)depending on axis tilt */
gint		x_shift_per_block;/* shift in pixels per block for 3D mode */
gfloat		x_tilt;		/* Tilt factor for semi-perspective views */
gfloat		y_tilt;		/* Tilt factor for semi-perspective views */
gfloat		xaxis_tilt;	/* Tilt factor for semi-perspective views */
gfloat		yaxis_tilt;	/* Tilt factor for semi-perspective views */

gint		y_border;	/* border on right side of display */
gint		y_offset;	/* 3D X axis offset for centering */
gint		y_shift;	/* 3D shift factor(x axis)depending on axis tilt */
gint		y_shift_per_block;/* shift in pixels per block for 3D mode */
float		scale;
gint		recalc_scale;	/* its NOT fixed YET. (done dynamically) */
gint		recalc_markers;	/* flag for marker recalculation routine */
float		scalefactor; 	/* dynamically figured out by the program */
gint		show_leader;	/* show leading edge on 3D landscape fft */
gfloat		multiplier;	/* Level multiplier, for fft routines */
gint		x_fudge;	/* fudge factor */
gint		y_fudge;	/* fudge factor */
gint		top;		/* Top of window */
gint		bottom;		/* Bottom or window */
gint		maxlevel;	/* maximum level reached on Graphic Eq mode */
gint		prevlevel;	/* maximum level reached on Graphic Eq mode */
gfloat		pix_per_block;	/* pixels per block */
gint		pix_int;	/* int version of above */
gint		space_used;	/* space the display takes up */
gint		extras;
gint		to_get;
gint		last_is_full;	/* its initially ready ?? */
gint		ready;		/* is everything initialized? */
gfloat		x_disp;		/* X displacement */
gfloat		y_disp; 	/* Y displacement */
gfloat		old_x_disp;	/* X displacement */
gfloat		old_y_disp;	/* Y displacement */
gint		dir_win_present;/* Flag */
gint		grad_win_present;/* Flag */

GtkObject	*x3d_start_ptr;	/* Pointer */
GtkObject	*x3d_end_ptr;	/* Pointer */
GtkObject	*y3d_start_ptr;	/* Pointer */
GtkObject	*y3d_end_ptr;	/* Pointer */
GtkObject	*xdet_start_ptr;/* Pointer */
GtkObject	*xdet_end_ptr;	/* Pointer */
GtkObject	*ydet_start_ptr;/* Pointer */
GtkObject	*ydet_end_ptr;	/* Pointer */
GtkObject	*x3d_scroll_ptr;/* Pointer */
GtkObject	*z3d_scroll_ptr;/* Pointer */
GtkObject	*xdet_scroll_ptr;/* Pointer */
GtkObject	*zdet_scroll_ptr;/* Pointer */
GtkWidget	*main_win_ptr;	/* Pointer */
GtkWidget	*dir_win_ptr;	/* Pointer */
GtkWidget	*about_but_ptr;	/* Pointer */
GtkWidget	*grad_win_ptr;	/* Pointer */
GtkWidget	*options_win_ptr;/* pointer specific to that button */
GtkWidget	*colorseldlg; 	/* Pointer */
GtkObject	*lag_adj; 	/* Pointer */


double		levels[MAXBANDS];/* Levels on screen for 3D and 3D Eq/spectral */
double		plevels[MAXBANDS];/* Levels on screen for 3D and 3D Eq/spectral */
gint		trail_counter[MAXBANDS];/* Peak/Hold counter for 2D EQ */
double		trailers[MAXBANDS];/* Levels for "trailers" in 2D EQ */
double		ptrailers[MAXBANDS];/* Levels for "trailers" in 2D EQ */
gfloat		freqmark[MAXBANDS];/* Frequency markers for 2D EQ */
gfloat		freq_at_pointer;/* Frequency at mouse pointer */
gint		pt_lock;	/* Lock variable for DND code */
gint		one_to_fix;	/* DND helper variable (which axis to modify) */
gint		window_func;	/* Which window function r we using? */
gint		axis_type;	/* Linear or LOG (NOT SPIKE or SPECT modes) */
gint		main_x_origin;	/* Coords of main window on screen */
gint		main_y_origin;	/* Coords of main window on screen */
gint		grad_x_origin;	/* Coords of gradient window */
gint		grad_y_origin;	/* Coords of gradient window */
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
gint		sound_source;	/* ALSA or ESD (alsa is work in progress) */
gdouble		left_amplitude;	/* Scaler */
gdouble		right_amplitude;/* Scaler */
int		alsa_card;	/* Alsa Soundcard number */
int		alsa_device;	/* Alsa PCM Device number */
int		alsa_sub_dev;	/* Alsa PCM subdevice  (multichannel cards)*/
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
gint		use_back_pixmap;/* Use backing pixmap or not? */
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
gfloat		land_axis_angle;/* angle of 3D axis in degrees */
gfloat		det_axis_angle;	/* angle of 3D axis in degrees */
gint		tag;		/* Used by gdk_input_* */
#ifdef HAVE_PTHREAD_H
/* Temporarily depreciated.  may be needed in the future */
#endif
gint		landflip;	/* Invert Y axis on 3D modes */
gint		spikeflip;	/* Invert Y axis on 3D modes */
gint		fft_signal_source;/* Left right or both channels */
gint		refresh_rate;	/* display refresh rate*/
guint 		display_id;	/* display ID for gtk_timeout_* */
gint 		callback_buffer_size;/* ALSA loopback callback buffer size */	
gint		convolve_factor;	
gshort 		*raw_ptr;
gfloat		update_factor;
gint 		rtc_fd;		/* Real Time Clock Filedescriptor */
gint 		use_rtc;
gint 		draw_running;
gint 		winstyle;	/* window function options */	
gint		active_drawing_area;
gfloat 		noise_floor;
gfloat 		noise_floor_min;
gfloat 		noise_floor_max;
gint 		paused;
gint		low_freq;
gint		high_freq;
gfloat		bandwidth;
gint 		clear_display;
