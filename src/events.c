/*
 * events.c extace source file
 * Contains all GTK/GDK event handlers (well, most of them)
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
#include <math.h>
#include <gtk/gtk.h>


gint lock_x_at;


gint configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    GdkPixmap *pixmap=NULL;
    gint w=0;
    gint h=0;

    if (widget->window)  // Make sure window exists
    {
	if ((int)data == BUFFER_AREA)
	{
//	    printf("Selected BUFFER_AREA pixmap\n");
	    w = buffer_area_width;
	    h = buffer_area_height;
	    pixmap = buffer_pixmap;
	}
	else if ((int)data == MAIN_DISPLAY)
	{
//	    printf("Selected MAIN_DISPLAY pixmap\n");
	    w = width;
	    h = height;
	    pixmap = main_pixmap;
	}
	else if ((int)data == DIR_AREA)
	{
//	    printf("Selected DIR_AREA pixmap\n");
	    w = dir_width;
	    h = dir_height;
	    pixmap = dir_pixmap;
	}

	//	printf("w=%i, event_w=%i ,h=%i ,event_h=%i\n",w,event->width,h,event->height);
	if ((w!=event->width)||(h!=event->height))
	{
	    if (pixmap)
	    {
		gdk_pixmap_unref(pixmap);
	    }

	    w=event->width;
	    h=event->height;

	    pixmap=gdk_pixmap_new(widget->window,
		    w,h,
		    gtk_widget_get_visual(widget)->depth);
	    gdk_draw_rectangle(pixmap,
		    widget->style->black_gc,
		    TRUE, 0,0,
		    w,h);
	    gdk_window_set_back_pixmap(widget->window,pixmap,0);
	}

	if ((int)data == BUFFER_AREA)
	{
	    buffer_pixmap = pixmap;
	    buffer_area_width = w;
	    buffer_area_height = h;
	    buffer_area_update();
	}
	else if ((int)data == DIR_AREA)
	{
	    dir_pixmap = pixmap;
	    dir_width = w;
	    dir_height = h;
	    setup_dircontrol(widget);
	    dir_axis_update();
	    update_pointer();
	}
	else if ((int)data == MAIN_DISPLAY)
	{
	    width = w;
	    height = h;
	    main_pixmap = pixmap;

	    switch (mode)
	    {
		case (HORIZ_SPECGRAM):
		    if (horiz_spec_start < 55)
			horiz_spec_start = 55;
		    if (horiz_spec_start > width)
			horiz_spec_start = width-10;
		    display_markers = 1;
		    clear_display = 1;
		    break;
		case (VERT_SPECGRAM):
		    if (vert_spec_start < 120)
			vert_spec_start = 120;
		    if (vert_spec_start > height)
			vert_spec_start = height-10;
		    display_markers = 1;
		    clear_display = 1;
		    break;
		case (SPIKE_3D):
		    update_dircontrol(dir_area);
		    break;

		default:
		    break;
	    }
	    convolve_factor = floor(nsamp/width) < 3 ? floor(nsamp/width) : 3 ;
	    if (convolve_factor == 0)
		convolve_factor = 1;

	}

    }
    gdk_window_clear(widget->window);
    return TRUE;
}

gint expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    GdkPixmap *pixmap = NULL;

	if ((int)data == BUFFER_AREA)
	{
//	    printf("Selected BUFFER_AREA EXPOSE pixmap\n");
	    pixmap = buffer_pixmap;
	}
	else if ((int)data == MAIN_DISPLAY)
	{
//	    printf("Selected MAIN_DISPLAY EXPOSE pixmap\n");
	    pixmap = main_pixmap;
	}
	else if ((int)data == DIR_AREA)
	{
//	    printf("Selected DIR_AREA EXPOSE pixmap\n");
	    pixmap = dir_pixmap;
	}
    gdk_draw_pixmap(widget->window,
	    widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
	    pixmap,
	    event->area.x, event->area.y,
	    event->area.x, event->area.y,
	    event->area.width, event->area.height);

    return TRUE;
}
gint main_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    gdk_draw_pixmap(widget->window,
	    widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
	    main_pixmap,
	    event->area.x, event->area.y,
	    event->area.x, event->area.y,
	    event->area.width, event->area.height);

    return TRUE;
}
gint buffer_area_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    gdk_draw_pixmap(widget->window,
	    widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
	    buffer_pixmap,
	    event->area.x, event->area.y,
	    event->area.x, event->area.y,
	    event->area.width, event->area.height);

    return TRUE;
}

gint button_notify_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    int x,y;
    int result;
    x = event->x;
    y = event->y;
    if(event->state & (GDK_BUTTON1_MASK))
    {
	pt_lock = 0;
	one_to_fix = 0;
//	spec_drag = 0;
#ifdef DND_DEBUG
	g_print("BUTTON 1 RELEASED!! releasing lock\n");
#endif
	return TRUE;
    }
    if(event->state & (GDK_BUTTON2_MASK))
    {
	/* do something related to letting go of button two (colormap button) */
#ifdef DND_DEBUG
	g_print("BUTTON 2 RELEASED!! releasing lock\n");
#endif
	return TRUE;
    }

    if (event->button == 1 ) /* If your press button 1 (left one) */
    {
#ifdef DND_DEBUG
	g_print("BUTTON 1 PRESSED!! running handler \n");
#endif
	if (mode == EQ_2D)
	{
	    int band_num =0;
	    int x_draw_width = width-2*x_border;

	    if (x < x_border)
		x = x_border;
	    if (x > width-x_border)
		x = width-x_border;
	    band_num = (int)(((float)(x-x_border)/(float)(x_draw_width))*(float)bands);
	    if (band_num > bands-1)
		band_num = bands-1;
	    freq_at_pointer = freqmark[band_num];

	    pt_lock = 1;
	    one_to_fix = EQ_2D;
	}
	    
	if ((mode == LAND_3D)||(mode == SPIKE_3D)||(mode == HORIZ_SPECGRAM)||
		(mode == VERT_SPECGRAM))
	{
	    result = test_if_close(x,y);
	    if (result > 0)
	    {
#ifdef DND_DEBUG
		g_print("mouse is close enough to an endpoint\n");
		g_print("setting lock\n");
#endif
		lock_x_at = x;	// Remeber where we grabbed on.
		pt_lock = 1;
		one_to_fix = result;

	    }
	    else 
	    {
#ifdef DND_DEBUG
		g_print("mouse is NOT close enough to an endpoint\n");
		g_print("releasing lock\n");
#endif
		pt_lock = 0;
		one_to_fix = 0;
	    }
	}
    }
    if (event->button == 2 ) /* If your press button 2 (middle one) */
    {
	if (!grad_win_present)
	{
	    grad_win_create();
	    gradient_update();
	}
    }
    if ((event->button == 3 ) && ((mode == HORIZ_SPECGRAM) || (mode == VERT_SPECGRAM))) /* If your press button 3 (right one) */
    {
	/* This section of code is designed for stretching and compressing the 
	 * frequency axis, (Zoom ability). IT ain't written yet.. feel free
	 * to write it and submit it to the author so he can add it into the 
	 * normal releases...
	 */
	result = test_on_line(x,y); /* find out if the place we click is
				     * on the "drawing" line of the display.
				     * This place is where the images is being
				     * drawn "from". (i.e. start of spectrogram)
				     */
	if (result == OFF_THE_LINE)
	{
#ifdef DND_DEBUG
	    g_print("We didn't!! \n");
#endif
	}
	else if (result == ON_THE_LINE)
	{
#ifdef DND_DEBUG
	    g_print("We did... \n");
#endif
	}
    }


    return TRUE;
}
gint motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    int x, y;
    GdkModifierType state;

    if (event->is_hint)
    {
	gdk_window_get_pointer (event->window, &x, &y, &state);
    }
    else
    {
	x = event->x;
	y = event->y;
	state = event->state;
    }

    if (pt_lock)
    {
	switch (one_to_fix)
	{
	    case EQ_2D:
		{
		    int band_num =0;
		    int x_draw_width = width-2*x_border;
		    
		    if (x < x_border)
			x = x_border;
		    if (x > width-x_border)
			x = width-x_border;
		    band_num = (int)(((float)(x-x_border)/(float)(x_draw_width))*(float)bands);
		    if (band_num > bands-1)
			band_num = bands-1;
		    freq_at_pointer = freqmark[band_num];
		    break;
		}
	    case CHANGE_X_START:
		{
#ifdef DND_DEBUG
		    g_print("Changing start point\n");
#endif
		    change_x_start(x,y);
		    update_dircontrol(dir_area);
//
		    break;
		}
	    case CHANGE_X_END:
		{
#ifdef DND_DEBUG
		    g_print("Changing end point\n");
#endif
		    change_x_end(x,y);
		    update_dircontrol(dir_area);
		    break;
		}
	    case CHANGE_SPEC_START:
		{
#ifdef DND_DEBUG
		    g_print("Changing Spectrogram Start point\n");
#endif
		    if (mode == HORIZ_SPECGRAM)
		    {
			change_spec_start(x);
		    }
		    if (mode == VERT_SPECGRAM)
		    {
			change_spec_start(y);
		    }
		}
	    default:
		break;
	}
    }
    return TRUE;
}

int test_on_line(int x_fed, int y_fed)
{ 

    if (mode == HORIZ_SPECGRAM)
    {
	if (abs(((width-horiz_spec_start) - x_fed)) < 20)
	    return (ON_THE_LINE);
    }
    if (mode == VERT_SPECGRAM)
    {
	if (abs(((height-vert_spec_start) - y_fed+20)) < 20)
	    return (ON_THE_LINE);
    }
    return (OFF_THE_LINE);
}

int test_if_close(int x_fed, int y_fed)
{
    struct point 
    {
	gint x;
	gint y;
    }start,end;
    gint x_rel=0;
    gint y_rel=0;
    gint x_draw_width=0;
    gint y_draw_height=0;

    /* Must pass both X and Y tests to be "captured", if both  x and y 
     * points pass then we're in trouble... :( (meaning both start and end)
     */

    switch (mode)
    {
	case (LAND_3D):
	    {
		x_draw_width = width - abs(x3d_scroll)-2*x_border;
		y_draw_height = height - abs(zdet_scroll)-2*y_border;
		start.x = (x3d_start*x_draw_width);
		start.y = ((1.0-y3d_start)*y_draw_height);
		end.x = (x3d_end*x_draw_width);
		end.y = ((1.0-y3d_end)*y_draw_height);
		x_rel = x_fed - x_border - abs(x3d_scroll); 
		if (x3d_scroll < 0)
		    x_rel -= x3d_scroll;
		y_rel = y_fed - 3*y_border - (abs(z3d_scroll)/2);
#ifdef DND_DEBUG
		g_print("fed coords, %i,%i, end %i,%i start %i,%i\n",x_rel,y_rel,end.x,end.y, start.x,start.y);
#endif

		/* tests to see if the mouse pointer is close to an end of an */

		if (abs((start.x-x_rel)) < 40)
		{
		    if (abs((start.y-y_rel)) < (40 + abs(z3d_scroll/2)))
		    {
			return (CHANGE_X_START);
		    }
		}
		else if (abs((end.x-x_rel)) < 40)
		{
		    if (abs((end.y-y_rel)) < 40); /* + levels[0]) */
		    {
			return (CHANGE_X_END);
		    }
		}

		break;
	    }
	case (SPIKE_3D):
	    {
		x_draw_width = width - abs(xdet_scroll)-2*x_border;
		y_draw_height = height - abs(zdet_scroll)-2*y_border;
		start.x = (xdet_start*x_draw_width);
		start.y = ((1.0-ydet_start)*y_draw_height);
		end.x = xdet_end*x_draw_width;
		end.y = (1.0-ydet_end)*y_draw_height;
		x_rel = x_fed - x_border - xdet_scroll/2; 
		if (xdet_scroll < 0)
		    x_rel += xdet_scroll;
		y_rel = y_fed-3*y_border - abs((zdet_scroll/2));

#ifdef DND_DEBUG
		g_print("fed coords, %i,%i, end %i,%i start %i,%i\n",x_rel,y_rel,end.x,end.y, start.x,start.y);
#endif
		/* tests to see if the mouse pointer is close to an end of an */

		if (abs((start.x-x_rel)) < 40)
		{
		    if (abs((start.y-y_rel)) < 40)
		    {
#ifdef DND_DEBUG
			g_print("Start det found\n");
#endif
			return (CHANGE_X_START);
		    }
		}
		else if (abs((end.x-x_rel)) < 40)
		{
		    if (abs((end.y-y_rel)) < 40)
		    {
#ifdef DND_DEBUG
			g_print("End det found\n");
#endif
			return (CHANGE_X_END);
		    }
		}
		break;
	    }
	case HORIZ_SPECGRAM:
	    {
		if (abs(((width-horiz_spec_start) - x_fed)) < 20)
		    return (CHANGE_SPEC_START);
		break;
	    }
	case VERT_SPECGRAM:
	    {
		if (abs(((height-vert_spec_start) - y_fed+20)) < 20)
		    return (CHANGE_SPEC_START);
		break;
	    }
    }

    return -1;
}

void change_spec_start(gint new_pos)
{
    if (mode == HORIZ_SPECGRAM)
    {
	horiz_spec_start = width-new_pos;
	if (horiz_spec_start < 55)
	    horiz_spec_start = 55;
	if (horiz_spec_start > width)
	    horiz_spec_start = width-10;
    }
    else if (mode == VERT_SPECGRAM)
    {
	vert_spec_start = height-new_pos+20;
	if (vert_spec_start < 120)
	    vert_spec_start = 120;
	if (vert_spec_start > height)
	    vert_spec_start = height-10;
    }
    display_markers = 1;
    clear_display = 1;
}

void change_x_start(gint x_rel, gint y_rel)
{
    int x_draw_width = 0;
    int y_draw_height = 0;
    y_rel -= 3.5*y_border;
    switch (mode)
    {
	case (LAND_3D):
	    x_rel -= x_border + x3d_scroll/2;
	    x_draw_width = width - abs(x3d_scroll)-2*x_border;
	    y_draw_height = height - abs(z3d_scroll)-2*y_border;
	    break;
	case (SPIKE_3D):
	    x_rel -= x_border;
	    x_draw_width = width - abs(xdet_scroll)-2*x_border;
	    y_draw_height = height - abs(zdet_scroll)-2*y_border;
	    break;
    }

    if (x_rel > x_draw_width)
	x_rel = x_draw_width;
    if (x_rel < x_offset/2)
	x_rel = x_offset/2;
    if (y_rel > y_draw_height)
	y_rel = y_draw_height;
    if (y_rel < y_offset/2)
	y_rel = y_offset/2;
    switch (mode)
    {
	case (LAND_3D):
	    {
		x3d_start = abs(abs(0.5*x3d_scroll)-(gfloat)x_rel)/(gfloat)x_draw_width;
		y3d_start = 1.0-(abs(abs(0.5*z3d_scroll)-(gfloat)y_rel)/(gfloat)y_draw_height);
		break;
	    }
	case (SPIKE_3D):
	    {
		xdet_start = abs(abs(0.5*xdet_scroll)-(gfloat)x_rel)/(gfloat)x_draw_width;
		ydet_start = 1.0-(abs(abs(0.5*zdet_scroll)-(gfloat)y_rel)/(gfloat)y_draw_height);
		break;
	    }
    }
}

void change_x_end(gint x_rel, gint y_rel)
{
    int x_draw_width = 0;
    int y_draw_height = 0;
    y_rel -= 3.5*y_border;
    switch (mode)
    {
	case (LAND_3D):
	    x_rel -= x_border + x3d_scroll/2;
	    x_draw_width = width - abs(x3d_scroll)-2*x_border;
	    y_draw_height = height - abs(z3d_scroll)-2*y_border;
	    break;
	case (SPIKE_3D):
	    x_rel -= x_border;
	    x_draw_width = width - abs(xdet_scroll)-2*x_border;
	    y_draw_height = height - abs(zdet_scroll)-2*y_border;
	    break;
    }

    if (x_rel > x_draw_width)
	x_rel = x_draw_width;
    if (x_rel < x_offset/2)
	x_rel = x_offset/2;
    if (y_rel > y_draw_height)
	y_rel = y_draw_height;
    if (y_rel < y_offset/2)
	y_rel = y_offset/2;
    switch (mode)
    {
	case (LAND_3D):
	    {
		x3d_end = abs(abs(0.5*x3d_scroll)-(gfloat)x_rel)/(gfloat)x_draw_width;
		y3d_end = 1.0-(abs(abs(0.5*z3d_scroll)-(gfloat)y_rel)/(gfloat)y_draw_height);
		break;
	    }
	case (SPIKE_3D):
	    {
		xdet_end = abs(abs(0.5*xdet_scroll)-(gfloat)x_rel)/(gfloat)x_draw_width;
		ydet_end = 1.0-(abs(abs(0.5*zdet_scroll)-(gfloat)y_rel)/(gfloat)y_draw_height);
		break;
	    }
    }

}
