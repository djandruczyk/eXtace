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

#include <config.h>
#include <globals.h>
#include <protos.h>
#include <math.h>
#include <gtk/gtk.h>
#include <string.h>


/* See globals.h for variable declarations and DEFINES */
static gint i=0;
static GdkColor cl;

void draw_2d_eq()
{
	gint xdraw_width = width-2*x_border;
	gint lwidth = (int)(xdraw_width/bands);
	gint line_width = 0;
	gfloat count = 0;
	gint peak_spot=0;
	gfloat frag = ((float)xdraw_width/(float)bands) - (float)lwidth;
	gint fragcount = 0;
	gint bar_start=0;         /* start position for bars on graphic EQ */
	gint pos = x_border;
	gchar buff[20];

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

	maxlevel = ((maxlevel*(height))/128)+y_border+seg_height;

	gdk_threads_enter();

	gdk_draw_rectangle(main_pixmap,
			main_display->style->black_gc,
			TRUE, 0,height-maxlevel,
			width,maxlevel);

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
		cl.pixel=colortab[32][(int)(((float)pos)*((float)MAXBANDS/(float)width))];
		gdk_gc_set_foreground(gc,&cl);
		bar_start = height -y_border- (((gint)levels[i]*height)/128);
		peak_spot = height -y_border- (((gint)trailers[i]*height)/128);
		gdk_draw_rectangle(main_pixmap,gc,
				TRUE,
				pos,bar_start,
				line_width-1,(((gint)levels[i]*height)/128));
		if (peak_decay)
		{
			if (ptrailers[i] > 0)
			{
				gdk_draw_rectangle(main_pixmap,gc,
						TRUE,
						pos,peak_spot-seg_height,
						line_width-1,seg_height);
			}
		}

		pos += line_width;

	}
	for(i=y_border;i<maxlevel;i+=seg_height)
	{
		gdk_draw_rectangle(main_pixmap,main_display->style->black_gc,
				TRUE,
				0,height-i,
				width,seg_space);
	}
	gdk_draw_rectangle(main_pixmap,main_display->style->black_gc,
			TRUE,
			xdraw_width*0.66,0,
			xdraw_width*0.33,10);

	g_snprintf(buff,20,"%f hertz", freq_at_pointer);
	gdk_draw_text(main_pixmap,main_display->style->font,
			main_display->style->white_gc,
			xdraw_width*.66,10,
			buff,
			strlen(buff));


	gdk_window_clear_area(main_display->window,0,height-maxlevel,width,maxlevel);
	gdk_window_clear_area(main_display->window,xdraw_width*0.66,0,xdraw_width*0.33,10);
	gdk_threads_leave();
}
