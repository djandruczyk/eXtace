
/*
 *  /GDK/GNOME sound (esd) system output display program
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


#include <config.h>
#include <configfile.h>
#include <datawindow.h>
#include <draw.h>
#include <enums.h>
#include <fcntl.h>
#include <globals.h>
#include <init.h>
#include <math.h>
#include <input.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <buttons.h>

gint major_ver;
gint minor_ver;
gint micro_ver;
gint main_x_origin;
gint main_y_origin;

extern gint ready;
extern gint seg_height;	/* from 2d_eq.c */
extern gint seg_space;	/* from 2d_eq.c */
extern gfloat xdet_start;
extern gfloat xdet_end;
extern gfloat x3d_start;
extern gfloat x3d_end;
extern gfloat ydet_start;
extern gfloat ydet_end;
extern gfloat y3d_start;
extern gfloat y3d_end;
extern gint xdet_scroll;    /* 3D spike scroll in pixels */
extern gint zdet_scroll;    /* 3D spike scroll in pixels */
extern gint x3d_scroll;     /* 3D scroll in pixels x axis */
extern gint z3d_scroll;     /* 3D scroll in pixels z axis */
extern gint grad_x_origin;
extern gint grad_y_origin;
extern gfloat left_amplitude;
extern gfloat right_amplitude;
extern GtkObject *lf_adj;
extern GtkObject *hf_adj;
extern GtkObject *lag_adj;

void init()
{
	extern gint buffer_area_width;
	extern gint buffer_area_height;
	extern gint dir_width;
	extern gint dir_height;
	extern gint seg_height;
	extern gint seg_space;
	/* 
	   Initialize ALL variables, 
	   should be first functional called from main.
	   These are still needed in case
           default file is missing or incomplete.
	 */

	data_handle = -1;  /* initialize to empty handle */
	data_source = ESD;

	refresh_rate = 29;	/* 25 frames per sec */
	left_amplitude = 127.0/32768.0; /* Scaler for something */
	right_amplitude = 127.0/32768.0; /* Scaler for something */
	fft_signal_source = LEFT_PLUS_RIGHT;/* signal input source for fft */
	landflip = 1;		/* Flip 3D axis over */
	spikeflip = 1;		/* Flip 3D axis over */
	axis_type = LOG;	/* Logarithmic display for 3D land and EQ modes */
	window_func = HAMMING;	/* Hamming window (see misc.c) */
	win_width = FULL;	/* use full window function, not cramped version */
	nsamp = 2048;		/* number of samples per FFT/scope */
	bands = 128;		/* to start with, should be configurable */
	
	bandwidth_change = 0;	/* FLAG, reset to default */
	mode = LAND_3D;		/* default mode. (3D FFT) */
	sub_mode_3D = FILL_3D;	/* default 3D mode */
	scope_sub_mode = LINE_SCOPE;/* default Scope mode */
	show_graticule = 1;	/* show scope graticule*/
	lag = 360;		/* Lag (how many milliseconds behind) */
	decimation_factor=1;	
	seg_height = 2;		/* height per segment in 2d spectrum analyzer */
	seg_space = 1;		/* space between segments in 2d analyzer */
	stabilized = 1;		/* Scope stabilizer routine */
	bar_decay = 0;		/* bar_decay and peak_decay are tied together */
	peak_decay = 0;		/* bar_decay and peak_decay are tied together */
	bar_decay_speed = 7;	/* decay_speed ONLY works with bar_decay "on" (1) */
	peak_decay_speed = 1;	/* decay_speed ONLY works with peak_decay "on" (1) */
	peak_hold_time = 10;	/* hold_time ONLY works with peak_decay "on" (1) */
	xdet_scroll = 2;	/* detailed scroll in pixels */
	zdet_scroll = 2;	/* detailed scroll in pixels */
	xdet_start= 0.00;	/* The 3d DETAILED fft's amount of horizontal */
	ydet_start = 0.00;	/* detailed y axis start position (percent) */
	xdet_end = 1.00;	/* The 3d DETAILED fft's amount of horizontal */
	ydet_end = 0.23;	/* detailed y axis start position (percent) */
	x3d_start = 0.00;	/* The 3d X start point of axis (percentage) */
	y3d_start = 0.00;	/* The 3d Y start point of axis (percentage) */
	x3d_end = 0.95;		/* 3D fft X end point of axis (percentage) */
	y3d_end = 0.13;		/* 3D fft Y end point of axis (percentage) */
	x3d_scroll = 3;		/* 3D scroll in pixels x axis */
	z3d_scroll = 6;		/* 3D scroll in pixels z axis */
	border = 8;		/* border around most displays */
	x_offset = 0;		/* 3D X axis offset for centering */
	landtilt = 1;		/* Flag */
	outlined = TRUE;	/* Outlined 3D Landform style */
	spiketilt = 0;		/* Flag */

	y_offset = 0;		/* 3D X axis offset for centering */
	recalc_scale = 1;	/* its NOT fixed YET. (done dynamically) */
	recalc_markers = 1;	/* its NOT fixed YET. (done dynamically) */
	show_leader = 0;	/* show leading edge on 3d landscape fft */
	multiplier = 26.0;	/* Level multiplier, fft amplitude adj */
	noise_floor = -80;	/* FFT noise floor position. (NEEDS WORK!!!) */
	/* WON'T max out when you resize */
	dir_win_present = 1;	/* Direction control window */
	grad_win_present = 0;	/* Color picker window */
	height = 480;		/* Self explanitory */
	width  = 640;		/* Self explanitory */
	buffer_area_height = 100;	/* Self explanitory */
	buffer_area_width  = 400;	/* Self explanitory */
	dir_width = 100;	/* Self explanitory */
	dir_height = 100;	/* Self explanitory */
	main_x_origin = 40;	/* window locations on screen */
	main_y_origin = 40;	/* window locations on screen */
	dir_x_origin = width + 0;
	dir_y_origin = 0;
	grad_x_origin = width + 0;
	grad_y_origin = dir_y_origin + dir_height;
	tape_scroll = 2;
	horiz_spec_start = 60;	/* 60 from right edge of screen */
	vert_spec_start = 120;	/* 120 from BOTTOM of the screen, unconventional */
	sync_to_left = 1;	/* default to sync to left channel */
	sync_to_right = 0; 	/* sync to right channel */
	sync_independant = 0;	 /* independtant sync */
	paused = 0;		 /* display running */
	low_freq = 0;		 /* Low frequency cutoff in hi-res displays */
	clear_display = 0;	/* Flag for markers */

	/*	Color presets (default colormap) */

	//    printf("eXtace version is %i.%i.%i\n",_MAJOR_,_MINOR_,_MICRO_);
}

void read_config(void)
{
	ConfigFile *cfgfile;
	gchar *filename;
	int fd;
	gchar *temp_cmap = NULL;
	Color_map.filename = NULL;
	filename = g_strconcat(g_get_home_dir(), "/.eXtace/config", NULL);
	cfgfile = cfg_open_file(filename);
	if (cfgfile)
	{
		cfg_read_int(cfgfile, "Global", "major_ver", &major_ver);
		cfg_read_int(cfgfile, "Global", "minor_ver", &minor_ver);
		cfg_read_int(cfgfile, "Global", "micro_ver", &micro_ver);
		if (major_ver == 0)
		{
			printf("Config file structure changed. using defaults. \nClosing eXtace will save your NEW settings.\n");
			cfg_free(cfgfile);
			unlink(filename);
			g_free(filename);
			return;

		}
		cfg_read_string(cfgfile, "Global", "last_colormap", &temp_cmap);
		if (temp_cmap != NULL)
		{
			fd = open(temp_cmap, O_RDONLY);
			if (fd > 0)
			{
				Color_map.filename = g_strdup(temp_cmap);
				close(fd);
			}
			else
				Color_map.filename = NULL;

			g_free(temp_cmap);
		}
		cfg_read_int(cfgfile, "Global", "landtilt", &landtilt);
		cfg_read_int(cfgfile, "Global", "spiketilt", &spiketilt);
		cfg_read_float(cfgfile, "Global", "low_freq", &low_freq);
		cfg_read_float(cfgfile, "Global", "high_freq", &high_freq);
		cfg_read_int(cfgfile, "Window", "width", &width);
		cfg_read_int(cfgfile, "Window", "height", &height);
		cfg_read_int(cfgfile, "Window", "main_x_origin", &main_x_origin);
		cfg_read_int(cfgfile, "Window", "main_y_origin", &main_y_origin);
		cfg_read_int(cfgfile, "Window", "grad_x_origin", &grad_x_origin);
		cfg_read_int(cfgfile, "Window", "grad_y_origin", &grad_y_origin);
		cfg_read_int(cfgfile, "Window", "dir_x_origin", &dir_x_origin);
		cfg_read_int(cfgfile, "Window", "dir_y_origin", &dir_y_origin);

		cfg_read_int(cfgfile, "Global", "mode", &mode);
		{
		  int i;
		  if(cfg_read_int(cfgfile, "Global", "data_source", &i))
		  data_source=i;
		}
		cfg_read_int(cfgfile, "Global", "decimation_factor", &decimation_factor);
		cfg_read_int(cfgfile, "Global", "fft_signal_source", &fft_signal_source);
		cfg_read_int(cfgfile, "Global", "refresh_rate", &refresh_rate);
		cfg_read_int(cfgfile, "Global", "landflip", &landflip);
		cfg_read_int(cfgfile, "Global", "spikeflip", &spikeflip);
		cfg_read_int(cfgfile, "Global", "outlined", &outlined);
		cfg_read_int(cfgfile, "Global", "sub_mode_3D", &sub_mode_3D);
		cfg_read_int(cfgfile, "Global", "scope_sub_mode", &scope_sub_mode);
		cfg_read_int(cfgfile, "Global", "dir_win_present", &dir_win_present);
		cfg_read_int(cfgfile, "Global", "nsamp", &nsamp);

		/* fft_lag is an added delay because the fft looks most synced to
		 * audio when viewing the "middle" of the datawindow. i.e. 
		 * at 1/2 the number of samples in the window
		 * whereas the scope looks best at the beginning of the window
		 * and the error would have been bad at large window sizes at the 
		 * expense of one display over the other (time vs freq domains)
		 * This factor helps to balance things out..
		 */
		fft_lag = 1000*((nsamp/2)/ring_rate);
		cfg_read_int(cfgfile, "Global", "window_func", &window_func);
		cfg_read_int(cfgfile, "Global", "win_width", &win_width);
		cfg_read_int(cfgfile, "Global", "axis_type", &axis_type);
		cfg_read_int(cfgfile, "Global", "bands", &bands);
		cfg_read_int(cfgfile, "Global", "lag", &lag);
		cfg_read_float(cfgfile, "Global", "noise_floor", &noise_floor);
		cfg_read_int(cfgfile, "Global", "seg_height", &seg_height);
		cfg_read_int(cfgfile, "Global", "seg_space", &seg_space);
		cfg_read_int(cfgfile, "Global", "bar_decay", &bar_decay);
		cfg_read_int(cfgfile, "Global", "peak_decay", &peak_decay);
		cfg_read_int(cfgfile, "Global", "stabilized", &stabilized);
		cfg_read_int(cfgfile, "Global", "show_graticule", &show_graticule);
		cfg_read_int(cfgfile, "Global", "decay_speed", &bar_decay_speed);
		cfg_read_int(cfgfile, "Global", "peak_decay_speed", &peak_decay_speed);
		cfg_read_int(cfgfile, "Global", "peak_hold_time", &peak_hold_time);
		cfg_read_int(cfgfile, "Global", "tape_scroll", &tape_scroll);
		cfg_read_int(cfgfile, "Global", "xdet_scroll", &xdet_scroll);
		cfg_read_int(cfgfile, "Global", "zdet_scroll", &zdet_scroll);
		cfg_read_float(cfgfile, "Global", "xdet_start", &xdet_start);
		cfg_read_float(cfgfile, "Global", "xdet_end", &xdet_end);
		cfg_read_float(cfgfile, "Global", "ydet_start", &ydet_start);
		cfg_read_float(cfgfile, "Global", "ydet_end", &ydet_end);
		cfg_read_float(cfgfile, "Global", "x3d_start", &x3d_start);
		cfg_read_float(cfgfile, "Global", "x3d_end", &x3d_end);
		cfg_read_float(cfgfile, "Global", "y3d_start", &y3d_start);
		cfg_read_float(cfgfile, "Global", "y3d_end", &y3d_end);
		cfg_read_float(cfgfile, "Global", "multiplier", &multiplier);
		cfg_read_int(cfgfile, "Global", "x3d_scroll", &x3d_scroll);
		cfg_read_int(cfgfile, "Global", "z3d_scroll", &z3d_scroll);
		cfg_read_int(cfgfile, "Global", "show_leader", &show_leader);
		cfg_read_int(cfgfile, "Global", "sync_to_left", &sync_to_left);
		cfg_read_int(cfgfile, "Global", "sync_to_right", &sync_to_right);
		cfg_read_int(cfgfile, "Global", "sync_independant", &sync_independant);
		cfg_read_int(cfgfile, "Global", "horiz_spec_start", &horiz_spec_start);
		cfg_read_int(cfgfile, "Global", "vert_spec_start", &vert_spec_start);
		if (horiz_spec_start > width)
			horiz_spec_start = width-10; 
		if (vert_spec_start > height)
			vert_spec_start = height-10;
		if (horiz_spec_start < 60)
			horiz_spec_start = 60; 
		if (vert_spec_start < 120)
			vert_spec_start = 120;


		cfg_free(cfgfile);

	}
	else
		printf("Config file not found, using defaults\n");
	g_free(filename);

}
void save_config(GtkWidget *widget)
{
	gchar *filename;
	ConfigFile *cfgfile;
	gint x;
	gint y;
	filename = g_strconcat(g_get_home_dir(), "/.eXtace/config", NULL);
	cfgfile = cfg_open_file(filename);
	if (!cfgfile)
		cfgfile = cfg_new();

	cfg_write_int(cfgfile, "Global", "major_ver", _MAJOR_);
	cfg_write_int(cfgfile, "Global", "minor_ver", _MINOR_);
	cfg_write_int(cfgfile, "Global", "micro_ver", _MICRO_);
	if (Color_map.filename)
		cfg_write_string(cfgfile, "Global", "last_colormap", Color_map.filename);
	else
		cfg_write_string(cfgfile, "Global", "last_colormap",g_strconcat(g_get_home_dir(),"/.eXtace/ColorMaps/","Default",NULL));
	cfg_write_int(cfgfile, "Global", "mode", mode);
	cfg_write_int(cfgfile, "Global", "data_source", data_source);
	cfg_write_int(cfgfile, "Global", "decimation_factor", decimation_factor);
	cfg_write_int(cfgfile, "Global", "fft_signal_source", fft_signal_source);
	cfg_write_int(cfgfile, "Global", "refresh_rate", refresh_rate);
	cfg_write_int(cfgfile, "Global", "landflip", landflip);
	cfg_write_int(cfgfile, "Global", "spikeflip", spikeflip);
	cfg_write_int(cfgfile, "Global", "outlined", outlined);
	cfg_write_int(cfgfile, "Global", "sub_mode_3D", sub_mode_3D);
	cfg_write_int(cfgfile, "Global", "scope_sub_mode", scope_sub_mode);
	cfg_write_int(cfgfile, "Global", "dir_win_present", dir_win_present);
	cfg_write_int(cfgfile, "Global", "nsamp", nsamp);
	cfg_write_int(cfgfile, "Global", "window_func", window_func);
	cfg_write_int(cfgfile, "Global", "win_width", win_width);
	cfg_write_int(cfgfile, "Global", "axis_type", axis_type);
	cfg_write_int(cfgfile, "Global", "bands", bands);
	cfg_write_int(cfgfile, "Global", "lag", lag);
	cfg_write_float(cfgfile, "Global", "noise_floor", noise_floor);
	cfg_write_int(cfgfile, "Global", "seg_height", seg_height);
	cfg_write_int(cfgfile, "Global", "seg_space", seg_space);
	cfg_write_int(cfgfile, "Global", "bar_decay", bar_decay);
	cfg_write_int(cfgfile, "Global", "peak_decay", peak_decay);
	cfg_write_int(cfgfile, "Global", "stabilized", stabilized);
	cfg_write_int(cfgfile, "Global", "show_graticule", show_graticule);
	cfg_write_int(cfgfile, "Global", "decay_speed", bar_decay_speed);
	cfg_write_int(cfgfile, "Global", "peak_decay_speed", peak_decay_speed);
	cfg_write_int(cfgfile, "Global", "peak_hold_time", peak_hold_time);
	cfg_write_int(cfgfile, "Global", "tape_scroll", tape_scroll);
	cfg_write_int(cfgfile, "Global", "xdet_scroll", xdet_scroll);
	cfg_write_int(cfgfile, "Global", "zdet_scroll", zdet_scroll);
	cfg_write_float(cfgfile, "Global", "xdet_start", xdet_start);
	cfg_write_float(cfgfile, "Global", "xdet_end", xdet_end);
	cfg_write_float(cfgfile, "Global", "ydet_start", ydet_start);
	cfg_write_float(cfgfile, "Global", "ydet_end", ydet_end);
	cfg_write_float(cfgfile, "Global", "x3d_start", x3d_start);
	cfg_write_float(cfgfile, "Global", "x3d_end", x3d_end);
	cfg_write_float(cfgfile, "Global", "y3d_start", y3d_start);
	cfg_write_float(cfgfile, "Global", "y3d_end", y3d_end);
	cfg_write_float(cfgfile, "Global", "multiplier", multiplier);
	cfg_write_int(cfgfile, "Global", "horiz_spec_start", horiz_spec_start);
	cfg_write_int(cfgfile, "Global", "vert_spec_start", vert_spec_start);
	cfg_write_int(cfgfile, "Global", "x3d_scroll", x3d_scroll);
	cfg_write_int(cfgfile, "Global", "z3d_scroll", z3d_scroll);
	cfg_write_int(cfgfile, "Global", "show_leader", show_leader);
	cfg_write_int(cfgfile, "Global", "sync_to_left", sync_to_left);
	cfg_write_int(cfgfile, "Global", "sync_to_right", sync_to_right);
	cfg_write_int(cfgfile, "Global", "sync_independant", sync_independant);
	cfg_write_int(cfgfile, "Global", "landtilt",landtilt);
	cfg_write_int(cfgfile, "Global", "spiketilt", spiketilt);
	cfg_write_float(cfgfile, "Global", "low_freq", low_freq);
	cfg_write_float(cfgfile, "Global", "high_freq", high_freq);
	cfg_write_int(cfgfile, "Window", "width", width);
	cfg_write_int(cfgfile, "Window", "height", height+22);
	gdk_window_get_root_origin(widget->window, &x, &y);
	cfg_write_int(cfgfile, "Window", "main_x_origin", x);
	cfg_write_int(cfgfile, "Window", "main_y_origin", y);
	//    cfg_write_int(cfgfile, "Window", "grad_win_present", grad_win_present);
	if (grad_win_present)
	{
		gdk_window_get_root_origin((gpointer) grad_win_ptr->window, &x, &y);
		cfg_write_int(cfgfile, "Window", "grad_x_origin", x);
		cfg_write_int(cfgfile, "Window", "grad_y_origin", y);
	}
	if (dir_win_present)
	{
		gdk_window_get_root_origin((gpointer) dir_win_ptr->window, &x, &y);
		cfg_write_int(cfgfile, "Window", "dir_x_origin", x);
		cfg_write_int(cfgfile, "Window", "dir_y_origin", y);
	}

	cfg_write_file(cfgfile, filename);
	cfg_free(cfgfile);

	g_free(filename);

}
void make_extace_dirs(void)
{
	gchar *filename;

	filename = g_strconcat(g_get_home_dir(), "/.eXtace", NULL);
	mkdir(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	g_free(filename);
	filename = g_strconcat(g_get_home_dir(), "/.eXtace/ColorMaps", NULL);
	mkdir(filename, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	g_free(filename);


}


void mem_alloc()
{
	l_state = convolve_init();
	if (!l_state) 
		exit(1);
	r_state = convolve_init();
	if (!r_state) 
		exit(1);

	/* Color Gradient variables */
	start = malloc(nsamp*sizeof(GdkColor));
	pt2 = malloc(nsamp*sizeof(GdkColor));
	pt3 = malloc(nsamp*sizeof(GdkColor));
	pt4 = malloc(nsamp*sizeof(GdkColor));
	end = malloc(nsamp*sizeof(GdkColor));

	/* Audio Data Specific definitions */
#define FRAMES		88200  // Audio ring size in audio "frames"
	/* 1 frame is a left and right channel of audio, Signed 16 bit LE.
	   Thus 1 frame is 32 bits in total size, 16 bits for left and 16 
	   bits for right.
	   I'm making the assumption that we are using standard STEREO 
	   (2 channel)
	   audio for our source. (that may change in the distant future)
	   A frame for a 4 channel input card would NOT be the same for a 
	   stereo input.
	*/
#define NSEC 2 // number of seconds
	ring_end = FRAMES*NSEC;  // Audio ring size (NSEC at 44100/stereo)
	/* 
	   Actual buffer is twice that value in size, because its a buffer 
	   of "shorts." 1 "short" = 16 bits, thus 2 bytes, thus total 
	   buffer size is 176000 bytes per second.
	*/
	ringbuffer = malloc(ring_end*sizeof(ring_type));
	/* initialize array to zero */
	memset((void *)ringbuffer, 0, ring_end*sizeof(ring_type));

	/* Audio block in frequency domain being processed, size of nsamp */
	raw_fft_out = malloc(nsamp*sizeof(gdouble));
	raw_fft_in = malloc(nsamp*sizeof(gdouble));
	/* datawindow applied to TIME series data, thus need "nsamp" datapoints */
	datawindow = malloc(nsamp*sizeof(gdouble));
	/* FFT after scaling/massaging, size of (nsamp+1)/2 */
	norm_fft = malloc(nsamp*sizeof(gdouble));

	audio_left = malloc(nsamp*sizeof(gshort));
	audio_last_l = malloc(nsamp*sizeof(gshort));
	audio_right = malloc(nsamp*sizeof(gshort));
	audio_last_r = malloc(nsamp*sizeof(gshort));

	/* Display values of norm_fft, scaled for screen viewing ,
	 * for low resolution fft's (LAND_3D) */
	disp_val = malloc(nsamp*sizeof(gint));
	/* array of values for screen viewing after interpolation to fit display
	 * width, designed for hi-res fft's 
	 * We make the assumption no one has a screen over 8192 pixels wide
	 * just in case.. :) Otherwise eXtace will prolly crash
	 */

	pip_arr = malloc(8192*sizeof(gint));


	if ((raw_fft_out == NULL) \
	                || (ringbuffer == NULL) \
			|| (raw_fft_in == NULL) \
			|| (start == NULL) \
			|| (pt2 == NULL) \
			|| (pt3 == NULL) \
			|| (pt4 == NULL) \
			|| (end == NULL) \
			|| (datawindow == NULL) \
			|| (norm_fft == NULL) \
			|| (pip_arr == NULL) \
			|| (disp_val == NULL) \
			|| (audio_left == NULL) \
			|| (audio_right == NULL) \
			|| (audio_last_l == NULL) \
			|| (audio_last_r == NULL)) 
			
	{   
		g_print("Memory could NOT be allocated!!!!, Exiting now!\n");
		exit (-2);
	}
	memset((void *)start , 0, nsamp*sizeof(GdkColor));
	memset((void *)pt2 , 0, nsamp*sizeof(GdkColor));
	memset((void *)pt3 , 0, nsamp*sizeof(GdkColor));
	memset((void *)pt4 , 0, nsamp*sizeof(GdkColor));
	memset((void *)end , 0, nsamp*sizeof(GdkColor));

	memset((void *)raw_fft_out , 0, nsamp*sizeof(gdouble));
	memset((void *)raw_fft_in , 0, nsamp*sizeof(gdouble));
	memset((void *)norm_fft , 0, nsamp*sizeof(gdouble));
	memset((void *)datawindow , 0, nsamp*sizeof(gdouble));
	memset((void *)audio_left , 0, nsamp*sizeof(gshort));
	memset((void *)audio_last_l , 0, nsamp*sizeof(gshort));
	memset((void *)audio_right , 0, nsamp*sizeof(gshort));
	memset((void *)audio_last_r , 0, nsamp*sizeof(gshort));
	memset((void *)pip_arr , 0, 8192*sizeof(gint));
	memset((void *)disp_val , 0, nsamp*sizeof(gint));
}

void mem_dealloc()
{
	free(ringbuffer);  
	free(raw_fft_out);
	free(raw_fft_in);
	free(norm_fft);
	free(datawindow);
	free(audio_left);
	free(audio_last_l);
	free(audio_right);
	free(audio_last_r);
	free(pip_arr);
	free(disp_val);
	ring_end=0;

	free(start);
	free(pt2);
	free(pt3);
	free(pt4);
	free(end);
	//	free(Color_map.triplets);
	//	free(Color_map.locations);

	if (plan)
		rfftw_destroy_plan(plan);
	convolve_close(l_state); 
	convolve_close(r_state); 
}

void reinit_extace(int new_nsamp)
{

  /* Stop drawing the display */
    draw_stop();
    if(data_handle != -1) /* stop if previously opened */
      { 
	input_thread_stopper(data_handle);
	close_datasource(data_handle);
      }	

  /* Free all buffers */
        mem_dealloc();
	scope_begin_l = 0;
	scope_begin_l = 0;
	old_scope_begin_l = 0;
	old_scope_begin_r = 0;
	/* auto shift lag slightly to maintain good sync 
	 * The idea is the shift the lag slighly so that the "on-time" data
	 * is in the MIDDLE of the window function for better eye/ear matchup
	 */
	nsamp = new_nsamp;
	fft_lag = 1000*((nsamp/2)/ring_rate);

	convolve_factor = floor(nsamp/width) < 3 ? floor(nsamp/width) : 3 ;
	if (convolve_factor == 0)
		convolve_factor = 1;
	recalc_markers = 1;
	recalc_scale = 1;	
	mem_alloc();
	setup_datawindow(NULL,(WindowFunction)window_func);
	ring_rate_changed();
	ring_pos=0;
	
	/* only start if it has been stopped above */
	if(data_handle != -1 && (data_handle=open_datasource(data_source)) >= 0)
	  {
	    input_thread_starter(data_handle);
	    draw_start();
	  }
}

void ring_rate_changed()
{
	/* Fixes all adjustments that depend on sample rate */
	gfloat val = 0.0;
	gfloat low = 0.0;
	gfloat upper = 0.0;
	gfloat percentage = 0.0;
	gfloat newval = 0.0;

	if (!ready)
		return;

	/* The idea behind this is pretty cool.  
	 * First off, if you increas the decimation or fft size, the
	 * low limit goes lower.  What this does is gets the adjustments
	 * position as a percentage of range, alters the limits of that 
	 * range and recalculates a new value and moves the pointer.  This
	 * way if the pointer was atthe min, and you increased the fft size
	 * the adjustment will auotmatically move to show you the increaed
	 * resolution...
	 */
	/* Store values BEFORE we change the limits... */
	val = GTK_ADJUSTMENT(lf_adj)->value;
	low = GTK_ADJUSTMENT(lf_adj)->lower;
	upper = GTK_ADJUSTMENT(lf_adj)->upper;
	percentage = (val-low)/(upper-low);
	/* Set new limits to the adjustment */
	GTK_ADJUSTMENT(lf_adj)->lower = 
			(float)ring_rate/(float)decimation_factor/(float)nsamp;
	GTK_ADJUSTMENT(lf_adj)->upper = 
			high_freq - 64.0*((float)ring_rate
			/ (float)decimation_factor/(float)nsamp);
	GTK_ADJUSTMENT(lf_adj)->step_increment = (float)ring_rate/(float)nsamp;
	GTK_ADJUSTMENT(lf_adj)->page_increment = (float)ring_rate/(float)nsamp;
	/* Copy new values to temp vars for new calc (cleaner code) */
	low = GTK_ADJUSTMENT(lf_adj)->lower;
	upper = GTK_ADJUSTMENT(lf_adj)->upper;
	newval = (percentage*(upper-low)) + low;
	/* Reset the value */
	GTK_ADJUSTMENT(lf_adj)->value = newval;

	/* Store values BEFORE we change the limits... */
	val = GTK_ADJUSTMENT(hf_adj)->value;
	low = GTK_ADJUSTMENT(hf_adj)->lower;
	upper = GTK_ADJUSTMENT(hf_adj)->upper;
	percentage = (val-low)/(upper-low);
	/* Set new limits to the adjustment */
	GTK_ADJUSTMENT(hf_adj)->lower = 
			low_freq + 64.0*((float)ring_rate
			/ (float)decimation_factor/(float)nsamp);
	GTK_ADJUSTMENT(hf_adj)->upper = 
			(float)ring_rate/(float)(2*decimation_factor)
			+ 10.001;
	GTK_ADJUSTMENT(hf_adj)->step_increment = (float)ring_rate/(float)nsamp;
	GTK_ADJUSTMENT(hf_adj)->page_increment = (float)ring_rate/(float)nsamp;
	/* Copy new values to temp vars for new calc (cleaner code) */
	low = GTK_ADJUSTMENT(hf_adj)->lower;
	upper = GTK_ADJUSTMENT(hf_adj)->upper;
	newval = (percentage*(upper-low)) + low;
	/* Reset the value */
	GTK_ADJUSTMENT(hf_adj)->value = newval;

	/* Force the adjustments to update on screen */
	gtk_adjustment_changed(GTK_ADJUSTMENT(lf_adj));
	gtk_adjustment_changed(GTK_ADJUSTMENT(hf_adj));

	/* Display lag control */
	GTK_ADJUSTMENT(lag_adj)->upper = (int)(1000*ring_end/(ring_rate*sizeof(ring_type)));
	gtk_adjustment_changed(GTK_ADJUSTMENT(lag_adj));

	return;
}
