/*
 * buttons.c extace source file
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

#include <buttons.h>
#include <config.h>
#include <convolve.h>
#include <datawindow.h>
#include <dir.h>
#include <draw.h>
#include <enums.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <init.h>
#include <logo.xpm>
#include <markers.h>
#include <input.h>
#include <stars.h>
#include <unistd.h>


extern GtkWidget *stars; /* from stars.c */
extern GtkObject *lf_adj;	/* Freq adjustment sliders... */
extern GtkObject *hf_adj;	/* Freq adjustment sliders... */

void leave(GtkWidget *widget, gpointer *data)
{
	draw_stop();
	save_config(widget);
	input_thread_stopper(data_handle);
	close_datasource(data_handle);
	/* Free all buffers */
	mem_dealloc();
	gtk_main_quit();
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
	switch ((Slider)data)
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
//			update_time_markers();
			break;
		case REFRESH_RATE:
			refresh_rate = GTK_ADJUSTMENT(widget)->value;
			draw_stop();
			draw_start();
			break;
		case LOW_LIMIT:
			low_freq = GTK_ADJUSTMENT(widget)->value;
			GTK_ADJUSTMENT(hf_adj)->lower = low_freq + (33*RATE/nsamp);
			gtk_adjustment_changed(GTK_ADJUSTMENT(hf_adj));
			break;
		case HIGH_LIMIT:
			high_freq = GTK_ADJUSTMENT(widget)->value;
			GTK_ADJUSTMENT(lf_adj)->upper = high_freq - (33*RATE/nsamp);
			gtk_adjustment_changed(GTK_ADJUSTMENT(lf_adj));
			break;
		default:
			break;
	}
	return 0;
}

gint fft_set_axis_type(GtkWidget * widget, gpointer *data)
{
	if (GTK_TOGGLE_BUTTON(widget)->active) /* its pressed */
	{
		switch((AxisType)data)
		{
			case LOG:
				axis_type = LOG; 
				recalc_markers = 1;
				break;
			case LINEAR:
				axis_type = LINEAR; 
				recalc_markers = 1;
				break;
		}
	}
	return TRUE;
}

gint scope_sync_source_set(GtkWidget * widget, gpointer *data)
{
	if (GTK_TOGGLE_BUTTON(widget)->active) /* its pressed */
	{
		switch((ScopeSyncSource)data)
		{
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
		}
	}
	return TRUE;
}

gint set_data_source(GtkWidget *widget, gpointer *data)
{
  
  if (GTK_TOGGLE_BUTTON(widget)->active){ /* its pressed */
    draw_stop();
    input_thread_stopper(data_handle);
    close_datasource(data_handle);
    data_source = (DataSource) data;
    /* start even if none previously opened 
       (in case previous sound source was bad) */
    if ((data_handle=open_datasource(data_source)) >= 0)
      {
	input_thread_starter(data_handle);
	draw_start();
      }
  }
  return TRUE;
}

gint set_window_width(GtkWidget *widget, gpointer *data)
{
	if (GTK_TOGGLE_BUTTON(widget)->active) /* its pressed */
	{
		switch((WindowWidth)data)
		{
			case FULL:
				win_width = FULL;
				setup_datawindow(NULL,window_func);
				break;
			case HALF:
				win_width = HALF;
				setup_datawindow(NULL,window_func);
				break;
			case QUARTER:
				win_width = QUARTER;
				setup_datawindow(NULL,window_func);
				break;
			case EIGHTH:
				win_width = EIGHTH;
				setup_datawindow(NULL,window_func);
				break;
		}
	}
	return TRUE;
}

gint set_fft_data_to_display(GtkWidget *widget, gpointer *data)
{
	if (GTK_TOGGLE_BUTTON(widget)->active) /* its pressed */
	{
		switch((FftDataPacking)data)
		{
			case LEFT:
				fft_signal_source=LEFT;
				break;
			case RIGHT:
				fft_signal_source=RIGHT;
				break;
			case LEFT_PLUS_RIGHT:
				fft_signal_source=LEFT_PLUS_RIGHT;
				break;
			case LEFT_MINUS_RIGHT:
				fft_signal_source=LEFT_MINUS_RIGHT;
				break;
		}
	}
	return TRUE;
}
gint set_fft_size(GtkWidget *widget, gpointer *data)
{
	if (GTK_TOGGLE_BUTTON(widget)->active) /* its pressed */
	{
		switch((FftSize)data)
		{
			case S_512:
				reinit_extace(512);
				break;
			case S_1024:
				reinit_extace(1024);
				break;
			case S_2048:
				reinit_extace(2048);
				break;
			case S_4096:
				reinit_extace(4096);
				break;
			case S_8192:
				reinit_extace(8192);
				break;
			case S_16384:
				reinit_extace(16384);
				break;
			case S_32768:
				reinit_extace(32768);
				break;
		}
	}
	return TRUE;
}

gint button_handle(GtkWidget *widget, gpointer *data)
{
	if (GTK_TOGGLE_BUTTON(widget)->active) /* its pressed */
	{
		switch((ToggleButton)data)
		{
			case OPTIONS:
				gtk_widget_show(options_win_ptr);
				break;

			case LEADING_EDGE:

				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Leading Edge Hidden");
				show_leader = 0;
				break;
			case USE_BAR_DECAY:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Bar Decay Enabled");
				bar_decay = 1;
				break;
			case USE_PEAK_DECAY:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Peak Decay Enabled");
				peak_decay = 1;
				break;
			case OUTLINED:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Outlined 3D Landform");
				outlined = TRUE;
				break;
			case STABILIZED:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Trace Stabilizer Enabled");
				stabilized = 1;
				break;
			case GRATICULE:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Scope Graticule Enabled");
				show_graticule = 1;
				break;
			case LAND_PERS_TILT:
				landtilt = 1; 
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Landform Perspective Tilt Enabled");
				break;
			case SPIKE_PERS_TILT:
				spiketilt = 1; 
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Spikes Perspective Tilt Enabled");
				break;
			case LANDFLIP:
				landflip = -1;
				break;

			case SPIKEFLIP:
				spikeflip = -1;
				break;

			case PAUSE_DISP:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Resume Display");
				paused = 1;
				draw_stop();
				break;

			default:
				break;
		}
	}
	else
	{
		switch((ToggleButton)data)
		{
			case OPTIONS:
				gtk_widget_hide(options_win_ptr);
				break;

			case LEADING_EDGE:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Leading Edge Shown");
				show_leader = 1;
				break;
			case USE_BAR_DECAY:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Bar Decay Disabled");
				bar_decay = 0;
				break;
			case USE_PEAK_DECAY:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Peak Decay Disabled");
				peak_decay = 0;
				break;
			case OUTLINED:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Smooth 3D Landform");
				outlined = FALSE;
				break;
			case STABILIZED:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
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
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
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
			case LAND_PERS_TILT:
				landtilt = 0; 
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Landform Perspective Tilt Disabled");
				break;
			case SPIKE_PERS_TILT:
				spiketilt = 0; 
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
						"Spikes Perspective Tilt Disabled");
				break;
			case LANDFLIP:
				landflip = 1;
				break;
			case SPIKEFLIP:
				spikeflip = 1;
				break;
			case PAUSE_DISP:
				gtk_label_set_text(GTK_LABEL(GTK_BIN 
						(widget)->child),
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
	
gint change_display_mode(GtkWidget *widget, gpointer *data)
{
	gint enable_dir_win = 0;

	draw_stop();	/* stop the display */
	/* Clear the screen to remove stray stuff */
	gdk_draw_rectangle(main_pixmap,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);
	gdk_draw_rectangle(main_display->window,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);
	gdk_window_clear(main_display->window);
	switch ((DisplayMode)data)
	{
		case WIRE_3D:
			mode = LAND_3D;
			sub_mode_3D = WIRE_3D;
			enable_dir_win = 1;
			break;
		case FILL_3D:
			mode = LAND_3D;
			sub_mode_3D = FILL_3D;
			enable_dir_win = 1;
			break;
		case EQ_2D:
			mode = EQ_2D;
			enable_dir_win = 0;
			break;
		case SCOPE:
			mode = SCOPE;
			enable_dir_win = 0;
			break;
		case HORIZ_SPECGRAM:
			mode = HORIZ_SPECGRAM;
			if (horiz_spec_start < 60)
				horiz_spec_start = 60;
			if (horiz_spec_start > width)
				horiz_spec_start = width-10;
			enable_dir_win = 0;
			display_markers = 1;
			break;
		case VERT_SPECGRAM:
			mode = VERT_SPECGRAM;
			if (vert_spec_start > height)
				vert_spec_start = height-10;
			if (vert_spec_start < 120)
				vert_spec_start = 120;
			display_markers = 1;
			break;
		case SPIKE_3D:
			mode = SPIKE_3D;
			enable_dir_win = 1;
			break;
		case STARS:
			mode = STARS;
			enable_dir_win = 0;
			if (!stars)
			{
				GdkPixmap *pm = NULL, *mk = NULL;
				stars = (GtkWidget *)kt_stars_new(
						main_display,
						main_pixmap);
				pm = gdk_pixmap_create_from_xpm_d(
						stars->
						window, &mk, NULL, 
						logo_xpm);
				kt_stars_set_logo_pixmp(stars, pm, mk);
			}
			break;
		default:
			break;
	}
	if (enable_dir_win)	/* enable direction control widget */
	{
		if (!dir_win_present)
		{
			gtk_widget_show(dir_win);
			gtk_widget_set_uposition(dir_win,
					dir_x_origin,dir_y_origin);
			dir_win_present = 1;
		}
		update_dircontrol(dir_area);
	}
	else			/* if enabled, disable it */
	{
		if (dir_win_present)
		{
			gtk_widget_hide(dir_win);
			dir_win_present = 0;
		}
	}
	draw_start();

	return TRUE;

}

gint set_decimation_factor(GtkWidget *widget, gpointer *data)
{
	if (GTK_TOGGLE_BUTTON (widget)->active)
	{
		if (((long int)data > 0) && ((long int)data <= 16))
		{
			decimation_factor = (long int)data;
			recalc_markers=1;
			GTK_ADJUSTMENT(lf_adj)->upper = high_freq - (33.0*(float)RATE/(2.0*decimation_factor));
			GTK_ADJUSTMENT(hf_adj)->upper = (float)RATE/(2.0*decimation_factor)+RATE/nsamp;
			gtk_adjustment_changed(GTK_ADJUSTMENT(lf_adj));
			gtk_adjustment_changed(GTK_ADJUSTMENT(hf_adj));

		}
	}
	return (0);
}

gint scope_mode(GtkWidget *widget, gpointer *data)
{
	switch((ScopeMode)data)
	{
		case DOT_SCOPE:
			scope_sub_mode = (ScopeMode)data;
			break;
		case LINE_SCOPE:
			scope_sub_mode = (ScopeMode)data;
			break;
		case GRAD_SCOPE:
			scope_sub_mode = (ScopeMode)data;
			break;
	}
	if (mode == SCOPE)
	{
		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, 0,0,
				width,height);
		gdk_window_clear(main_display->window);
	}
	return 0;
}

