/*
 * vert_specgram.c source file for extace
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
static gint count=0;


void draw_vert_specgram()
{
	count++;
	active_drawing_area = width-time_border;
	gdk_threads_enter();
	if (display_markers)
	{
		clear_display = 0;
		update_time_markers();
		update_freq_markers();
		display_markers = 0;
	}
	if (count >= 30)
	{
		display_markers=1;
		count=0;
	}
	gdk_window_copy_area(drawable,gc,
			0,0,
			drawable,
			0,tape_scroll,
			active_drawing_area,
			height-vert_spec_start);
	gdk_draw_rectangle(drawable,main_display->style->black_gc,
			TRUE,
			0,	
			height-vert_spec_start+50,	
			width,
			vert_spec_start-50);

	reducer(low_freq, high_freq, active_drawing_area);

	cl.pixel=colortab[16][0];
	gdk_gc_set_foreground(gc,&cl);
	gdk_draw_rectangle(drawable,gc,
			TRUE,
			0,
			height-vert_spec_start-tape_scroll,
			active_drawing_area,
			tape_scroll);
	gdk_draw_line(drawable,gc,
			0,
			height-y_border,
			active_drawing_area,
			height-y_border);

	for (i=0; i < (active_drawing_area); i++)
	{
		lvl=(gint)pip_arr[i]*4;
		if (lvl > (MAXBANDS-1))
			lvl=(MAXBANDS-1);

		cl.pixel=colortab[16][lvl];

		gdk_gc_set_foreground(gc,&cl);

		gdk_draw_line(drawable,gc,
				i,
				height-vert_spec_start-tape_scroll,
				i,
				height-vert_spec_start);

		gdk_draw_line(drawable,gc,
				i,
				height-y_border,
				i,
				height-y_border-pip_arr[i]);
	}

	if (use_back_pixmap)
		gdk_window_clear(main_display->window);

	gdk_threads_leave();
}
