/*
 * markers.C extace source file
 * 
 * /GDK/GNOME sound (esd) system output display program
 * 
 * Copyright (C) 1999 by Dave J. Andruczyk 
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
#include <globals.h>
#include <protos.h>
#include <gtk/gtk.h>
#include <string.h>


void update_time_markers()
{
	gint x=0;
	gint y=0;
	gint start=0;
	gint space=0;
	if (mode == HORIZ_SPECGRAM)
	{
		start = width-horiz_spec_start;
		space = (gint)((((float)(RATE)/(float)nsamp))*2.0*(float)tape_scroll);
		if (nsamp < 4096)
			space = space/2.0;

		gdk_draw_rectangle(drawable,
				main_display->style->black_gc,
				TRUE, 0,height-time_border,
				width-horiz_spec_start,time_border);

		for (x=start; x > 0; x-= space)
		{
			gdk_draw_line(drawable,main_display->style->white_gc,
					x,height-time_border+3,x,height-time_border/2);
			for (y=1; y < 10;y++)
			{
				gdk_draw_line(drawable,main_display->style->white_gc,
						x-(y*space/10),height-time_border+2,x-(y*space/10),height-time_border+6);
			}
		}
	}
	else if (mode == VERT_SPECGRAM)
	{
		start = height-vert_spec_start;
		space = (gint)((((float)(RATE)/(float)nsamp))*2.0*(float)tape_scroll);
		if (nsamp < 4096)
			space = space/2.0;
		gdk_draw_rectangle(drawable,
				main_display->style->black_gc,
				TRUE, width-time_border,0,
				time_border,height-vert_spec_start);

		for (y=start; y > 0; y-= space)
		{
			gdk_draw_line(drawable,main_display->style->white_gc,
					width-time_border+3,y,width-time_border/2,y);
			for (x=1; x < 10;x++)
			{
				gdk_draw_line(drawable,main_display->style->white_gc,
						width-time_border+2,y-(x*space/10),width-time_border+6,y-(x*space/10));
			}
		}
	}
	if(use_back_pixmap)
		gdk_window_clear(main_display->window);
}

void buffer_area_update(void)
{
	gchar buff[60];
	gint x;

	g_snprintf(buff,60,"Incoming audio position in buffer");
	x = gdk_text_width(buffer_area->style->font,
			buff,
			strlen(buff))/2;
	gdk_draw_text(buffer_pixmap,buffer_area->style->font,
			buffer_area->style->white_gc,
			buffer_area->allocation.width/2-x,12,
			buff,
			strlen(buff));

	g_snprintf(buff,60,"Position in Audio Buffer when DRAWING");
	x = gdk_text_width(buffer_area->style->font,
			buff,
			strlen(buff))/2;
	gdk_draw_text(buffer_pixmap,buffer_area->style->font,
			buffer_area->style->white_gc,
			buffer_area->allocation.width/2-x,57,
			buff,
			strlen(buff));
	g_snprintf(buff,60,"Incoming audio position in buffer");
	x = gdk_text_width(buffer_area->style->font,
			buff,
			strlen(buff))/2;
	gdk_draw_text(buffer_pixmap,buffer_area->style->font,
			buffer_area->style->white_gc,
			buffer_area->allocation.width/2-x,12,
			buff,
			strlen(buff));
}

void update_freq_markers()
{
	int bord =0;
	int l_length = 10;
	int i = 0;
	gchar buff[10];
	gchar less_markers = 0;
	gchar lot_less_markers = 0;
	gint x1,x2,y1,y2;
	if(!ready)
		return;
	if (clear_display == 1)
	{
		gdk_draw_rectangle(drawable,
				main_display->style->black_gc,
				TRUE, 0,0,
				width,height);
	}

	if (mode == HORIZ_SPECGRAM)
	{
		if (bandwidth_change)
		{
			bord = width-horiz_spec_start + 5;
			bandwidth_change = 0;
		}
		else
		{
			bord = width-horiz_spec_start + 5;
		}

		if (!clear_display)
		{
			gdk_draw_rectangle(drawable,
					main_display->style->black_gc,
					TRUE,bord-5,0,
					2*l_length+35,height);
		}

		if (height < 350)
			less_markers = 1;
		for (i = 1;i <= (int)(bandwidth/1000);i++) 
		{
			if ((less_markers) && ((i+1)%2))
			{
				continue;
			}
			x1 = bord;
			y1 = (height - time_border) - ((float)i/(bandwidth/1000.0) * (height-time_border));
			x2 = x1 + l_length;
			y2 = y1;

			gdk_draw_line(drawable,main_display->style->white_gc,
					x1,y1,x2,y2);

			g_snprintf(buff,10,"%i kHz",i);
			x2 += 3;
			y2 += gdk_text_height(main_display->style->font,
					buff,
					strlen(buff))/2;
			gdk_draw_text(drawable,main_display->style->font,
					main_display->style->white_gc,
					x2,y2,
					buff,
					strlen(buff));
		}
	}
	else if (mode == VERT_SPECGRAM)
	{
		if (bandwidth_change)
		{
			bord = height-vert_spec_start + 5;
			bandwidth_change = 0;
		}
		else
		{
			bord = height-vert_spec_start + 5;
		}
		if (!clear_display)
		{
			gdk_draw_rectangle(drawable,
					main_display->style->black_gc,
					TRUE,0,bord-5,
					width,2*l_length+35);
		}
		if (width < 820)
			less_markers = 1;
		if (width < 410)
			lot_less_markers = 1;
		for (i = 1; i <= (bandwidth/1000);i++) 
		{
			if ((less_markers) && ((i+1)%2))
			{
				continue;
			}
			if ((lot_less_markers) && ((i+1)%4))
			{
				continue;
			}
			x1 = ((float)i/(bandwidth/1000.0)*(width-time_border));
			y1 = bord;
			x2 = x1;
			y2 = y1+l_length;

			gdk_draw_line(drawable,main_display->\
					style->white_gc,\
					x1,y1,x2,y2);

			g_snprintf(buff,10,"%i kHz",i);
			x2 -= 16;
			y2 = y1+l_length + 2*gdk_text_height(main_display->\
					style->font,
					buff,
					strlen(buff));
			gdk_draw_text(drawable,main_display->style->font,
					main_display->style->white_gc,
					x2,y2,
					buff,
					strlen(buff));
			y2 += gdk_text_height(main_display->style->font,
					buff,strlen(buff));
			gdk_draw_line(drawable,main_display->style->white_gc,
					x1,y2,x1,y2+l_length);

		}
	}
	if (use_back_pixmap)
		gdk_window_clear(main_display->window);
}
