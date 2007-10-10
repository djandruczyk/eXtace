/*
 * 2d_eq.c source file for extace
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

#include <line_eq.h>
#include <config.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>


/* See globals.h for variable declarations and DEFINES */
static gint i=0;
static GdkColor cl;
gint seg_height;     /* height per segment in 2d spectrum analyzer */
gint seg_space;      /* space between segments in 2d analyzer */
static gint maxlevel;
static gint prevlevel;


void draw_line_eq()
{
	gint xdraw_width = width-2*border;
	gint lwidth = (int)(xdraw_width/bands);
	gint line_width = 0;
	gint last_line_width = 0;
	gfloat count = 0;
//	gint peak_spot=0;
	gfloat frag = ((float)xdraw_width/(float)bands) - (float)lwidth;
	gint fragcount = 0;
	gint pos = border;
	gint x_begin = 0;
	gint y_begin = 0;
	gint y_pbegin = 0;
	gint x_end = 0;
	gint y_end = 0;
	gint y_pend = 0;
	gchar *buff;
	static GdkGC *trail_gc = NULL;

	if (!trail_gc)
	{
		trail_gc = gdk_gc_new(main_display->window);
		gdk_gc_copy(trail_gc,main_display->style->white_gc);
	}


	active_drawing_area = xdraw_width;

	if (frag == 0.0)
		frag += .00001;

	maxlevel = prevlevel = 0;
	for (i=0;i < bands; i++)
	{
		if (peak_decay == 1)
		{
			if (peak_decay_speed > bar_decay_speed)
			{
				if (plevels[i] > prevlevel)
					prevlevel = (gint)plevels[i];
				if (levels[i] > maxlevel)
					maxlevel = (gint)levels[i];
			}
			else
			{
				if (ptrailers[i] > prevlevel)
					prevlevel = (gint)ptrailers[i];
				if (trailers[i] > maxlevel)
					maxlevel = (gint)trailers[i];
			}
		}
		else
		{
			if (plevels[i] > prevlevel)
				prevlevel = (gint)plevels[i];
			if (levels[i] > maxlevel)
				maxlevel = (gint)levels[i];
		}
	}
	if (prevlevel > maxlevel)
		maxlevel = prevlevel;

	maxlevel = ((maxlevel*(height))/128)+border+seg_height;

	gdk_threads_enter();

	gdk_draw_rectangle(main_pixmap,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);

	for( i=0; i < bands; i++ )
	{ 
		count = count + frag;
		if (count > 1.0)
		{
			count -= 1.0;
			line_width = lwidth+1;
			fragcount ++;
		}
		else
		{
			line_width = lwidth;
		}
		cl.pixel=colortab[64][(int)(((float)pos)*((float)MAXBANDS/(float)width))];
		gdk_gc_set_foreground(gc,&cl);
		cl.pixel=colortab[32][(int)(((float)pos)*((float)MAXBANDS/(float)width))];
		gdk_gc_set_foreground(trail_gc,&cl);


		if (i == 0)
		{
			x_begin = pos;
			y_begin = height-border;
			y_pbegin = height-border;
		}
		else
		{
			x_begin = pos + border;
			y_begin = y_end;
			y_pbegin = y_pend;
		}

		pos += line_width;
		x_end = pos + border;
		y_end = height-border-(((gint)levels[i]*height)/128);
		y_pend = height-border-(((gint)trailers[i]*height)/128);

		last_line_width = line_width;


		gdk_draw_line(main_pixmap,gc,
				x_begin,y_begin,
				x_end,y_end);

		if (peak_decay)
		{
			gdk_draw_line(main_pixmap,trail_gc,
				x_begin,y_pbegin,
				x_end,y_pend);
		/*	if (ptrailers[i] > 0)
			{
				gdk_draw_point(main_pixmap,gc,
						pos,peak_spot);
			}
			*/
		}


	}
	gdk_draw_rectangle(main_pixmap,main_display->style->black_gc,
			TRUE,
			xdraw_width*0.66,0,
			xdraw_width*0.33,10);

	buff = g_strdup_printf("%.2f hertz", freq_at_pointer);
	gdk_draw_text(main_pixmap,main_display->style->font,
			main_display->style->white_gc,
			xdraw_width*.66,10,
			buff,
			strlen(buff));
	g_free(buff);


	//gdk_window_clear_area(main_display->window,0,height-maxlevel,width,maxlevel);
	gdk_window_clear_area(main_display->window,0,0,width,height);
	gdk_window_clear_area(main_display->window,xdraw_width*0.66,0,xdraw_width*0.33,10);
	gdk_threads_leave();
}
