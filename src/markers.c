/*
 * markers.C extace source file
 * 
 * Audio visualization
 * 
 * Copyright (C) 1999-2017 by Dave J. Andruczyk 
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
#include <enums.h>
#include <input.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <markers.h>
#include <string.h>


void buffer_area_update(void)
{
	extern PangoLayout *layout;
	extern PangoFontDescription *font_desc;
	PangoRectangle ink_rect;
	PangoRectangle log_rect;

	gchar *buff;

	buff = g_strdup("Incoming audio position in buffer");
	pango_layout_set_font_description(layout,font_desc);
	pango_layout_set_text(layout,buff,-1);
        pango_layout_get_pixel_extents(layout,&ink_rect,&log_rect);
	gdk_draw_layout(buffer_pixmap,buffer_area->style->white_gc,
			buffer_area->allocation.width/2-(log_rect.width/2),
			2,layout);
	g_free(buff);


	buff = g_strdup("Position in Audio Buffer when DRAWING");
	pango_layout_set_text(layout,buff,-1);
        pango_layout_get_pixel_extents(layout,&ink_rect,&log_rect);
	gdk_draw_layout(buffer_pixmap,buffer_area->style->white_gc,
			buffer_area->allocation.width/2-(log_rect.width/2),
			47,layout);
	g_free(buff);
}

void update_freq_markers()
{
	int bord =0;
	int l_length = 10;
	int i = 0;
	gint num_markers = 0;
	gint x1,x2,y1,y2;
	const gint pixels_per_vmarker = 35;
	const gint pixels_per_hmarker = 60;
	extern gint ready;
	gfloat freq_mark;
	gchar *buff;
	extern PangoLayout *layout;
	extern PangoFontDescription *font_desc;
	PangoRectangle ink_rect;
	PangoRectangle log_rect;
	pango_layout_set_font_description(layout,font_desc);

	if(!ready)
		return;
	if (clear_display)
	{
		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, 0,0,
				width,height);
	}

	if (mode == HORIZ_SPECGRAM)
	{
		bord = width-horiz_spec_start + 5;

		if (!clear_display)
		{
			gdk_draw_rectangle(main_pixmap,
					main_display->style->black_gc,
					TRUE,bord-5,0,
					width-bord-5,height);
		}

		num_markers = (height-(border*2))/pixels_per_vmarker;
		for (i=0;i<=num_markers;i++) 
		{
			x1 = bord;
			y1 = (((float)i/(float)num_markers)*active_drawing_area)+border;
			x2 = x1 + l_length;
			y2 = y1;

			gdk_draw_line(main_pixmap,main_display->style->white_gc,
					x1,y1,x2,y2);
			freq_mark = (((float)(num_markers-i)/(float)num_markers)*(high_freq-low_freq))+(low_freq/1);

			if (freq_mark > 1000)
				buff = g_strdup_printf("%.1f Khz",freq_mark/1000.0);
			else
				buff = g_strdup_printf("%.1f Hz",freq_mark);
			
			x2 += 3;
			pango_layout_set_text(layout,buff,-1);
       			pango_layout_get_pixel_extents(layout,&ink_rect,&log_rect);
			y2 -= log_rect.height/2;
			gdk_draw_layout(main_pixmap,
					main_display->style->white_gc,
					x2,y2,layout);
			g_free(buff);

		}
	}
	else if ((mode == VERT_SPECGRAM) || (mode == VERT_SPECGRAM2))
	{
		bord = height-vert_spec_start + 5;
		if (!clear_display)
		{
			gdk_draw_rectangle(main_pixmap,
					main_display->style->black_gc,
					TRUE,0,bord-5,
					width,2*l_length+35);
		}
		num_markers = (width-(border*2))/pixels_per_hmarker;
		for (i=0; i<=num_markers;i++) 
		{
			x1 = (((float)i/(float)num_markers)*active_drawing_area)+border;
			y1 = bord;
			x2 = x1;
			y2 = y1+l_length;

			gdk_draw_line(main_pixmap,main_display->\
					style->white_gc,\
					x1,y1,x2,y2);

			freq_mark = (((float)i/(float)num_markers)*(high_freq-low_freq))+low_freq;
			if (freq_mark > 1000)
				buff = g_strdup_printf("%.1f Khz",freq_mark/1000.0);
			else
				buff = g_strdup_printf("%.1f Hz",freq_mark);
			x2 -= 16;
			pango_layout_set_text(layout,buff,-1);
       			pango_layout_get_pixel_extents(layout,&ink_rect,&log_rect);
			y2 = y1+l_length + log_rect.height/2;

			gdk_draw_layout(main_pixmap,
					main_display->style->white_gc,
					x2,y2,layout);
			g_free(buff);
			y2 += 1.5*log_rect.height;
			gdk_draw_line(main_pixmap,main_display->style->white_gc,
					x1,y2,x1,y2+l_length);

		}
	}
	gdk_window_clear(main_display->window);
}
