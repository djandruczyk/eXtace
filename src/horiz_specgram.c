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
#include <gtk/gtk.h>
#include <horiz_specgram.h>
#include <math.h>
#include <markers.h>
#include <reducer.h>


/* See globals.h for variable declarations and DEFINES */
static gint i=0;
static GdkColor cl;
static gint lvl;


void draw_horiz_specgram()
{
	active_drawing_area = height-(2*border);
	gdk_threads_enter();
	if (display_markers)
	{
                update_freq_markers();
                clear_display = 0;
                display_markers = TRUE;
	}
	gdk_window_copy_area(main_pixmap,gc,
			0,border,
			main_pixmap,
			tape_scroll,border,
			width-horiz_spec_start,
			active_drawing_area);

	reducer(low_freq, high_freq, active_drawing_area);

	for (i=0; i < active_drawing_area; i++)
	{
		lvl=(gint)pip_arr[i]*4;
		if (lvl > (MAXBANDS-1))
			lvl=(MAXBANDS-1);
		cl.pixel=colortab[16][lvl];
		gdk_gc_set_foreground(gc,&cl);

		gdk_draw_line(main_pixmap,gc,
				width-horiz_spec_start-tape_scroll, 
				active_drawing_area-i+border,
				width-horiz_spec_start,
				active_drawing_area-i+border);
	}

	gdk_window_clear(main_display->window);

	gdk_threads_leave();
}
