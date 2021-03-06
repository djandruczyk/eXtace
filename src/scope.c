/*
 * scope.c source file for extace
 * 
 * Audio visualization
 * 
 * Copyright (C) 1999-2017 by Dave J. Andruczyk 
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
#include <enums.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <math.h>
#include <scope.h>


/* See globals.h for variable declarations and DEFINES */
static gint i=0;
static gint j=0;
static gint idx=0;
static gint height_per_scope=0;
static gint scope_pos_l=0;
static gint scope_pos_r=0;
static gint l_val=0;
static gint r_val=0;
static gfloat left_val=0.0;
static gfloat right_val=0.0;
static gint lo_width=0;
static gint max=0;
static gint right_scope_pos=0;

	/* NOTE to myself.  VERY VERY bad to statically define this.  this WILL
	 * break with a window size larger than 2048 pixels.  (Not possible on
	 * my system, but still possible elsewhere
	 */
static GdkPoint	l_scope_points[2048];
static GdkPoint	r_scope_points[2048];
//static gint top;
//static gint bottom;
gfloat left_amplitude;
gfloat right_amplitude;
extern gint scope_sync_source;



void draw_scope()
{
	gfloat min_zoom = 0.0;
	gint t = 0;
	lo_width = (width < nsamp) ? width : nsamp;
	height_per_scope = height/4;

//		top = (height/4 - 128);
//		if (top < 0)
//			top = 0;
//		bottom = (height-(height/4))+127;
//		if (bottom > height);
//		bottom = height;

	gdk_threads_enter();
		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, 0,0,
				width,height);

	if ((!stabilized) || (nsamp <=1024))
	{
		scope_begin_l = 0;
		scope_begin_r = 0;
	}
	else
	{
		switch((ScopeSyncSource)scope_sync_source)
		{
			case SYNC_LEFT:
				scope_begin_r=scope_begin_l;
				break;
			case SYNC_RIGHT:
				scope_begin_l=scope_begin_r;
				break;
			default:
				break;
		}
	}
	if (show_graticule)
	{
		//max = (height_per_scope < 128) ? height_per_scope : 128;
		max = height/4;
		for (i=0;i<=max;i+=height/16)
		{

			gdk_draw_line(main_pixmap,graticule_gc,\
					0,\
					height-height_per_scope+i,\
					width,\
					height-height_per_scope+i);
			gdk_draw_line(main_pixmap,graticule_gc,\
					0,\
					height-height_per_scope-i,\
					width,\
					height-height_per_scope-i);
			gdk_draw_line(main_pixmap,graticule_gc,\
					0,\
					height_per_scope+i,\
					width,\
					height_per_scope+i);
			gdk_draw_line(main_pixmap,graticule_gc,\
					0,\
					height_per_scope-i,\
					width,\
					height_per_scope-i);
		}
		i-=height/16;

		for (j=0;j<width/2;j+=32)
		{
			gdk_draw_line(main_pixmap,graticule_gc,\
					width/2+j,\
					height_per_scope-i,\
					width/2+j,\
					height_per_scope+i);
			gdk_draw_line(main_pixmap,graticule_gc,\
					width/2-j,\
					height_per_scope-i,\
					width/2-j,\
					height_per_scope+i);
			gdk_draw_line(main_pixmap,graticule_gc,\
					width/2+j,\
					height-height_per_scope-i,\
					width/2+j,\
					height-height_per_scope+i);
			gdk_draw_line(main_pixmap,graticule_gc,\
					width/2-j,\
					height-height_per_scope-i,\
					width/2-j,\
					height-height_per_scope+i);
		}
	}
	//    printf("drawing scope of %i points\n",lo_width);
	t = (nsamp-scope_begin_l) > (nsamp-scope_begin_r) ? nsamp-scope_begin_l:nsamp-scope_begin_r;
	min_zoom = scope_zoom < 2*(float)lo_width/(float)t ? 2*(float)lo_width/(float)t:scope_zoom ;
	scope_zoom = min_zoom;
	
	for(i=0,
			scope_pos_l=scope_begin_l,
			scope_pos_r=scope_begin_r;
			i<(int)((float)lo_width/min_zoom);
			i++,
			scope_pos_l++,
			scope_pos_r++)
	{
		if (scope_pos_l > nsamp)
			printf("scope_pos_left UNDERFLOW!!!!\n");
		if (scope_pos_r > nsamp)
			printf("scope_pos_right UNDERFLOW!!!!\n");

		left_val = audio_left[scope_pos_l]/left_amplitude;

		right_val = audio_right[scope_pos_r]/right_amplitude;

		l_scope_points[i].x = i*min_zoom;
		l_scope_points[i].y = height_per_scope+(left_val*height/4);
		r_scope_points[i].x = i*min_zoom;
		r_scope_points[i].y = height-height_per_scope+(right_val*height/4);
	}
	switch ((ScopeMode)scope_sub_mode)
	{
		case DOT_SCOPE:
			/* Left Channel */
			gdk_draw_points(main_pixmap,\
					trace_gc,\
					l_scope_points,\
					(int)((float)lo_width/min_zoom));
			/* Right Channel */
			gdk_draw_points(main_pixmap,\
					trace_gc,\
					r_scope_points,\
					(int)((float)lo_width/min_zoom));
			break;

		case LINE_SCOPE:
			/* Left Channel */
			gdk_draw_lines(main_pixmap,\
					trace_gc,\
					l_scope_points,\
					(int)((float)lo_width/min_zoom));
			/* RIGHT Channel scope */
			gdk_draw_lines(main_pixmap,\
					trace_gc,\
					r_scope_points,\
					(int)((float)lo_width/min_zoom));
			break;
		case GRAD_SCOPE:

			right_scope_pos = height-height_per_scope;
			for(i=0;i<(int)((float)lo_width/min_zoom);i++)
			{

				l_val = l_scope_points[i].y\
					-height_per_scope;
				r_val = r_scope_points[i].y\
					+height_per_scope-height;
				if (l_val < 0) /* Negative signal */
				{
					idx = 127-(gint)((-l_val/(height/4.0))*127);
					gdk_draw_pixmap(main_pixmap,gc,\
							grad[idx],\
							0,0,\
							i*min_zoom,l_scope_points[i].y,
							1*min_zoom,-l_val);
				}
				else	/* Positive Signal (left channel)*/
				{
					
					idx = 127+(gint)((l_val/(height/4.0))*127);
					gdk_draw_pixmap(main_pixmap,gc,\
							grad[idx],\
							0,0,\
							i*min_zoom,height_per_scope,\
							1*min_zoom,l_val);
				}

				if (r_val < 0) //Negative Signal 
				{
					idx = 127-(gint)((-r_val/(height/4.0))*127);
					gdk_draw_pixmap(main_pixmap,gc,\
							grad[idx],\
							0,0,\
							i*min_zoom,r_scope_points[i].y,\
							
							1*min_zoom,\
							-r_val);
				}
				else // Positive Signal 
				{
					idx = 127+(gint)((r_val/(height/4.0))*127);
					gdk_draw_pixmap(main_pixmap,gc,\
							grad[idx],\
							0,0,\
							i*min_zoom,right_scope_pos,\
							1*min_zoom,r_val);
				}

			}
			break;
		default:
			break;
	}
	for (i=0;i<nsamp;i++)
	{
		audio_last_l[i] =  audio_left[i];       /* copy the arrays */
		audio_last_r[i] =  audio_right[i];      /* copy the arrays */
	}
	for (i=0;i<CONVOLVE_SMALL;i++)
	{
		last_buf_l[i] = (last_buf_l[i]>>1) + cur_buf_l[(i)+scope_begin_l/convolve_factor];
		last_buf_r[i] = (last_buf_r[i]>>1) + cur_buf_r[(i)+scope_begin_r/convolve_factor];
	}

	gdk_window_clear(main_display->window);

	gdk_threads_leave();

}
