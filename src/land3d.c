/*
 * draw.c source file for extace
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


/* See globals.h for variable declarations and DEFINES */
static gint i=0;
static GdkPoint pt[4];
static GdkPoint cap_pt[4];
static GdkPoint oldpt[4];
static GdkPoint lpt[4];
static gint br,lvl;
static GdkColor cl;
static gfloat land_axis_angle_deg = 0.0;
static gfloat dir_angle_rad = 0.0;
static gfloat dir_angle_deg = 0.0;
static gint x_draw_width = 0;
static gint y_draw_height = 0;
static gint x_fudge = 0;
static gfloat x_axis_tilt = 0.0;
static gfloat y_axis_tilt = 0.0;
static gfloat x_axis_weight = 0.0;
static gfloat y_axis_weight = 0.0;
static gfloat x_tilt = 0.0;
static gfloat y_tilt = 0.0;
static gfloat x_tilt_weight = 0.0;
static gfloat y_tilt_weight = 0.0;
static gint x_shift = 0;
static gint x_shift_per_block = 0;
static gint y_shift_per_block = 0;
static gfloat x_amplitude = 0.0;
static gfloat y_amplitude = 0.0;


gfloat x3d_start;      /* The 3D X start point of axis (percentage) */
gfloat y3d_start;      /* The 3D Y start point of axis (percentage) */
gfloat x3d_end;        /* 3D fft X end point of axis (percentage) */
gfloat y3d_end;        /* 3D fft Y end point of axis (percentage) */
gint x3d_scroll;
gint z3d_scroll;


void draw_land3d_fft()
{
	extern gfloat land_axis_angle;
	gdk_threads_enter();
	/* Scroll the window */
	gdk_window_copy_area(main_pixmap,gc,0,0,main_pixmap,
			x3d_scroll,z3d_scroll,width-x3d_scroll,height-z3d_scroll);
	if (x3d_scroll > 0)
	{
		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, width-x3d_scroll,0,
				x3d_scroll,height);
	}
	else
	{
		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, 0,0,
				abs(x3d_scroll),height);
	}

	if (z3d_scroll > 0)
	{
		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, 0,height-z3d_scroll,
				width,z3d_scroll);
	}
	else
	{
		gdk_draw_rectangle(main_pixmap,
				main_display->style->black_gc,
				TRUE, 0,0,
				width,abs(z3d_scroll));
	}
	gdk_threads_leave();

	land_axis_angle_deg = land_axis_angle*90/M_PI_2; 
	if (land_axis_angle_deg < 0.0)
		land_axis_angle_deg += 360;

	dir_angle_rad = (float)atan2((float)z3d_scroll, -(float)x3d_scroll);
	dir_angle_deg = dir_angle_rad*90/M_PI_2;
	if (dir_angle_deg < 0.0)
		dir_angle_deg += 360;


	if (landtilt == 0) /* Perspective tilting disabled */
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

	/*
	 * pixel shift for scalability
	 *
	 * need to recalculate these, for borders so we don't get "black
	 * spots" on the display from running offscreen
	 * x_draw_width = width - "some magic amount"
	 * Smoothing routine.  Adjusts border slightly to avoid 2 pixel
	 * breakups in the display... (woohoo, it actually works!!)
	 * pix_per_block = floating point!!!
	 */


	x_draw_width = width - abs(x3d_scroll)-2*x_border;
	y_draw_height = height - abs(z3d_scroll)-2*y_border;

	x_shift = ((x_draw_width*(1-x3d_start))-(x_draw_width*(1-x3d_end)));
	x_shift_per_block = x_shift/bands;

	x_offset = (width-x_draw_width)/2;
	y_offset = (height-y_draw_height)/2;
	if(x3d_start < x3d_end)
		x_fudge = 1;
	else x_fudge = -1;

	y_shift_per_block = z3d_scroll/2;

	x_axis_tilt = sin(land_axis_angle);
	y_axis_tilt = cos(land_axis_angle);

	x_tilt_weight = cos(land_axis_angle);
        y_tilt_weight = 0.0;
        x_axis_weight = 1.0;
        y_axis_weight = 1.0;

/*
        // Debugging Code....  

        printf("axis angle in deg is %f\n",land_axis_angle_deg);
        printf("direction angle in deg is %f\n",dir_angle_deg);
        printf("\nx_tilt %f, y_tilt %f\n",x_tilt,y_tilt);
        printf("x_tilt_weight %f, y_tilt_weight %f\n",x_tilt_weight,y_tilt_weight);
        printf("Scroll Tilt components (%f,%f)\n",x_tilt*x_tilt_weight,y_tilt*y_tilt_weight);
        printf("x_axis_tilt %f, y_axis_tilt %f\n",x_axis_tilt,y_axis_tilt);
        printf("x_axis_weight %f, y_axis_weight %f\n",x_axis_weight,y_axis_weight);
        printf("Perspective Tilt components (%f,%f)\n\n",x_axis_tilt*x_axis_weight,y_axis_tilt*y_axis_weight);
        
*/

	gdk_threads_enter();
	if (((x3d_scroll > 0) && (x_fudge == 1)) || ((x3d_scroll < 0) && (x_fudge == -1)))
		draw_land3d_forward();
	else
		draw_land3d_reverse();
	gdk_window_clear(main_display->window);
	gdk_threads_leave();



}



void draw_land3d_forward()
{
//printf("land3d forward\n");
//printf("x_tilt %f, y_tilt %f\n",x_tilt,y_tilt);
//printf("xaxis_tilt %f, yaxis_tilt %f\n",xaxis_tilt,yaxis_tilt);
	x_amplitude = (x_tilt*x_tilt_weight)+(x_axis_tilt*x_axis_weight);
        y_amplitude = (y_tilt*y_tilt_weight)+(y_axis_tilt*y_axis_weight);

	for( i=0; i < bands; i++)
	{
		if (i==0)
		{
			pt[0].x=width-(((i*x_draw_width)*(1-x3d_start))/bands)\
				-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
				-(x3d_scroll/2)-x_offset+x_fudge-(x3d_scroll%2)
				-(gint)plevels[i]*x_amplitude;

			pt[0].y=height-((i*y_draw_height*y3d_start)/bands)\
				-(((bands-i)*y_draw_height*y3d_end)/bands)\
				-(2*y_shift_per_block)-y_offset-(z3d_scroll%2)\
				-(gint)plevels[i]*y_amplitude;

			pt[1].x=pt[0].x+(2*(x3d_scroll/2))+(x3d_scroll%2)\
				-(gint)levels[i]*x_amplitude\
				+(gint)plevels[i]*x_amplitude;

			pt[1].y=height-((i*y_draw_height*y3d_start)/bands)\
				-(((bands-i)*y_draw_height*y3d_end)/bands)\
				-y_offset\
				-(gint)levels[i]*y_amplitude;

			pt[2].x=width-(((i*x_draw_width)*(1-x3d_start))/bands)\
				-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
				-x_shift_per_block+(x3d_scroll/2)-x_offset\
				-(gint)levels[i+1]*x_amplitude;

			pt[2].y=height-(((i+1)*y_draw_height*y3d_start)/bands)\
				-(((bands-i-1)*y_draw_height*y3d_end)/bands)\
				-y_offset\
				-(gint)levels[i+1]*y_amplitude;

			pt[3].x=width-(((i*x_draw_width)*(1-x3d_start))/bands)\
				-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
				-x_shift_per_block-(x3d_scroll/2)\
				-x_offset-(x3d_scroll%2)\
				-(gint)plevels[i+1]*x_amplitude;

			pt[3].y=height-(((i+1)*y_draw_height*y3d_start)/bands)\
				-(((bands-i-1)*y_draw_height*y3d_end)/bands)\
				-(2*y_shift_per_block)-y_offset-(z3d_scroll%2)\
				-(gint)plevels[i+1]*y_amplitude;

			/* Low Freq "cap" to avoid open end in the 3D disp */
			cap_pt[0].x=width\
				-(((i*x_draw_width)*(1-x3d_start))/bands)\
				-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
				-(x3d_scroll/2)-x_offset+x_fudge-(x3d_scroll%2);

			cap_pt[0].y=height-((i*y_draw_height*y3d_start)/bands)\
				-(((bands-i)*y_draw_height*y3d_end)/bands)\
				-(2*y_shift_per_block)-y_offset-(z3d_scroll%2);

			cap_pt[1].x=cap_pt[0].x+(2*(x3d_scroll/2))\
				+(x3d_scroll%2);

			cap_pt[1].y=height\
				-((i*y_draw_height*y3d_start)/bands)\
				-(((bands-i)*y_draw_height*y3d_end)/bands)\
				-y_offset;
			cap_pt[2].x=pt[1].x;
			cap_pt[2].y=pt[1].y;
			cap_pt[3].x=pt[0].x;
			cap_pt[3].y=pt[0].y;
		
		}
		if ((i > 0 ) && (i <(bands-1)))  /* case from i-1 to i< bands-1 */
		{
			pt[0].x=oldpt[3].x;
			pt[0].y=oldpt[3].y;
			pt[1].x=oldpt[2].x;
			pt[1].y=oldpt[2].y;
			pt[2].x=width-(((i*x_draw_width)*(1-x3d_start))/bands)\
				-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
				-x_shift_per_block+(x3d_scroll/2)-x_offset\
				-(gint)levels[i+1]*x_amplitude;

			pt[2].y=height-(((i+1)*y_draw_height*y3d_start)/bands)\
				-(((bands-i-1)*y_draw_height*y3d_end)/bands)\
				-y_offset\
				-(gint)levels[i+1]*y_amplitude;

			pt[3].x=width-(((i*x_draw_width)*(1-x3d_start))/bands)\
				-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
				-x_shift_per_block-(x3d_scroll/2)\
				-x_offset-(x3d_scroll%2)\
				-(gint)plevels[i+1]*x_amplitude;

			pt[3].y=height-(((i+1)*y_draw_height*y3d_start)/bands)\
				-(((bands-i-1)*y_draw_height*y3d_end)/bands)\
				-(2*y_shift_per_block)-y_offset-(z3d_scroll%2)\
				-(gint)plevels[i+1]*y_amplitude;
		}
		if (i == (bands-1))
		{
			pt[0].x=oldpt[3].x;
			pt[0].y=oldpt[3].y;
			pt[1].x=oldpt[2].x;
			pt[1].y=oldpt[2].y;
			pt[2].x=width-(((i*x_draw_width)*(1-x3d_start))/bands)\
				-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
				-x_shift_per_block+(x3d_scroll/2)-x_offset;

			pt[2].y=height-(((i+1)*y_draw_height*y3d_start)/bands)\
				-(((bands-i-1)*y_draw_height*y3d_end)/bands)-y_offset;

			pt[3].x=width-(((i*x_draw_width)*(1-x3d_start))/bands)\
				-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
				-x_shift_per_block-(x3d_scroll/2)\
				-x_offset-(x3d_scroll%2);

			pt[3].y=height-(((i+1)*y_draw_height*y3d_start)/bands)\
				-(((bands-i-1)*y_draw_height*y3d_end)/bands)\
				-(2*y_shift_per_block)-y_offset-(z3d_scroll%2);
		}

		lvl=abs((gint)plevels[i]*2); 	/* 1/2 as much as spike mode 
						 * so it doesn't look too 
						 * "crowded" with colors.
						 */

		if (lvl > (MAXBANDS-1)) lvl=MAXBANDS-1;
		br=16+(gint)(levels[i]-plevels[i+1]);

		if (br<0) br=0;
		else if (br > (MAXBANDS-1)) br= (MAXBANDS-1);
		cl.pixel=colortab[br][lvl];
		gdk_gc_set_foreground(gc,&cl);

		/* Variables for black outline on the displays */

		lpt[0].x=pt[1].x+(gint)levels[i]*x_amplitude;
		lpt[0].y=pt[1].y+(gint)levels[i]*y_amplitude;
		lpt[1].x=pt[1].x;
		lpt[1].y=pt[1].y;
		lpt[2].x=pt[2].x;
		lpt[2].y=pt[2].y;
		if(i == (bands-1))
		{
			lpt[3].x=pt[2].x;
			lpt[3].y=pt[2].y;
		}
		else
		{
			lpt[3].x=pt[2].x+(gint)levels[i+1]*x_amplitude;
			lpt[3].y=pt[2].y+(gint)levels[i+1]*y_amplitude;
					
		}

		switch (sub_mode_3D)
		{
			case FILL_3D:
				if (i == 0) /* cap */
				{
					gdk_draw_polygon(main_pixmap,\
							gc,TRUE,\
							cap_pt,4);
					if (outlined)
					{
						gdk_draw_polygon(main_pixmap,\
							main_display->\
							style->\
							black_gc,\
							FALSE,cap_pt,4);
					}
				}
				gdk_draw_polygon(main_pixmap,\
						gc,TRUE,pt,4);
				if (outlined)
				{
					gdk_draw_polygon(main_pixmap,\
							main_display->style->\
							black_gc,FALSE,pt,4);
				}
				break;
			case WIRE_3D:

				if (i == 0) /* cap */
				{
					gdk_draw_polygon(main_pixmap,\
							main_display->\
							style->\
							black_gc,\
							TRUE,cap_pt,4);
					gdk_draw_polygon(main_pixmap,\
							gc,FALSE,\
							cap_pt,4);
				}
				gdk_draw_polygon(main_pixmap,\
						main_display->style->black_gc,\
						TRUE,pt,4);
				gdk_draw_polygon(main_pixmap,\
						gc,FALSE,pt,4);
				break;
		}


		if(show_leader)
		{
			switch (sub_mode_3D)
			{
				case FILL_3D:
					gdk_draw_polygon(main_pixmap,\
							gc,TRUE,lpt,4);
					if (outlined)
					{
						gdk_draw_line(main_pixmap,\
								main_display->\
								style->\
								black_gc,\
								lpt[2].x,\
								lpt[2].y,\
								lpt[1].x,\
								lpt[1].y);
					}

					break;
				case WIRE_3D:
					gdk_draw_line(main_pixmap,gc,\
							lpt[2].x,\
							lpt[2].y,\
							lpt[3].x,\
							lpt[3].y);
					break;
			}
		}

		oldpt[0].x = pt[0].x;
		oldpt[0].y = pt[0].y;
		oldpt[1].x = pt[1].x;
		oldpt[1].y = pt[1].y;
		oldpt[2].x = pt[2].x;
		oldpt[2].y = pt[2].y;
		oldpt[3].x = pt[3].x;
		oldpt[3].y = pt[3].y;

	}
}


void draw_land3d_reverse()
{
//printf("land3d reverse\n");
//printf("x_tilt %f, y_tilt %f\n",x_tilt,y_tilt);
//printf("xaxis_tilt %f, yaxis_tilt %f\n",xaxis_tilt,yaxis_tilt);
	x_amplitude = (x_tilt*x_tilt_weight)+(x_axis_tilt*x_axis_weight);
        y_amplitude = (y_tilt*y_tilt_weight)+(y_axis_tilt*y_axis_weight);
	for(i=bands-1; i >= 0; i--)
	{
		pt[0].x=width-(((i*x_draw_width)*(1-x3d_start))/bands)\
			-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
			-(x3d_scroll/2)-x_offset+x_fudge-(x3d_scroll%2)\
			-(gint)plevels[i]*x_amplitude;

		pt[0].y=height-(((i)*y_draw_height*y3d_start)/bands)\
			-(((bands-i)*y_draw_height*y3d_end)/bands)\
			-(2*y_shift_per_block)-y_offset-(z3d_scroll%2)\
			-(gint)plevels[i]*y_amplitude;

		pt[1].x=pt[0].x+2*(x3d_scroll/2)+(x3d_scroll%2)\
			-(gint)levels[i]*x_amplitude\
			+(gint)plevels[i]*x_amplitude;

		pt[1].y=height-(((i)*y_draw_height*y3d_start)/bands)\
			-(((bands-i)*y_draw_height*y3d_end)/bands)-y_offset\
			-(gint)levels[i]*y_amplitude;

		if(i == (bands-1))
		{
			pt[2].x=width-(((i*x_draw_width)*(1-x3d_start))/bands)\
				-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
				-x_shift_per_block+(x3d_scroll/2)-x_offset;

			pt[2].y=height-(((i+1)*y_draw_height*y3d_start)/bands)\
				-(((bands-i-1)*y_draw_height*y3d_end)/(bands))-y_offset;

			pt[3].x=width-(((i*x_draw_width)*(1-x3d_start))/bands)\
				-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
				-x_shift_per_block-(x3d_scroll/2)\
				-x_offset-(x3d_scroll%2);

			pt[3].y=height-(((i+1)*y_draw_height*y3d_start)/bands)\
				-(((bands-i-1)*y_draw_height*y3d_end)/bands)\
				-(2*y_shift_per_block)-y_offset-(z3d_scroll%2);
		}
		else
		{
			pt[2].x = oldpt[1].x;
			pt[2].y = oldpt[1].y;
			pt[3].x = oldpt[0].x;
			pt[3].y = oldpt[0].y;
		}
		if (i == 0)
		{	/* Low Freq "cap" to avoid open end in the 3D disp */
			cap_pt[0].x=width\
				-(((i*x_draw_width)*(1-x3d_start))/bands)\
				-((((bands-i)*x_draw_width)*(1-x3d_end))/bands)\
				-(x3d_scroll/2)-x_offset+x_fudge-(x3d_scroll%2);
			cap_pt[0].y=height\
				-(((i)*y_draw_height*y3d_start)/bands)\
				-(((bands-i)*y_draw_height*y3d_end)/bands)\
				-(2*y_shift_per_block)-y_offset-(z3d_scroll%2);

			cap_pt[1].x=cap_pt[0].x+2*(x3d_scroll/2)+(x3d_scroll%2);

			cap_pt[1].y=height\
				-(((i)*y_draw_height*y3d_start)/bands)\
				-(((bands-i)*y_draw_height*y3d_end)/bands)\
				-y_offset;
			cap_pt[2].x=pt[1].x;
			cap_pt[2].y=pt[1].y;
			cap_pt[3].x=pt[0].x;
			cap_pt[3].y=pt[0].y;
		}


		lvl=abs((gint)plevels[i]*2);	/* 1/2 as much as spike mode so it 
						 * doesn't look to"crowded" with colors.
						 */
		if (lvl > (MAXBANDS-1)) lvl=MAXBANDS-1;
		br=16+(gint)(levels[i]-plevels[i+1]);

		if (br<0) br=0;
		else if (br > (MAXBANDS-1)) br= (MAXBANDS-1);
		cl.pixel=colortab[br][lvl];
		gdk_gc_set_foreground(gc,&cl);

		lpt[0].x=pt[1].x+(gint)levels[i]*x_amplitude;
		lpt[0].y=pt[1].y+(gint)levels[i]*y_amplitude;
		lpt[1].x=pt[1].x;
		lpt[1].y=pt[1].y;
		lpt[2].x=pt[2].x;
		lpt[2].y=pt[2].y;
		if(i == (bands-1))
		{
			lpt[3].x=pt[2].x;
			lpt[3].y=pt[2].y;
		}
		else
		{
			lpt[3].x=pt[2].x+(gint)levels[i+1]*x_amplitude;
			lpt[3].y=pt[2].y+(gint)levels[i+1]*y_amplitude;
		}

		switch (sub_mode_3D)
		{
			case FILL_3D:
				gdk_draw_polygon(main_pixmap,\
						gc,TRUE,pt,4);
				if (outlined)
				{
					gdk_draw_polygon(main_pixmap,\
							main_display->\
							style->black_gc,\
							FALSE,pt,4);
				}
				if (i == 0) /* cap */
				{
					gdk_draw_polygon(main_pixmap,\
							gc,TRUE,\
							cap_pt,4);
					if (outlined)
					{
						gdk_draw_polygon(main_pixmap,\
								main_display->\
								style->\
								black_gc,\
								FALSE,cap_pt,4);
					}
				}
				break;
			case WIRE_3D:
				gdk_draw_polygon(main_pixmap,\
						main_display->\
						style->black_gc,\
						TRUE,pt,4);
				gdk_draw_polygon(main_pixmap,\
						gc,FALSE,pt,4);
				if (i == 0) /* cap */
				{
					gdk_draw_polygon(main_pixmap,\
							main_display->\
							style->\
							black_gc,TRUE,\
							cap_pt,4);
					gdk_draw_polygon(main_pixmap,\
							gc,\
							FALSE,cap_pt,4);
				}
				break;
		}



		if(show_leader)
		{		/* Show_Leader */
			switch (sub_mode_3D)
			{
				case FILL_3D:
					gdk_draw_polygon(main_pixmap,\
							gc,TRUE,lpt,4);
					if (outlined)
					{
						gdk_draw_line(main_pixmap,\
								main_display->\
								style->\
								black_gc,\
								lpt[2].x,\
								lpt[2].y,\
								lpt[1].x,\
								lpt[1].y);
					}
					break;
				case WIRE_3D:
					gdk_draw_line(main_pixmap,\
							gc,lpt[2].x,\
							lpt[2].y,\
							lpt[3].x,\
							lpt[3].y);
					break;
			}
		}		/* Show_Leader */

		oldpt[0].x = pt[0].x;
		oldpt[0].y = pt[0].y;
		oldpt[1].x = pt[1].x;
		oldpt[1].y = pt[1].y;
		oldpt[2].x = pt[2].x;
		oldpt[2].y = pt[2].y;
		oldpt[3].x = pt[3].x;
		oldpt[3].y = pt[3].y;
	}
}
