/*
 * spike_3d.c source file for extace
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
#include <reducer.h>
#include <spike_3d.h>


/* See globals.h for variable declarations and DEFINES */
static gint i=0;
static gfloat dir_angle_rad = 0.0;
static gfloat dir_angle_deg = 0.0;
static gfloat det_axis_angle_deg = 0.0;
static gfloat x_axis_weight = 0.0;
static gfloat y_axis_weight = 0.0;
static gfloat x_tilt_weight = 0.0;
static gfloat y_tilt_weight = 0.0;
static gfloat x_amplitude = 0.0;
static gfloat y_amplitude = 0.0;
static gfloat loc_bins_per_pip=0;
static gint start_x = 0;
static gint end_x = 0;
static gint start_y = 0;
static gint end_y = 0;
static gint axis_length=0;
static GdkPoint pt[4];
static GdkColor cl;
static gint lvl;
static gint x_draw_width = 0;
static gint y_draw_height = 0;
static gfloat x_axis_tilt = 0.0;
static gfloat y_axis_tilt = 0.0;
static gfloat x_tilt = 0.0;
static gfloat y_tilt = 0.0;

gfloat xdet_start;
gfloat xdet_end;
gfloat ydet_start;
gfloat ydet_end;
gint xdet_scroll;    /* 3D spike scroll in pixels */
gint zdet_scroll;    /* 3D spike scroll in pixels */






void draw_spike_3d()
{
	extern gfloat det_axis_angle;
	gdk_threads_enter();
	gdk_window_copy_area(main_pixmap,gc,
			0,0,
			main_pixmap,
			xdet_scroll,
			zdet_scroll,
			width-xdet_scroll,
			height-zdet_scroll);
	if (xdet_scroll > 0)
	{
		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, width-xdet_scroll,0,
				xdet_scroll,height);
	}
	else
	{
		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, 0,0,
				abs(xdet_scroll),height);
	}

	if (zdet_scroll > 0)
	{
		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, 0,height-zdet_scroll,
				width,zdet_scroll);
	}
	else
	{
		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, 0,0,
				width,abs(zdet_scroll));
	}
	gdk_threads_leave();

	/* in pixels */
	x_draw_width = width - abs(xdet_scroll)-2*border;
	y_draw_height = height - abs(zdet_scroll)-2*border;

	/* in pixels */
	x_offset = (width-x_draw_width)/2;
	y_offset = (height-y_draw_height)/2;


	det_axis_angle_deg = det_axis_angle*90/M_PI_2;
	if (det_axis_angle_deg < 0.0)
		det_axis_angle_deg += 360.0;

	dir_angle_rad = (float)atan2((float)zdet_scroll,-(float)xdet_scroll);
	dir_angle_deg = dir_angle_rad*90/M_PI_2;
	if (dir_angle_deg < 0.0)
		dir_angle_deg += 360.0;


	if (spiketilt == FALSE)
	{
		x_tilt=0.0;
		y_tilt=0.0;
	}
	else
	{
		/* Perspecitve Tilting algorithm.  Probably severely flawed
		 * but it displays nicely..  
		 * 
		 * Note to self.  Learn how to use Trig properly... :)
		 */ 
		if ((dir_angle_deg >= 315.0) || (dir_angle_deg < 45.0))
		{
			//printf("Quad 4<->1\n");
			x_tilt = sin(dir_angle_rad);
			y_tilt = cos(dir_angle_rad);
		}
		else if ((dir_angle_deg >= 45.0) && (dir_angle_deg < 135.0))
		{
			//printf("Quad 1<->2\n");
			x_tilt = cos(dir_angle_rad);
			y_tilt = sin(dir_angle_rad);
		}
		else if ((dir_angle_deg >= 135.0) && (dir_angle_deg < 225.0))
		{
			//printf("Quad 2<->3\n");
			x_tilt = -sin(dir_angle_rad);
			y_tilt = -cos(dir_angle_rad);
		}
		else if  ((dir_angle_deg >= 225.0) && (dir_angle_deg < 315.0))
		{
			//printf("Quad 3<->4\n");
			x_tilt = -cos(dir_angle_rad);
			y_tilt = -sin(dir_angle_rad);
		}
	}

	x_axis_tilt = sin(det_axis_angle);
	y_axis_tilt = cos(det_axis_angle);

	x_tilt_weight = cos(det_axis_angle);
	y_tilt_weight = 0.0;
	x_axis_weight = 1.0;
	y_axis_weight = 1.0;

/*
	// Debugging Code....  

	printf("axis angle in deg is %f\n",det_axis_angle_deg);
	printf("direction angle in deg is %f\n",dir_angle_deg);
	printf("\nx_tilt %f, y_tilt %f\n",x_tilt,y_tilt);
	printf("x_tilt_weight %f, y_tilt_weight %f\n",x_tilt_weight,y_tilt_weight);
	printf("Scroll Tilt components (%f,%f)\n",x_tilt*x_tilt_weight,y_tilt*y_tilt_weight);
	printf("x_axis_tilt %f, y_axis_tilt %f\n",x_axis_tilt,y_axis_tilt);
	printf("x_axis_weight %f, y_axis_weight %f\n",x_axis_weight,y_axis_weight);
	printf("Perspective Tilt components (%f,%f)\n\n",x_axis_tilt*x_axis_weight,y_axis_tilt*y_axis_weight);
	
*/

	/* in pixels */
	start_x = width-((x_draw_width)*(1.0-xdet_end))-x_offset;
	end_x = width-((x_draw_width)*(1.0-xdet_start))-x_offset;
	start_y = height-((y_draw_height)*(1.0-ydet_end))-y_offset;
	end_y = height-((y_draw_height)*(1.0-ydet_start))-y_offset;

	/* in pixels */
	start_x = width-((x_draw_width)*(1.0-xdet_end))-x_offset;
	end_x = width-((x_draw_width)*(1.0-xdet_start))-x_offset;
	start_y = height-((y_draw_height)*(1.0-ydet_end))-y_offset;
	end_y = height-((y_draw_height)*(1.0-ydet_start))-y_offset;
	/* end_x - start_x is X screen space in pixels 
	 * the idea is to reduce the number of gdk_draw_lines to 
	 * the number in axis_length, instead of nsamp/2. which saves drawing 
	 * over the same location on the screen and wasting time.
	 * I guess when combining pips together we should AVG them. What
	 * other way might be better? (suggestions welcome...)
	 */

	/* disp_val[] is a malloc'd array storing pip values for ALL pips, 
	 * (1/2 NSAMP). pip_arr[] is malloc'd array that is length 
	 * "axis_length"and contains anti-aliased reduction of 
	 * disp[val] using linear interpolation to combine/average 
	 * multiple pip values into one viewable spike. Should be 
	 * fully scaling with best results of having bins_per_pip 
	 * being a multiple of nsamp/2.
	 */
	axis_length = (gint)sqrt(((end_x-start_x)*(end_x-start_x))+((end_y-start_y)*(end_y-start_y)));
	loc_bins_per_pip = ((float)nsamp/2.0)/(fabs(axis_length));

	reducer(low_freq, high_freq, axis_length);
	gdk_threads_enter();

	/* Do this here instead of in the loop  axis_length number of times..
	 * cpu savings... :)
	 */
	x_amplitude = (x_tilt*x_tilt_weight)+(x_axis_tilt*x_axis_weight);
	y_amplitude = (y_tilt*y_tilt_weight)+(y_axis_tilt*y_axis_weight);

	for( i=0; i < axis_length; i++ )
	{
		pt[0].x=width-(((i*x_draw_width)*(1-xdet_start))/axis_length)\
				-((((axis_length-i)*x_draw_width)*(1-xdet_end))\
				/(axis_length))-x_offset;
		pt[0].y=height-(((i*loc_bins_per_pip)*y_draw_height*ydet_start)\
				/(nsamp/2))-((((nsamp/2)-(i*loc_bins_per_pip))\
				*y_draw_height*ydet_end)/(nsamp/2))\
				-y_offset;
		pt[1].x=pt[0].x -(gint)pip_arr[i]*x_amplitude;
		pt[1].y=pt[0].y -(gint)pip_arr[i]*y_amplitude; \
		lvl=abs((gint)pip_arr[i]*4);
		if (lvl > (MAXBANDS-1))
			lvl=(MAXBANDS-1);
		else if (lvl <= 0)
			lvl = 0;

		cl.pixel=colortab[16][lvl];
		gdk_gc_set_foreground(gc,&cl);

		gdk_draw_line(main_pixmap,gc,\
				pt[0].x,\
				pt[0].y,\
				pt[1].x,\
				pt[1].y);
	}
	gdk_window_clear(main_display->window);

	gdk_threads_leave();
}
