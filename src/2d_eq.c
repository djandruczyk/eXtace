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

#include <2d_eq.h>
#include <config.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>


/* See globals.h for variable declarations and DEFINES */
static gint i=0;
gint seg_height;     /* height per segment in 2d spectrum analyzer */
gint seg_space;      /* space between segments in 2d analyzer */
static gint maxlevel;
static gint prevlevel;


void draw_2d_eq()
{
	gint xdraw_width = width-2*border;
	gint lwidth = (int)(xdraw_width/bands);
	gint line_width = 0;
	gfloat count = 0;
	gint peak_spot=0;
	gfloat frag = ((float)xdraw_width/(float)bands) - (float)lwidth;
	gint fragcount = 0;
	gint bar_start=0;         /* start position for bars on graphic EQ */
	gint pos = border;
	gchar *buff;
	static GdkGC * trail_gc = NULL;
	extern PangoLayout *layout;
	extern PangoFontDescription *font_desc;
	PangoRectangle ink_rect;
	PangoRectangle log_rect;

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
		gdk_gc_set_foreground(gc,&colortab[32][(int)(((float)pos)*((float)MAXBANDS/(float)width))]);
		gdk_gc_set_foreground(trail_gc,&colortab[64][(int)(((float)pos)*((float)MAXBANDS/(float)width))]);

		bar_start = height - border - (((gint)levels[i]*height)/128);
		peak_spot = height - border - (((gint)trailers[i]*height)/128);
		gdk_draw_rectangle(main_pixmap,gc,
				TRUE,
				pos,bar_start,
				line_width-1,(((gint)levels[i]*height)/128));
		if (peak_decay)
		{
			if (ptrailers[i] > 0)
			{
				gdk_draw_rectangle(main_pixmap,trail_gc,
						TRUE,
						pos,peak_spot-seg_height,
						line_width-1,seg_height);
			}
		}

		pos += line_width;

	}
	for(i=border;i<maxlevel;i+=seg_height)
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

	buff = g_strdup_printf("%.2f hertz", freq_at_pointer);
	pango_layout_set_font_description(layout,font_desc);
	pango_layout_set_text(layout,buff,-1);
	pango_layout_get_pixel_extents(layout,&ink_rect,&log_rect);
	gdk_draw_layout(main_pixmap,main_display->style->white_gc,
			xdraw_width*.66,10,layout);
	g_free(buff);

	/* Area freq display text */
	gdk_window_clear_area(main_display->window,xdraw_width*0.66,10,xdraw_width*0.33,log_rect.height);
	/* Main area, only clear up to what's necessary for speed */
	gdk_window_clear_area(main_display->window,0,height-maxlevel,width,maxlevel);
	gdk_threads_leave();
}
