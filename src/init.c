
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
#include <globals.h>
#include <protos.h>
#include <configfile.h>
#include <math.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

gint major_ver;
gint minor_ver;
gint micro_ver;

void init()
{
    /* Should actualy get these to/from a text file instead so settings 
     * persist between sessions.. :|  Ohh, how to do it..... :)
     * Initialize ALL variables, should be first functional called from main
     */

#ifdef HAVE_ALSA
    sound_source = ALSA;
#else
    sound_source = ESD;
#endif
    keep_reading = 1;
    use_rtc = 0;
    alsa_card = 0;	/* ALSA card, (physical card in the system */
    alsa_device = 0;	/* DAC/ADC on above card */
    alsa_sub_dev = 0;	/* Subdevice, i.e. multichannel cards */
    refresh_rate = 29;	/* 25 frames per sec */
    left_amplitude = 127.0/32768.0; /* Scaler for something */
    right_amplitude = 127.0/32768.0; /* Scaler for something */
    fft_signal_source = COMPOSITE;/* signal input source for fft */
    landflip = 1;		/* Flip 3D axis over */
    spikeflip = 1;		/* Flip 3D axis over */
    axis_type = LOG;	/* Logarithmic display for 3D land and EQ modes */
    ready = 0;		/* We're NOT ready yet until all main windows are up */
    window_func = HAMMING;/* Hamming window (see misc.c) */
    winstyle = FULL;	/* use full window function, not cramped version */
    nsamp = 2048;	/* number of samples per FFT/scope */
    callback_buffer_size = nsamp;
    bands = 128;	/* to start with, should be configurable */
    mode = LAND_3D;	/* default mode. (3D FFT) */
    sub_mode_3D = FILL_3D;/* default 3D mode */
    scope_sub_mode = LINE_SCOPE;/* default Scope mode */
    show_graticule = 1;	/* show scope graticule*/
    lag = 500;		/* Lag (how many milliseconds behind) */
    last_is_full = 0;	/* its initially ready ?? */

    seg_height = 2;	/* height per segment in 2d spectrum analyzer */
    seg_space = 1;	/* space between segments in 2d analyzer */
    stabilized = 1;	/* Scope stabilizer routine */
    bar_decay = 0;	/* bar_decay and peak_decay are tied together */
    peak_decay = 0;	/* bar_decay and peak_decay are tied together */
    bar_decay_speed = 7;/* decay_speed ONLY works with bar_decay "on" (1) */
    peak_decay_speed = 1;/* decay_speed ONLY works with peak_decay "on" (1) */
    peak_hold_time = 10;/* hold_time ONLY works with peak_decay "on" (1) */
    xdet_scroll = 2;	/* detailed scroll in pixels */
    zdet_scroll = 2;	/* detailed scroll in pixels */
    xdet_start= 0.00;	/* The 3d DETAILED fft's amount of horizontal */
    ydet_start = 0.00;	/* detailed y axis start position (percent) */
    x3d_start = 0.00;	/* The 3d X start point of axis (percentage) */
    y3d_start = 0.00;	/* The 3d Y start point of axis (percentage) */
    xdet_end = 1.00;	/* The 3d DETAILED fft's amount of horizontal */
    ydet_end = 0.23;	/* detailed y axis start position (percent) */
    x3d_end = 1.00;	/* 3D fft X end point of axis (percentage) */
    y3d_end = 0.13;	/* 3D fft Y end point of axis (percentage) */
    x3d_scroll = 3;	/* 3D scroll in pixels x axis */
    z3d_scroll = 6;	/* 3D scroll in pixels z axis */
    time_border = 30;	/* border on bottom of spectrogram display */
    x_border = 8;	/* border on right side of display */
    x_offset = 0;	/* 3D X axis offset for centering */
    x_shift = 0;	/* 3D shift factor(x axis)depending on axis tilt */
    x_shift_per_block = 0;/* shift in pixels per block for 3D mode */
    landtilt = 0;
    spiketilt = 0;

    y_border = 8;	/* border on right side of display */
    y_offset = 0;	/* 3D X axis offset for centering */
    y_shift = 0;	/* 3D shift factor(x axis)depending on axis tilt */
    y_shift_per_block = 0;/* shift in pixels per block for 3D mode */
    recalc_scale = 1;	/* its NOT fixed YET. (done dynamically) */
    recalc_markers = 1;	/* its NOT fixed YET. (done dynamically) */
    scalefactor = 10.0;	/* dynamically figured out by the program */
    show_leader = 0;	/* show leading edge on 3d landscape fft */
    multiplier= 240;	/* Level multiplier, so the colortable */
    /* WON'T max out when you resize */
    ydet_special = 0.0; /* Detailed fudge factor. (should be removed!!!) */
    dir_win_present = 1;/* Direction control window */
    grad_win_present = 0;/* Color picker window */
    one_to_fix = 0;	/* which end of trace to fix up */
    to_get = nsamp/2;	/* how many sampels to read at a time */
    height = 256;	/* Self explanitory */
    width  = 370;	/* Self explanitory */
    buffer_area_height = 100;	/* Self explanitory */
    buffer_area_width  = 400;	/* Self explanitory */
    dir_width = 100;	/* Self explanitory */
    dir_height = 100;	/* Self explanitory */
    main_x_origin = 0;	/* window locations on screen */
    main_y_origin = 0;	/* window locations on screen */
    dir_x_origin = width + 0;
    dir_y_origin = 0;
    grad_x_origin = width + 0;
    grad_y_origin = dir_y_origin + dir_height;
    tape_scroll = 2;
    horiz_spec_start = 55; /* 55 from right edge of screen */
    vert_spec_start = 120; /* 120 from BOTTOM of the screen, unconventional */
    sync_to_left = 1;	/* default to sync to left channel */
    sync_to_right = 0;  /* sync to right channel */
    sync_independant = 0; /* independtant sync */
    use_back_pixmap = 0;/* Backing pixmap disabled (faster with low mem X)*/
    colortab_ready = 0;	/* NOT READY */
    paused = 0;		/* display running */
    low_freq = 0;	/* Low frequency cutoff in hi-res displays */
    high_freq = RATE/2;	/* High frequency cutoff in hi-res displays */
    bandwidth = 22050;	/* frequency bandwidth in hi res modes */
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
    cfgfile = extace_cfg_open_file(filename);
    if (cfgfile)
    {
	extace_cfg_read_int(cfgfile, "Global", "major_ver", &major_ver);
	extace_cfg_read_int(cfgfile, "Global", "minor_ver", &minor_ver);
	extace_cfg_read_int(cfgfile, "Global", "micro_ver", &micro_ver);
	if (major_ver == 0)
	{
	    printf("Config file structure changed. using defaults. \nClosing eXtace will save your NEW settings.\n");
	    extace_cfg_free(cfgfile);
	    unlink(filename);
	    g_free(filename);
	    return;

	}
        extace_cfg_read_string(cfgfile, "Global", "last_colormap", &temp_cmap);
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
	extace_cfg_read_int(cfgfile, "Global", "landtilt", &landtilt);
	extace_cfg_read_int(cfgfile, "Global", "spiketilt", &spiketilt);
	extace_cfg_read_int(cfgfile, "Global", "low_freq", &low_freq);
	extace_cfg_read_int(cfgfile, "Global", "high_freq", &high_freq);
	extace_cfg_read_int(cfgfile, "Window", "width", &width);
	extace_cfg_read_int(cfgfile, "Window", "height", &height);
	extace_cfg_read_int(cfgfile, "Window", "main_x_origin", &main_x_origin);
	extace_cfg_read_int(cfgfile, "Window", "main_y_origin", &main_y_origin);
//	extace_cfg_read_int(cfgfile, "Window", "grad_win_present", &grad_win_present);
	extace_cfg_read_int(cfgfile, "Window", "grad_x_origin", &grad_x_origin);
	extace_cfg_read_int(cfgfile, "Window", "grad_y_origin", &grad_y_origin);
	extace_cfg_read_int(cfgfile, "Window", "dir_x_origin", &dir_x_origin);
	extace_cfg_read_int(cfgfile, "Window", "dir_y_origin", &dir_y_origin);

	extace_cfg_read_int(cfgfile, "Global", "mode", &mode);
	extace_cfg_read_float(cfgfile, "Global", "bandwidth", &bandwidth);
	extace_cfg_read_int(cfgfile, "Global", "sound_source", &sound_source);
	extace_cfg_read_int(cfgfile, "Global", "alsa_card", &alsa_card);
	extace_cfg_read_int(cfgfile, "Global", "alsa_device", &alsa_device);
	extace_cfg_read_int(cfgfile, "Global", "alsa_sub_dev", &alsa_sub_dev);
	extace_cfg_read_int(cfgfile, "Global", "fft_signal_source", &fft_signal_source);
	extace_cfg_read_int(cfgfile, "Global", "refresh_rate", &refresh_rate);
	extace_cfg_read_int(cfgfile, "Global", "landflip", &landflip);
	extace_cfg_read_int(cfgfile, "Global", "spikeflip", &spikeflip);
	extace_cfg_read_int(cfgfile, "Global", "sub_mode_3D", &sub_mode_3D);
	extace_cfg_read_int(cfgfile, "Global", "scope_sub_mode", &scope_sub_mode);
	extace_cfg_read_int(cfgfile, "Global", "dir_win_present", &dir_win_present);
	extace_cfg_read_int(cfgfile, "Global", "nsamp", &nsamp);
	to_get = nsamp;/* nsamp * 2 channels * 2( 16 bit samples)*/
	callback_buffer_size = 4096;
	extace_cfg_read_int(cfgfile, "Global", "window_func", &window_func);
	extace_cfg_read_int(cfgfile, "Global", "winstyle", &winstyle);
	extace_cfg_read_int(cfgfile, "Global", "axis_type", &axis_type);
	extace_cfg_read_int(cfgfile, "Global", "bands", &bands);
	extace_cfg_read_int(cfgfile, "Global", "lag", &lag);
	extace_cfg_read_float(cfgfile, "Global", "scaler", &scaler);
	extace_cfg_read_int(cfgfile, "Global", "use_back_pixmap", &use_back_pixmap);
	extace_cfg_read_int(cfgfile, "Global", "seg_height", &seg_height);
	extace_cfg_read_int(cfgfile, "Global", "seg_space", &seg_space);
	extace_cfg_read_int(cfgfile, "Global", "bar_decay", &bar_decay);
	extace_cfg_read_int(cfgfile, "Global", "peak_decay", &peak_decay);
	extace_cfg_read_int(cfgfile, "Global", "stabilized", &stabilized);
	extace_cfg_read_int(cfgfile, "Global", "show_graticule", &show_graticule);
	extace_cfg_read_int(cfgfile, "Global", "decay_speed", &bar_decay_speed);
	extace_cfg_read_int(cfgfile, "Global", "peak_decay_speed", &peak_decay_speed);
	extace_cfg_read_int(cfgfile, "Global", "peak_hold_time", &peak_hold_time);
	extace_cfg_read_int(cfgfile, "Global", "tape_scroll", &tape_scroll);
	extace_cfg_read_int(cfgfile, "Global", "xdet_scroll", &xdet_scroll);
	extace_cfg_read_int(cfgfile, "Global", "zdet_scroll", &zdet_scroll);
	extace_cfg_read_float(cfgfile, "Global", "xdet_start", &xdet_start);
	extace_cfg_read_float(cfgfile, "Global", "xdet_end", &xdet_end);
	extace_cfg_read_float(cfgfile, "Global", "ydet_start", &ydet_start);
	extace_cfg_read_float(cfgfile, "Global", "ydet_end", &ydet_end);
	extace_cfg_read_float(cfgfile, "Global", "x3d_start", &x3d_start);
	extace_cfg_read_float(cfgfile, "Global", "x3d_end", &x3d_end);
	extace_cfg_read_float(cfgfile, "Global", "y3d_start", &y3d_start);
	extace_cfg_read_float(cfgfile, "Global", "y3d_end", &y3d_end);
	extace_cfg_read_int(cfgfile, "Global", "x3d_scroll", &x3d_scroll);
	extace_cfg_read_int(cfgfile, "Global", "z3d_scroll", &z3d_scroll);
	extace_cfg_read_int(cfgfile, "Global", "show_leader", &show_leader);
	extace_cfg_read_int(cfgfile, "Global", "multiplier", &multiplier);
	extace_cfg_read_int(cfgfile, "Global", "sync_to_left", &sync_to_left);
	extace_cfg_read_int(cfgfile, "Global", "sync_to_right", &sync_to_right);
	extace_cfg_read_int(cfgfile, "Global", "sync_independant", &sync_independant);
	extace_cfg_read_int(cfgfile, "Global", "horiz_spec_start", &horiz_spec_start);
	extace_cfg_read_int(cfgfile, "Global", "vert_spec_start", &vert_spec_start);
	if (horiz_spec_start > width)
	    horiz_spec_start = width-10; 
	if (vert_spec_start > height)
	    vert_spec_start = height-10;
	if (horiz_spec_start < 55)
	    horiz_spec_start = 55; 
	if (vert_spec_start < 120)
	    vert_spec_start = 120;


	extace_cfg_free(cfgfile);

    }
    else
	printf("Config file not found, using defaults\n");
    g_free(filename);

}
void save_config(void)
{
    gchar *filename;
    ConfigFile *cfgfile;
    gint x;
    gint y;
    filename = g_strconcat(g_get_home_dir(), "/.eXtace/config", NULL);
    cfgfile = extace_cfg_open_file(filename);
    if (!cfgfile)
	cfgfile = extace_cfg_new();

    extace_cfg_write_int(cfgfile, "Global", "major_ver", _MAJOR_);
    extace_cfg_write_int(cfgfile, "Global", "minor_ver", _MINOR_);
    extace_cfg_write_int(cfgfile, "Global", "micro_ver", _MICRO_);
    if (Color_map.filename)
	extace_cfg_write_string(cfgfile, "Global", "last_colormap", Color_map.filename);
    else
	extace_cfg_write_string(cfgfile, "Global", "last_colormap",g_strconcat(g_get_home_dir(),"/.eXtace/ColorMaps/","Default",NULL));
    extace_cfg_write_int(cfgfile, "Global", "mode", mode);
    extace_cfg_write_float(cfgfile, "Global", "bandwidth", bandwidth);
    extace_cfg_write_int(cfgfile, "Global", "sound_source", sound_source);
    extace_cfg_write_int(cfgfile, "Global", "alsa_card", alsa_card);
    extace_cfg_write_int(cfgfile, "Global", "alsa_device", alsa_device);
    extace_cfg_write_int(cfgfile, "Global", "alsa_sub_dev", alsa_sub_dev);
    extace_cfg_write_int(cfgfile, "Global", "fft_signal_source", fft_signal_source);
    extace_cfg_write_int(cfgfile, "Global", "refresh_rate", refresh_rate);
    extace_cfg_write_int(cfgfile, "Global", "landflip", landflip);
    extace_cfg_write_int(cfgfile, "Global", "spikeflip", spikeflip);
    extace_cfg_write_int(cfgfile, "Global", "sub_mode_3D", sub_mode_3D);
    extace_cfg_write_int(cfgfile, "Global", "scope_sub_mode", scope_sub_mode);
    extace_cfg_write_int(cfgfile, "Global", "dir_win_present", dir_win_present);
    extace_cfg_write_int(cfgfile, "Global", "nsamp", nsamp);
    extace_cfg_write_int(cfgfile, "Global", "window_func", window_func);
    extace_cfg_write_int(cfgfile, "Global", "winstyle", winstyle);
    extace_cfg_write_int(cfgfile, "Global", "axis_type", axis_type);
    extace_cfg_write_int(cfgfile, "Global", "bands", bands);
    extace_cfg_write_int(cfgfile, "Global", "lag", lag);
    extace_cfg_write_float(cfgfile, "Global", "scaler", scaler);
    extace_cfg_write_int(cfgfile, "Global", "use_back_pixmap", use_back_pixmap);
    extace_cfg_write_int(cfgfile, "Global", "seg_height", seg_height);
    extace_cfg_write_int(cfgfile, "Global", "seg_space", seg_space);
    extace_cfg_write_int(cfgfile, "Global", "bar_decay", bar_decay);
    extace_cfg_write_int(cfgfile, "Global", "peak_decay", peak_decay);
    extace_cfg_write_int(cfgfile, "Global", "stabilized", stabilized);
    extace_cfg_write_int(cfgfile, "Global", "show_graticule", show_graticule);
    extace_cfg_write_int(cfgfile, "Global", "decay_speed", bar_decay_speed);
    extace_cfg_write_int(cfgfile, "Global", "peak_decay_speed", peak_decay_speed);
    extace_cfg_write_int(cfgfile, "Global", "peak_hold_time", peak_hold_time);
    extace_cfg_write_int(cfgfile, "Global", "tape_scroll", tape_scroll);
    extace_cfg_write_int(cfgfile, "Global", "xdet_scroll", xdet_scroll);
    extace_cfg_write_int(cfgfile, "Global", "zdet_scroll", zdet_scroll);
    extace_cfg_write_float(cfgfile, "Global", "xdet_start", xdet_start);
    extace_cfg_write_float(cfgfile, "Global", "xdet_end", xdet_end);
    extace_cfg_write_float(cfgfile, "Global", "ydet_start", ydet_start);
    extace_cfg_write_float(cfgfile, "Global", "ydet_end", ydet_end);
    extace_cfg_write_float(cfgfile, "Global", "x3d_start", x3d_start);
    extace_cfg_write_float(cfgfile, "Global", "x3d_end", x3d_end);
    extace_cfg_write_float(cfgfile, "Global", "y3d_start", y3d_start);
    extace_cfg_write_float(cfgfile, "Global", "y3d_end", y3d_end);
    extace_cfg_write_int(cfgfile, "Global", "horiz_spec_start", horiz_spec_start);
    extace_cfg_write_int(cfgfile, "Global", "vert_spec_start", vert_spec_start);
    extace_cfg_write_int(cfgfile, "Global", "x3d_scroll", x3d_scroll);
    extace_cfg_write_int(cfgfile, "Global", "z3d_scroll", z3d_scroll);
    extace_cfg_write_int(cfgfile, "Global", "show_leader", show_leader);
    extace_cfg_write_int(cfgfile, "Global", "multiplier", multiplier);
    extace_cfg_write_int(cfgfile, "Global", "sync_to_left", sync_to_left);
    extace_cfg_write_int(cfgfile, "Global", "sync_to_right", sync_to_right);
    extace_cfg_write_int(cfgfile, "Global", "sync_independant", sync_independant);
    extace_cfg_write_int(cfgfile, "Global", "landtilt",landtilt);
    extace_cfg_write_int(cfgfile, "Global", "spiketilt", spiketilt);
    extace_cfg_write_int(cfgfile, "Global", "low_freq", low_freq);
    extace_cfg_write_int(cfgfile, "Global", "high_freq", high_freq);
    extace_cfg_write_int(cfgfile, "Window", "width", width);
    extace_cfg_write_int(cfgfile, "Window", "height", height);
    gdk_window_get_root_origin((gpointer) main_win_ptr->window, &x, &y);
    extace_cfg_write_int(cfgfile, "Window", "main_x_origin", x);
    extace_cfg_write_int(cfgfile, "Window", "main_y_origin", y);
//    extace_cfg_write_int(cfgfile, "Window", "grad_win_present", grad_win_present);
    if (grad_win_present)
    {
	gdk_window_get_root_origin((gpointer) grad_win_ptr->window, &x, &y);
	extace_cfg_write_int(cfgfile, "Window", "grad_x_origin", x);
	extace_cfg_write_int(cfgfile, "Window", "grad_y_origin", y);
    }
    if (dir_win_present)
    {
	gdk_window_get_root_origin((gpointer) dir_win_ptr->window, &x, &y);
	extace_cfg_write_int(cfgfile, "Window", "dir_x_origin", x);
	extace_cfg_write_int(cfgfile, "Window", "dir_y_origin", y);
    }

    extace_cfg_write_file(cfgfile, filename);
    extace_cfg_free(cfgfile);

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

    /* Audio block in time domain currently being processed, size of nsamp */
    audio_data = malloc(nsamp*sizeof(gdouble));
    /* Audio block in frequency domain being processed, size of nsamp */
    raw_fft_out = malloc(nsamp*sizeof(gdouble));
    /* datawindow applied to TIME series data, thus need "nsamp" datapoints */
    datawindow = malloc(nsamp*sizeof(gdouble));
    /* FFT after scaling/massaging, size of (nsamp+1)/2 */
    norm_fft = malloc(nsamp*sizeof(gdouble));
    /* Main audio ringbuffer of data after reading interleaved stereo */
    audio_ring = malloc(BUFFER*sizeof(gshort));

    audio_left = malloc(nsamp*sizeof(gshort));
    audio_last_l = malloc(nsamp*sizeof(gshort));
    audio_right = malloc(nsamp*sizeof(gshort));
    audio_last_r = malloc(nsamp*sizeof(gshort));

    /* incoming buf ONLY used for esd, as incoming data amount is unknown
     * when running unlinke ALSA 0.5.x callback
     */
    incoming_buf = malloc(nsamp*2*sizeof(gshort));
    /* Display values of norm_fft, scaled for screen viewing ,
     * for low resolution fft's (LAND_3D) */
    disp_val = malloc(nsamp*sizeof(gint));
    /* array of values for screen viewing after interpolation to fit display
     * width, designed for hi-res fft's 
     * We make the assumption no one has a screen over 8192 pixels wide
     * just in case.. :) Otherwise eXtace will prolly crash
     */

    pip_arr = malloc(8192*sizeof(gint));


	if ((audio_data == NULL)       || (raw_fft_out == NULL) ||
		(start == NULL)	       || (pt2 == NULL)   ||
		(pt3 == NULL)          || (pt4 == NULL)   ||
		(end == NULL)          || 
		(datawindow == NULL)   || (norm_fft == NULL)   ||
		(audio_ring == NULL)   || (incoming_buf == NULL) ||
		(pip_arr == NULL)      || (disp_val == NULL) ||
		(audio_left == NULL)   || (audio_right == NULL) ||
		(audio_last_l == NULL) || (audio_last_r == NULL)) 
	{   
	    g_print("Memory could NOT be allocated!!!!, Exiting now!\n");
	    exit (-2);
	}
    memset((void *)start , 0, nsamp*sizeof(GdkColor));
    memset((void *)pt2 , 0, nsamp*sizeof(GdkColor));
    memset((void *)pt3 , 0, nsamp*sizeof(GdkColor));
    memset((void *)pt4 , 0, nsamp*sizeof(GdkColor));
    memset((void *)end , 0, nsamp*sizeof(GdkColor));

    memset((void *)audio_data , 0, nsamp*sizeof(gdouble));
    memset((void *)raw_fft_out , 0, nsamp*sizeof(gdouble));
    memset((void *)norm_fft , 0, nsamp*sizeof(gdouble));
    memset((void *)datawindow , 0, nsamp*sizeof(gdouble));
    memset((void *)audio_ring, 0, BUFFER*sizeof(gshort));
    memset((void *)incoming_buf, 0, nsamp*2*sizeof(gshort));
    memset((void *)audio_left , 0, nsamp*sizeof(gshort));
    memset((void *)audio_last_l , 0, nsamp*sizeof(gshort));
    memset((void *)audio_right , 0, nsamp*sizeof(gshort));
    memset((void *)audio_last_r , 0, nsamp*sizeof(gshort));
    memset((void *)pip_arr , 0, 8192*sizeof(gint));
    memset((void *)disp_val , 0, nsamp*sizeof(gint));

    /* set pointers to proper values */
    ring_pos = 0;	/* 0 = beginning */
    ring_end = BUFFER; /* endpoint in ELEMENTS, NOT bytes */
}

void mem_dealloc()
{
    free(audio_data);
    free(raw_fft_out);
    free(norm_fft);
    free(datawindow);
    free(audio_ring);
    free(incoming_buf);
    free(audio_left);
    free(audio_last_l);
    free(audio_right);
    free(audio_last_r);
    free(pip_arr);
    free(disp_val);

    free(start);
    free(pt2);
    free(pt3);
    free(pt4);
    free(end);

    if (plan)
	rfftw_destroy_plan(plan);
    convolve_close(l_state); 
    convolve_close(r_state); 
}

void reinit_extace(int new_nsamp)
{
    /* Stop drawing the display */
    draw_stop();

	/* Setting "keep_reading" to 0 causes the audio thread to break from its
	 * loop and exit. We wait for flag to get set and spin till it exits 
	 */
    keep_reading = 0;
    switch (sound_source)
    {
	case ESD:
	    audio_thread_stopper();
	    close_sound();
	    break;
	case ALSA:
	    audio_thread_stopper();
	    close_sound();
	    break;
	default:
	    printf("don't know what sound source we are, ERROR!!!\b\n");
	    break;
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
    fft_lag = 1000*((nsamp/2)/(float)RATE);

    convolve_factor = floor(nsamp/width) < 3 ? floor(nsamp/width) : 3 ;
    if (convolve_factor == 0)
	convolve_factor = 1;
    recalc_markers = 1;
    recalc_scale = 1;	
    mem_alloc();
    setup_datawindow(NULL,(WindowFunction)window_func);
    ring_pos=0;
    if(open_sound() >= 0)
    {
	keep_reading = 1;
	audio_thread_starter();
	draw_start();
    }
}
