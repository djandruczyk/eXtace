/*
 * horiz_specgram.c source file for extace
 * 
 *    /GDK/GNOME sound (esd) system output display program
 * 
 * Based on the original extace written by The Rasterman and Michael Fulbright
 *   
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You may use this program at your own risk.
 */

#include <config.h>
#include <globals.h>
#include <protos.h>
#include <math.h>
#include <gtk/gtk.h>


/* See globals.h for variable declarations and DEFINES */
static gint i=0;
static GdkColor cl;
static gint lvl;


void draw_horiz_specgram()
{
	active_drawing_area = height-time_border;
	gdk_threads_enter();
	if (display_markers)
	{
		update_freq_markers();
		update_time_markers();
		display_markers = 0;
		clear_display = 0;
	}
	if (use_back_pixmap)
	{
		gdk_window_copy_area(main_pixmap,gc,0,0,main_pixmap,
				tape_scroll,0,width-horiz_spec_start,active_drawing_area);
	}
	else
	{
		gdk_window_copy_area(main_display->window,gc,0,0,win,
				tape_scroll,0,width-horiz_spec_start,active_drawing_area);
	}

	reducer(low_freq, high_freq, active_drawing_area);

	for (i=0; i < active_drawing_area; i++)
	{
		lvl=(gint)pip_arr[i]*4;
		if (lvl > (MAXBANDS-1))
			lvl=(MAXBANDS-1);
		cl.pixel=colortab[16][lvl];
		gdk_gc_set_foreground(gc,&cl);

		if (use_back_pixmap)
		{
			gdk_draw_line(main_pixmap,gc,width-horiz_spec_start-tape_scroll, active_drawing_area-i,width-horiz_spec_start,active_drawing_area-i);
		}
		else
		{
			gdk_draw_line(main_display->window,gc,width-horiz_spec_start-tape_scroll, active_drawing_area-i,width-horiz_spec_start,active_drawing_area-i);
		}
	}

	if (use_back_pixmap)
		gdk_window_clear(main_display->window);

	gdk_threads_leave();
}
