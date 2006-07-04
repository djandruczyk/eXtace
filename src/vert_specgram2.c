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
#include <gtk/gtk.h>
#include <math.h>
#include <markers.h>
#include <reducer.h>
#include <vert_specgram.h>


/* See globals.h for variable declarations and DEFINES */
static gint i=0;
static GdkColor cl;
static gint lvl;
static gint count=0;


void draw_vert_specgram2()
{
	static gint x_begin = 0;
	static gint x_end = 0;
	static gint y_begin = 0;
	static gint y_end = 0;


	count++;
	active_drawing_area = width-(border*2);
	gdk_threads_enter();
	if (display_markers)
	{
		update_freq_markers();
		clear_display = FALSE;
		display_markers = FALSE;
	}
	if (count >= 30)
	{
		display_markers=TRUE;
		count=0;
	}
	gdk_window_copy_area(main_pixmap,gc,
			border,0,
			main_pixmap,
			border,tape_scroll,
			active_drawing_area,
			height-vert_spec_start);

	gdk_draw_rectangle(main_pixmap,main_display->style->black_gc,
			TRUE,
			0,	
			height-vert_spec_start+50,	
			width,
			vert_spec_start-50);

	reducer(low_freq, high_freq, active_drawing_area);

	cl.pixel=colortab[16][0];
	gdk_gc_set_foreground(gc,&cl);
	gdk_draw_rectangle(main_pixmap,gc,
			TRUE,
			border,
			height-vert_spec_start-tape_scroll,
			active_drawing_area,
			tape_scroll);


	for (i=0; i < (active_drawing_area); i++)
	{
		//lvl=(gint)pip_arr[i]*4;
		if (i > 0)
			lvl=((gint)pip_arr[i-1]*4+(gint)pip_arr[i]*4)/2;
		else
			lvl=(gint)pip_arr[i]*4;
		if (lvl > (MAXBANDS-1))
			lvl=(MAXBANDS-1);

		cl.pixel=colortab[16][lvl];

		gdk_gc_set_foreground(gc,&cl);

		gdk_draw_line(main_pixmap,gc,
				i+border,
				height-vert_spec_start-tape_scroll,
				i+border,
				height-vert_spec_start);

		if (i==0)
		{
			x_begin = i+border;
			y_begin = height-border; // ZERO point
		}
		else
		{
			x_begin = (i-1)+border;
			y_begin = y_end;
		}

		x_end = i+border;
		y_end = height-border-pip_arr[i];

		gdk_draw_line(main_pixmap,gc,
				x_begin,
				y_begin,
				x_end,
				y_end);


	}
	gdk_window_clear(main_display->window);

	gdk_threads_leave();
}
