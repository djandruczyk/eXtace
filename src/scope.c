/*
 * scope.c source file for extace
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
#include <scope.h>


/* See globals.h for variable declarations and DEFINES */
static gint i=0;
static gint j=0;
static gint height_per_scope=0;
static gint old_left_val=0;
static gint old_right_val=0;
static gint old_scope_pos_l=0;
static gint scope_pos_l=0;
static gint old_scope_pos_r=0;
static gint scope_pos_r=0;
static gint left_val=0;
static gint right_val=0;
static gint lo_width=0;
static gint max=0;
static gint right_scope_pos=0;

	/* NOTE to myself.  VERY VERY bad to statically define this.  this WILL
	 * break with a window size larger than 2048 pixels.  (Not possible on
	 * my system, but still possible elsewhere
	 */
static GdkPoint	l_scope_points[2048];
static GdkPoint	r_scope_points[2048];
static GdkPoint	l_scope_points_last[2048];
static GdkPoint	r_scope_points_last[2048];
static gint top;
static gint bottom;
gfloat left_amplitude;
gfloat right_amplitude;



void draw_scope()
{
	lo_width = (width < nsamp) ? width : nsamp;
	height_per_scope = height/4;

	gdk_threads_enter();
		top = (height/4 - 128);
		if (top < 0)
			top = 0;
		bottom = (height-(height/4))+127;
		if (bottom > height);
		bottom = height;

		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, 0,top,
				width,bottom);

	if ((!stabilized) || (nsamp <=1024))
	{
		scope_begin_l = old_scope_begin_l = 0;
		scope_begin_r = old_scope_begin_r = 0;
	}
	else
	{
		if (sync_to_left)
		{
			scope_begin_r=scope_begin_l;
			old_scope_begin_r=old_scope_begin_l;
		}
		else if (sync_to_right)
		{
			scope_begin_l=scope_begin_r;
			old_scope_begin_l=old_scope_begin_r;
		}
	}
	if (show_graticule)
	{
		max = (height_per_scope < 128) ? height_per_scope : 128;
		for (i=0;i<=max;i+=32)
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
		i-=32;

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
	for(i=0,
			scope_pos_l=scope_begin_l,
			scope_pos_r=scope_begin_r,
			old_scope_pos_l=old_scope_begin_l,
			old_scope_pos_r=old_scope_begin_r;
			i<lo_width;
			i++,
			scope_pos_l++,
			scope_pos_r++,
			old_scope_pos_l++,
			old_scope_pos_r++)
	{
		/* if (old_scope_pos_l > 2046)
		 * g_print("WARNING to close to array boundary!!!!\n");
		 * if (old_scope_pos_r > 2046)
		 * g_print("WARNING to close to array boundary!!!!\n");
		 */
		if (scope_pos_l > nsamp)
			printf("scope_pos_left OVERFLOW!!!!\n");
		if (scope_pos_r > nsamp)
			printf("scope_pos_right OVERFLOW!!!!\n");
		old_left_val=(gint)(audio_last_l[old_scope_pos_l]\
				*left_amplitude);

		left_val=(gint)(audio_left[scope_pos_l]*left_amplitude);

		old_right_val=(gint)(audio_last_r[old_scope_pos_r]\
				*right_amplitude);

		right_val=(gint)(audio_right[scope_pos_r]*right_amplitude);

		if (left_val < -127)
		{
			left_val = -127;
			old_left_val = -127;
		}
		else if (left_val > 127)
		{
			left_val = 127;
			old_left_val = 127;
		}

		if (right_val < -127)
		{
			right_val = -127;
			old_right_val = -127;
		}
		else if (right_val > 127)
		{
			right_val = 127;
			old_right_val = 127;
		}


		l_scope_points[i].x = i;
		l_scope_points[i].y = height_per_scope+left_val;
		r_scope_points[i].x = i;
		r_scope_points[i].y = height-height_per_scope+right_val;
		l_scope_points_last[i].x = i;
		l_scope_points_last[i].y = height_per_scope+old_left_val;
		r_scope_points_last[i].x = i;
		r_scope_points_last[i].y = height-height_per_scope\
			+old_right_val;
	}
	switch (scope_sub_mode)
	{
		case DOT_SCOPE:
			/* Left Channel */
			gdk_draw_points(main_pixmap,\
					trace_gc,\
					l_scope_points,\
					lo_width);
			/* Right Channel */
			gdk_draw_points(main_pixmap,\
					trace_gc,\
					r_scope_points,\
					lo_width);
			break;

		case LINE_SCOPE:
			/* Left Channel */
			gdk_draw_lines(main_pixmap,\
					trace_gc,\
					l_scope_points,\
					lo_width);
			/* RIGHT Channel scope */
			gdk_draw_lines(main_pixmap,\
					trace_gc,\
					r_scope_points,\
					lo_width);
			break;
	}


	if (scope_sub_mode== GRAD_SCOPE)
	{
		right_scope_pos = height-height_per_scope;
		for(i=0;i<lo_width;i++)
		{

			left_val = l_scope_points[i].y\
				-height_per_scope;
			right_val = r_scope_points[i].y\
				+height_per_scope-height;
			if (left_val == 0)
				left_val++;
			if (right_val == 0)
				right_val++;
			if (left_val < 0) /* Negative signal */
			{
				gdk_draw_pixmap(main_pixmap,gc,\
						grad[left_val+127],\
						0,0,\
						i,l_scope_points[i].y,\
						1,-left_val);
			}
			else	/* Positive Signal (left channel)*/
			{
				gdk_draw_pixmap(main_pixmap,gc,\
						grad[left_val+127],\
						0,0,\
						i,height_per_scope,\
						1,left_val);
			}

			if (right_val < 0) /*Negative Signal */
			{
				gdk_draw_pixmap(main_pixmap,gc,\
						grad[right_val+127],\
						0,0,\
						i,right_scope_pos\
						+right_val,\
						1,\
						-right_val);
			}
			else /* Positive Signal */
			{
				gdk_draw_pixmap(main_pixmap,gc,\
						grad[right_val+127],\
						0,0,\
						i,right_scope_pos,\
						1,right_val);
			}

		}
	}
	for (i=0;i<nsamp;i++)
	{
		audio_last_l[i] =  audio_left[i];       /* copy the arrays */
		audio_last_r[i] =  audio_right[i];      /* copy the arrays */
	}
	old_scope_begin_l = scope_begin_l;
	old_scope_begin_r = scope_begin_r;
	for (i=0;i<CONVOLVE_SMALL;i++)
	{
		last_buf_l[i] = (last_buf_l[i]>>1) + cur_buf_l[(i)+scope_begin_l/convolve_factor];
		last_buf_r[i] = (last_buf_r[i]>>1) + cur_buf_r[(i)+scope_begin_r/convolve_factor];
	}

	gdk_window_clear(main_display->window);

	gdk_threads_leave();

}
