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
#include <protos.h>
#include <math.h>
#include <gtk/gtk.h>


/* See globals.h for variable declarations and DEFINES */
static gint i=0;
static gint j=0;
static gint height_per_scope=0;
static gint prev_left_val=0;
static gint prev_right_val=0;
static gint prev_old_left_val=0;
static gint prev_old_right_val=0;
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



void draw_scope()
{
    lo_width = (width < nsamp) ? width : nsamp;
    height_per_scope = height/4;

    gdk_threads_enter();
    if (use_back_pixmap)
    {
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
    }
    else if (scope_sub_mode == GRAD_SCOPE)
    {
	gdk_draw_rectangle(main_display->window,
		main_display->style->black_gc,
		TRUE, 0,0,
		lo_width,height);
    }
    prev_right_val = 0;
    prev_old_right_val = 0;

    prev_left_val = 0;
    prev_old_left_val = 0;

    if (show_graticule)
    {
	max = (height_per_scope < 128) ? height_per_scope : 128;
	for (i=0;i<=max;i+=32)
	{
	    if(use_back_pixmap)
	    {
		
		gdk_draw_line(main_pixmap,graticule_gc,0,height-height/4+i,lo_width,height-height/4+i);
		gdk_draw_line(main_pixmap,graticule_gc,0,height-height/4-i,lo_width,height-height/4-i);
		gdk_draw_line(main_pixmap,graticule_gc,0,height/4+i,lo_width,height/4+i);
		gdk_draw_line(main_pixmap,graticule_gc,0,height/4-i,lo_width,height/4-i);
	    }
	    else
	    {
		gdk_draw_line(main_display->window,graticule_gc,0,height-height/4+i,lo_width,height-height/4+i);
		gdk_draw_line(main_display->window,graticule_gc,0,height-height/4-i,lo_width,height-height/4-i);
		gdk_draw_line(main_display->window,graticule_gc,0,height/4+i,lo_width,height/4+i);
		gdk_draw_line(main_display->window,graticule_gc,0,height/4-i,lo_width,height/4-i);
	    }
	}
	i-=32;

	for (j=0;j<lo_width/2;j+=32)
	{
	    if(use_back_pixmap)
	    {
		gdk_draw_line(main_pixmap,graticule_gc,lo_width/2+j,height/4-i,lo_width/2+j,height/4+i);
		gdk_draw_line(main_pixmap,graticule_gc,lo_width/2-j,height/4-i,lo_width/2-j,height/4+i);
		gdk_draw_line(main_pixmap,graticule_gc,lo_width/2+j,height-height/4-i,lo_width/2+j,height-height/4+i);
		gdk_draw_line(main_pixmap,graticule_gc,lo_width/2-j,height-height/4-i,lo_width/2-j,height-height/4+i);
	    }
	    else
	    {
		gdk_draw_line(main_display->window,graticule_gc,lo_width/2+j,height/4-i,lo_width/2+j,height/4+i);
		gdk_draw_line(main_display->window,graticule_gc,lo_width/2-j,height/4-i,lo_width/2-j,height/4+i);
		gdk_draw_line(main_display->window,graticule_gc,lo_width/2+j,height-height/4-i,lo_width/2+j,height-height/4+i);
		gdk_draw_line(main_display->window,graticule_gc,lo_width/2-j,height-height/4-i,lo_width/2-j,height-height/4+i);
	    }
	}
    }
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
	old_left_val=(gint)(audio_last_l[old_scope_pos_l]*left_amplitude);
	left_val=(gint)(audio_left[scope_pos_l]*left_amplitude);

	old_right_val=(gint)(audio_last_r[old_scope_pos_r]*right_amplitude);
	right_val=(gint)(audio_right[scope_pos_r]*right_amplitude);

	if (i >= 1)
	{
	    prev_old_left_val=(gint)(audio_last_l[old_scope_pos_l-1]*left_amplitude);
	    prev_left_val=(gint)(audio_left[scope_pos_l-1]*left_amplitude);
	    prev_old_right_val=(gint)(audio_last_r[old_scope_pos_r-1]*right_amplitude);
	    prev_right_val=(gint)(audio_right[scope_pos_r-1]*right_amplitude);
	}
	if (left_val < -127)
	{
	    left_val = -127;
	    old_left_val = -127;
	}
	if (prev_left_val < -127)
	{
	    prev_left_val = -127;
	    prev_old_left_val = -127;
	}
	else if (left_val > 127)
	{
	    left_val = 127;
	    old_left_val = 127;
	}
	else if (prev_left_val > 127)
	{
	    prev_left_val = 127;
	    prev_old_left_val = 127;
	}

	if (right_val < -127)
	{
	    right_val = -127;
	    old_right_val = -127;
	}
	if (prev_right_val < -127)
	{
	    prev_right_val = -127;
	    prev_old_right_val = -127;
	}
	else if (right_val > 127)
	{
	    right_val = 127;
	    old_right_val = 127;
	}
	else if (prev_right_val > 127)
	{
	    prev_right_val = 127;
	    prev_old_right_val = 127;
	}

	switch (scope_sub_mode)
	{
	    case DOT_SCOPE:
		if (use_back_pixmap)
		{
		    gdk_draw_point(main_pixmap,main_display->style->white_gc,i,((height/4))+left_val);
		    /* RIGHT Channel scope */
		    gdk_draw_point(main_pixmap,main_display->style->white_gc,i,(height-(height/4))+right_val);
		}
		else
		{
		    gdk_draw_point(main_display->window,main_display->style->black_gc,i,((height/4))+old_left_val);
		    gdk_draw_point(main_display->window,main_display->style->white_gc,i,((height/4))+left_val);
		    /* RIGHT Channel scope */
		    gdk_draw_point(main_display->window,main_display->style->black_gc,i,(height-(height/4))+old_right_val);
		    gdk_draw_point(main_display->window,main_display->style->white_gc,i,(height-(height/4))+right_val);
		}
		break;

	    case LINE_SCOPE:
		if (use_back_pixmap)
		{
		
		    gdk_draw_line(main_pixmap,main_display->style->white_gc,i-1,((height/4))+prev_left_val,i,((height/4))+left_val);
		    /* RIGHT Channel scope */
		    gdk_draw_line(main_pixmap,main_display->style->white_gc,i-1,(height-(height/4))+prev_right_val,i,(height-(height/4))+right_val);
		}
		else
		{
		    gdk_draw_line(main_display->window,main_display->style->black_gc,i-1,((height/4))+prev_old_left_val,i,((height/4))+old_left_val);
		    gdk_draw_line(main_display->window,main_display->style->white_gc,i-1,((height/4))+prev_left_val,i,((height/4))+left_val);
		    /* RIGHT Channel scope */
		    gdk_draw_line(main_display->window,main_display->style->black_gc,i-1,(height-(height/4))+prev_old_right_val,i,(height-(height/4))+old_right_val);
		    gdk_draw_line(main_display->window,main_display->style->white_gc,i-1,(height-(height/4))+prev_right_val,i,(height-(height/4))+right_val);
		}
		break;
	    case GRAD_SCOPE:
		if (left_val == 0)
		    left_val++;
		if (right_val == 0)
		    right_val++;
		if (use_back_pixmap)
		{

		    if (left_val < 0)
			gdk_draw_pixmap(main_pixmap,gc,grad[left_val+127],0,0,                                        i,((height/4))+left_val,1,-left_val);
		    else
			gdk_draw_pixmap(main_pixmap,gc,grad[left_val+127],0,0,                                        i,height/4,1,left_val);

		    if (right_val < 0)
			gdk_draw_pixmap(main_pixmap,gc,grad[right_val+127],0,0,					i,(height-(height/4))+right_val,1,-right_val);
		    else
			gdk_draw_pixmap(main_pixmap,gc,grad[right_val+127],0,0,					i,(height-(height/4)),1,right_val);

		}
		else
		{
		    if (left_val < 0)
			gdk_draw_pixmap(main_display->window,gc,grad[left_val+127],0,0,						i,((height/4))+left_val,1,-left_val);
		    else
			gdk_draw_pixmap(main_display->window,gc,grad[left_val+127],0,0,						i,((height/4)),1,left_val);

		    if (right_val < 0)
			gdk_draw_pixmap(main_display->window,gc,grad[right_val+127],0,0,					i,(height-(height/4))+right_val,1,-right_val);
		    else
			gdk_draw_pixmap(main_display->window,gc,grad[right_val+127],0,0,					i,(height-(height/4)),1,right_val);
		}
		break;
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

    if (use_back_pixmap)
    {
	gdk_window_clear(main_display->window);
    }

    gdk_threads_leave();

}
