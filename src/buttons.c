/*
 * BUTTONS.C extace source file
 * 
 /GDK/GNOME sound (esd) system output display program
 * 
 * Copyright (C) 1999 by Dave J. Andruczyk 
 * 
 * Based on the original extace written by The Rasterman and Michael Fulbright
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
#include <unistd.h>
#include <gtk/gtk.h>
#ifdef HAVE_LIBRFFTW
#include <rfftw.h>
#endif
#include "logo.xpm"
#include "convolve.h"


void leave(GtkWidget *widget, gpointer *data)
{
    draw_stop();
    save_config();
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
    }

    /* Free all buffers */
    mem_dealloc();
    gtk_main_quit();
}

gint button_options(GtkWidget *widget, gpointer *data)
{
    if (GTK_TOGGLE_BUTTON (widget)->active)
    {                                           /* button's down */
	gtk_widget_show(options_win_ptr);
    }
    else
    {
	gtk_widget_hide(options_win_ptr);
    }
    return 0;
}
gint close_dir_win(GtkWidget *widget, gpointer *data)
{
    if (dir_win_present)
    {
	gtk_widget_hide(dir_win);
	dir_win_present = 0;
    }
    return TRUE;
}

gint close_grad_win(GtkWidget *widget, gpointer *data)
{
    if (grad_win_present)
    {
	gtk_widget_hide(grad_win_ptr);
	grad_win_present = 0;
    }
    return TRUE;
}

gint close_options(GtkWidget *widget, gpointer *data)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(optionsbut), FALSE);
    return TRUE;
}

gint slider_changed(GtkWidget *widget, gpointer *data)
{
    switch ((gint)data)
    {
	case BANDS:
	    bands = GTK_ADJUSTMENT(widget)->value;
	    recalc_scale = 1;		/* MUST recaluclate scalefactor */
	    recalc_markers = 1;		/* recalculate marker values */
	    break;
	case SENSITIVITY:
	    multiplier = GTK_ADJUSTMENT(widget)->value;
	    break;
	case BAR_DECAY:
	    bar_decay_speed = GTK_ADJUSTMENT(widget)->value;
	    break;
	case PEAK_DECAY:
	    peak_decay_speed = GTK_ADJUSTMENT(widget)->value;
	    break;
	case PEAK_HOLD:
	    peak_hold_time = GTK_ADJUSTMENT(widget)->value;
	    break;
	case LAG:
	    lag = (float)GTK_ADJUSTMENT(widget)->value;
	    break;
	case NOISE_FLOOR:
	    noise_floor = (float)GTK_ADJUSTMENT(widget)->value;
	    break;
	case TAPE_SCROLL:
	    tape_scroll = GTK_ADJUSTMENT(widget)->value;
	    update_time_markers();
	    break;
	case REFRESH_RATE:
	    refresh_rate = GTK_ADJUSTMENT(widget)->value;
	    draw_stop();
	    draw_start();
	    break;
	default:
	    break;
    }
    return 0;
}
gint change_fftlen(GtkWidget *widget, gpointer *data)
{
    int temp_nsamp = 0;
    
    switch ((gint)data)
    {
	case 256:
	    temp_nsamp=(gint)data;
	    break;
	case 512:
	    temp_nsamp=(gint)data;
	    break;
	case 1024:
	    temp_nsamp=(gint)data;
	    break;
	case 2048:
	    temp_nsamp=(gint)data;
	    break;
	case 4096:
	    temp_nsamp=(gint)data;
	    break;
	case 8192:
	    temp_nsamp=(gint)data;
	    break;
	case 16384:
	    temp_nsamp=(gint)data;
	    break;
	case 32768:
	    temp_nsamp=(gint)data;
	    break;
    }
    return 0;
}
gint button_handle(GtkWidget *widget, gpointer *data)
{
    if (GTK_TOGGLE_BUTTON(widget)->active) /* its pressed */
    {
	switch((gint)data)
	{
	    case LEADING_EDGE:

		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			      "Leading Edge Hidden");
		show_leader = 0;
		break;
	    case BAR_DECAY:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			      "Bar Decay Enabled");
		bar_decay = 1;
		break;
	    case PEAK_DECAY:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			      "Peak Decay Enabled");
		peak_decay = 1;
		break;
	    case STABLE:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			      "Trace Stabilizer Enabled");
		stabilized = 1;
		break;
	    case GRATICULE:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			      "Scope Graticule Enabled");
		show_graticule = 1;
		break;
	    case BACK_PIXMAP:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			      "Backing Pixmap Enabled");
		use_back_pixmap = 1;
		gdk_draw_rectangle(main_display->window,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);
		gdk_draw_rectangle(main_pixmap,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);
		if ((mode == HORIZ_SPECGRAM) || (mode == VERT_SPECGRAM))
		{
		    display_markers = 1;
		}

		break;
	    case LOG:
		axis_type = LOG; 
		recalc_markers = 1;
		break;
	    case LINEAR:
		axis_type = LINEAR; 
		recalc_markers = 1;
		break;
	    case SYNC_LEFT:
		sync_to_left = 1;
		sync_to_right = 0;
		sync_independant = 0;
		/* gotta clear the screen to prevent old data from
		 * laying around....
		 */
		gdk_draw_rectangle(main_display->window,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);
		break;
	    case SYNC_RIGHT:
		sync_to_left = 0;
		sync_to_right = 1;
		sync_independant = 0;
		/* gotta clear the screen to prevent old data from
		 * laying around....
		 */
		gdk_draw_rectangle(main_display->window,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);
		break;
	    case SYNC_INDEP:
		sync_to_left = 0;
		sync_to_right = 0;
		sync_independant = 1;
		/* gotta clear the screen to prevent old data from
		 * laying around....
		 */
		gdk_draw_rectangle(main_display->window,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);
		break;
	    case LAND_PERS_TILT:
		landtilt = 1; 
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Landform Perspective Tilt Enabled");
		break;
	    case SPIKE_PERS_TILT:
		spiketilt = 1; 
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Spikes Perspective Tilt Enabled");
		break;
	    case LANDFLIP:
		landflip = -1;
		break;

	    case SPIKEFLIP:
		spikeflip = -1;
		break;

	    case ESD:
		keep_reading = 0;
	        audio_thread_stopper();
//		usleep(2000);
		close_sound();
		sound_source = ESD;
		ring_pos=0;
		if (open_sound() >= 0)
		{
		    keep_reading = 1;
		    audio_thread_starter();
		}
		break;
	    case ALSA:
		keep_reading = 0;
	        audio_thread_stopper();
//		usleep(2000);
		close_sound();
		sound_source = ALSA;
		ring_pos=0;
		if (open_sound() >= 0)
		{
		    keep_reading = 1;
		    audio_thread_starter();
		}
		break;
	    case LEFT:
		fft_signal_source=LEFT;
		break;
	    case RIGHT:
		fft_signal_source=RIGHT;
		break;
	    case COMPOSITE:
		fft_signal_source=COMPOSITE;
		break;
	    case 512:
		reinit_extace(512);
		break;
	    case 1024:
		reinit_extace(1024);
		break;
	    case 2048:
		reinit_extace(2048);
		break;
	    case 4096:
		reinit_extace(4096);
		break;
	    case 8192:
		reinit_extace(8192);
		break;
	    case 16384:
		reinit_extace(16384);
		break;
	    case 32768:
		reinit_extace(32768);
		break;
	    case FULL:
		winstyle = FULL;
		setup_datawindow(NULL,window_func);
		break;
	    case HALF:
		winstyle = HALF;
		setup_datawindow(NULL,window_func);
		break;
	    case QUARTER:
		winstyle = QUARTER;
		setup_datawindow(NULL,window_func);
		break;
	    case EIGHTH:
		winstyle = EIGHTH;
		setup_datawindow(NULL,window_func);
		break;
	    case PAUSE_DISP:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Resume Display");
		paused = 1;
		draw_stop();
		break;
	    case 5512:
		bandwidth = 5512.5;
		update_freq_markers();
		update_time_markers();
		break;
	    case 11025:
		bandwidth = 11025;
		update_freq_markers();
		update_time_markers();
		break;
	    case 22050:
		bandwidth = 22050;
		update_freq_markers();
		update_time_markers();
		break;

	    default:
		break;
	}
    }
    else
    {
	switch((gint)data)
	{
	    case LEADING_EDGE:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Leading Edge Shown");
		show_leader = 1;
		break;
	    case BAR_DECAY:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Bar Decay Disabled");
		bar_decay = 0;
		break;
	    case PEAK_DECAY:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Peak Decay Disabled");
		peak_decay = 0;
		break;
	    case STABLE:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Trace Stabilizer Disabled");
		stabilized = 0;
		/* gotta clear the screen to prevent old data from
		 * laying around....
		 */
		gdk_draw_rectangle(main_display->window,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);

		break;
	    case GRATICULE:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Scope Graticule Disabled");
		show_graticule = 0;
		/* gotta clear the screen to prevent old data from
		 * laying around....
		 */
		gdk_draw_rectangle(main_display->window,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);

		break;
	    case BACK_PIXMAP:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Backing Pixmap Disabled");
		use_back_pixmap = 0;
		gdk_draw_rectangle(main_display->window,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);
		gdk_draw_rectangle(main_pixmap,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);
		if ((mode == HORIZ_SPECGRAM) || (mode == VERT_SPECGRAM))
		{
		    display_markers = 1;
		}

		break;
	    case LAND_PERS_TILT:
		landtilt = 0; 
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Landform Perspective Tilt Disabled");
		break;
	    case SPIKE_PERS_TILT:
		spiketilt = 0; 
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Spikes Perspective Tilt Disabled");
		break;
	    case LANDFLIP:
		landflip = 1;
		break;
	    case SPIKEFLIP:
		spikeflip = 1;
		break;
	    case PAUSE_DISP:
		gtk_label_set_text(GTK_LABEL(GTK_BIN (widget)->child),
			"Pause Display");
		paused = 0;
		draw_start();
		break;
	    default:
		break;
	}

    }
    return 0;
}
	

gint button_3d_fft(GtkWidget *widget, gpointer *data)
{
    draw_stop();

    gdk_draw_rectangle(main_pixmap,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_draw_rectangle(main_display->window,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_window_clear(main_display->window);
    mode = LAND_3D;
    if (data == (gpointer)FILL_3D)
	sub_mode_3D=(gint)data;
    if (data == (gpointer)WIRE_3D)
	sub_mode_3D=(gint)data;

    if (!dir_win_present)
    {
	gtk_widget_show(dir_win);
	gtk_widget_set_uposition(dir_win,dir_x_origin,dir_y_origin);
	dir_win_present = 1;
    }
    update_dircontrol(dir_area);

    draw_start();

    return 0;
}

gint button_2d_fft(GtkWidget *widget, gpointer *data)
{
    draw_stop();

    gdk_draw_rectangle(main_pixmap,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_draw_rectangle(main_display->window,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_window_clear(main_display->window);
    mode = EQ_2D;
    if(dir_win_present)
    {
	gtk_widget_hide(dir_win);
	dir_win_present = 0;
    }
    draw_start();

    return 0;
}

gint button_oscilloscope(GtkWidget *widget, gpointer *data)
{
    draw_stop();

    gdk_draw_rectangle(main_pixmap,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_draw_rectangle(main_display->window,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_window_clear(main_display->window);
    mode = SCOPE;

    if(dir_win_present)
    {
	gtk_widget_hide(dir_win);
	dir_win_present = 0;
    }
    draw_start();


    return 0;
}
gint scope_mode(GtkWidget *widget, gpointer *data)
{
    if (data == (gpointer)DOT_SCOPE)
	scope_sub_mode = (gint)data;
    if (data == (gpointer)LINE_SCOPE)
	scope_sub_mode = (gint)data;
    if (data == (gpointer)GRAD_SCOPE)
	scope_sub_mode = (gint)data;

    if (mode == SCOPE)
    {
	gdk_draw_rectangle(main_pixmap,
		main_display->style->black_gc,
		TRUE, 0,0,
		width,height);
	gdk_draw_rectangle(main_display->window,
		main_display->style->black_gc,
		TRUE, 0,0,
		width,height);
	gdk_window_clear(main_display->window);
    }
    return 0;
}

gint button_horiz_specgram(GtkWidget *widget, gpointer *data)
{
    draw_stop();

    gdk_draw_rectangle(main_pixmap,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_draw_rectangle(main_display->window,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_window_clear(main_display->window);
    mode = HORIZ_SPECGRAM;
    if (horiz_spec_start < 55)
	horiz_spec_start = 55;
    if (horiz_spec_start > width)
	horiz_spec_start = width-10;
    if(dir_win_present)
    {
	gtk_widget_hide(dir_win);
	dir_win_present = 0;
    }
    display_markers = 1;
    draw_start();

    return 0;
}
gint button_vert_specgram(GtkWidget *widget, gpointer *data)
{
    draw_stop();

    gdk_draw_rectangle(main_pixmap,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_draw_rectangle(main_display->window,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_window_clear(main_display->window);
    mode = VERT_SPECGRAM;
    if (vert_spec_start > height)
	vert_spec_start = height-10;
    if (vert_spec_start < 120)
	vert_spec_start = 120;
    if(dir_win_present)
    {
	gtk_widget_hide(dir_win);
	dir_win_present = 0;
    }
    display_markers = 1;
    draw_start();

    return 0;
}

gint button_3d_detailed(GtkWidget *widget, gpointer *data)
{
    draw_stop();

    gdk_draw_rectangle(main_pixmap,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_draw_rectangle(main_display->window,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_window_clear(main_display->window);
    mode = SPIKE_3D;
    if (!dir_win_present)
    {
	gtk_widget_show(dir_win);
	gtk_widget_set_uposition(dir_win,dir_x_origin,dir_y_origin);
	dir_win_present = 1;
    }
    update_dircontrol(dir_area);

    draw_start();
    return 0;
}

gint button_about(GtkWidget *widget, gpointer *data)
{
    draw_stop();

    gdk_draw_rectangle(main_pixmap,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_draw_rectangle(main_display->window,
	    main_display->style->black_gc,
	    TRUE, 0,0,
	    width,height);
    gdk_window_clear(main_display->window);
    mode=STARS;
    if (!stars)
    {
	GdkPixmap *pm = NULL, *mk = NULL;

	stars = (GtkWidget *)kt_stars_new(main_display, main_pixmap);
	pm = gdk_pixmap_create_from_xpm_d(stars->window, &mk, NULL, logo_xpm);
	kt_stars_set_logo_pixmp(stars, pm, mk);
    }
    if(dir_win_present)
    {
	gtk_widget_hide(dir_win);
	dir_win_present = 0;
    }
    draw_start();
    return 0;
}

