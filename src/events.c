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

#include <color_win.h>
#include <config.h>
#include <dir.h>
#include <enums.h>
#include <events.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <math.h>
#include <markers.h>


gint lock_x_at;
gint lock_y_at;
gint buffer_area_width;
gint buffer_area_height;
gint dir_width;
gint dir_height;
gint one_to_fix = 0;
gboolean pt_lock = FALSE;

extern gfloat xdet_start;
extern gfloat xdet_end;
extern gfloat x3d_start;
extern gfloat x3d_end;
extern gfloat ydet_start;
extern gfloat ydet_end;
extern gfloat y3d_start;
extern gfloat y3d_end;
extern gint xdet_scroll;    /* 3D spike scroll in pixels */
extern gint zdet_scroll;    /* 3D spike scroll in pixels */
extern gint x3d_scroll;     /* 3D scroll in pixels x axis */
extern gint z3d_scroll;     /* 3D scroll in pixels z axis */


gint configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	GdkPixmap *pixmap=NULL;
	gint w=0;
	gint h=0;

	if (widget->window)  // Make sure window exists
	{
		switch ((DrawableArea)data)
		{
			case BUFFER_AREA:
				//			    printf("Selected BUFFER_AREA pixmap\n");
				w = buffer_area_width;
				h = buffer_area_height;
				pixmap = buffer_pixmap;
				break;

			case DIR_AREA:
				//			    printf("Selected DIR_AREA pixmap\n");
				w = dir_width;
				h = dir_height;
				pixmap = dir_pixmap;
				break;

			case MAIN_DISPLAY:
				//			    printf("Selected MAIN_DISPLAY pixmap\n");
				w = width;
				h = height;
				pixmap = main_pixmap;
				break;

			default:
				break;
		}

		//	printf("w=%i, event_w=%i ,h=%i ,event_h=%i\n",w,event->width,h,event->height);
		if ((w!=event->width)||(h!=event->height))
		{
			if (pixmap)
				gdk_pixmap_unref(pixmap);

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

		switch ((DrawableArea)data)
		{
			case BUFFER_AREA:
				buffer_pixmap = pixmap;
				buffer_area_width = w;
				buffer_area_height = h;
				buffer_area_update();
				break;

			case DIR_AREA:
				dir_pixmap = pixmap;
				dir_width = w;
				dir_height = h;
				update_dircontrol(dir_area);
				break;

			case MAIN_DISPLAY:
				main_pixmap = pixmap;
				width = w;
				height = h;

				switch ((DisplayMode)mode)
				{
					case HORIZ_SPECGRAM:
						if (horiz_spec_start < 60)
							horiz_spec_start = 60;
						if (horiz_spec_start > width)
							horiz_spec_start = width-10;
						display_markers = TRUE;
						clear_display = TRUE;
						break;
					case VERT_SPECGRAM:
						if (vert_spec_start < 120)
							vert_spec_start = 120;
						if (vert_spec_start > height)
							vert_spec_start = height-10;
						display_markers = TRUE;
						clear_display = TRUE;
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
				break;

			default:
				break;

		}

	}
	gdk_window_clear(widget->window);
	return TRUE;
}

gint expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	GdkPixmap *pixmap = NULL;

	switch ((DrawableArea)data)
	{
		case BUFFER_AREA:
			//	    	    printf("Selected BUFFER_AREA EXPOSE pixmap\n");
			pixmap = buffer_pixmap;
			break;

		case DIR_AREA:
			//	    	    printf("Selected DIR_AREA EXPOSE pixmap\n");
			pixmap = dir_pixmap;
			break;

		case MAIN_DISPLAY:
			//	    	    printf("Selected MAIN_DISPLAY EXPOSE pixmap\n");
			pixmap = main_pixmap;
			break;

		default:
			break;

	}
	gdk_draw_pixmap(widget->window,
			widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
			pixmap,
			event->area.x, event->area.y,
			event->area.x, event->area.y,
			event->area.width, event->area.height);

	return TRUE;
}

gint button_notify_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
	gint x,y;
	gint result;
	gint status;
	x = event->x;
	y = event->y;
	/* Button one released */
	if(event->state & (GDK_BUTTON1_MASK))
	{
		pt_lock = FALSE;
		one_to_fix = 0;
#ifdef DND_DEBUG
		g_print("BUTTON 1 RELEASED!! releasing lock\n");
#endif
		return TRUE;
	}
	/* Button two released */
	if(event->state & (GDK_BUTTON2_MASK))
	{
		/* do something related to letting go of button two (colormap button) */
#ifdef DND_DEBUG
		g_print("BUTTON 2 RELEASED!! releasing lock\n");
#endif
		return TRUE;
	}

	/* Button three released */
	if(event->state & (GDK_BUTTON3_MASK))
	{
#ifdef DND_DEBUG
		g_print("BUTTON 3 RELEASED!! releasing lock\n");
#endif
		return TRUE;
	}

	/* If your press button 1 (left one) */
	if (event->button == 1 )
	{
#ifdef DND_DEBUG
		g_print("BUTTON 1 PRESSED!! running handler \n");
#endif
		/* Test if close to an endpoint on 3D modes, or near the beginning
		 * of the scrolled regio on the spectragram modes
		 */
		if ((mode == LAND_3D)||\
				(mode == SPIKE_3D)||\
				(mode == HORIZ_SPECGRAM)||\
				(mode == VERT_SPECGRAM))

		{
			result = test_if_close(x,y);
			if (result > 0)
			{
#ifdef DND_DEBUG
				g_print("mouse is close enough to an endpoint\n");
				g_print("setting lock\n");
#endif
				lock_x_at = x;	// Remember where we grabbed on.
				lock_y_at = y;	// Remember where we grabbed on.
				pt_lock = TRUE;
				one_to_fix = result;

			}
			else 
			{
#ifdef DND_DEBUG
				g_print("mouse is NOT close enough to an endpoint\n");
				g_print("releasing lock\n");
#endif
				pt_lock = FALSE;
				one_to_fix = 0;
			}
		}
	}
	if (event->button == 2 ) /* If your press button 2 (middle one) */
	{
#ifdef DND_DEBUG
		g_print("BUTTON 2 PRESSED!! running handler\n");
#endif
		if (!grad_win_present)
		{
			grad_win_create();
			gradient_update();
		}
		else if (grad_win_present)
		{
			gtk_widget_hide(grad_win_ptr);
			grad_win_present = 0;
		}
	}
	if ((event->button == 3 ) && ((mode == HORIZ_SPECGRAM) || (mode == VERT_SPECGRAM))) /* If your press button 3 (right one) */
	{
#ifdef DND_DEBUG
		g_print("BUTTON 3 PRESSED!! running handler\n");
#endif
		/* This section of code is designed for stretching and compressing the 
		 * frequency axis, (Zoom ability). It ain't written yet.. feel free
		 * to write it and submit it to the author so he can add it into the 
		 * normal releases...
		 */
		status = test_on_line(x,y); /* find out if the place we click is
					     * on the "drawing" line of the display.
					     * This place is where the images is being
					     * drawn "from". (i.e. start of spectrogram)
					     */
		switch ((EventStatus)status)
		{
			case OFF_THE_LINE:
#ifdef DND_DEBUG
				g_print("We didn't find the line !! \n");
#endif
				break;
			case ON_THE_LINE:
#ifdef DND_DEBUG
				g_print("We did find the line !! \n");
#endif
				break;
			default:
				break;

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

	if (mode == EQ_2D)
	{
		int band_num =0;
		int x_draw_width = width-2*border;

		if (x < border)
			x = border;
		if (x > width-border)
			x = width-border;
		band_num = (int)(((float)(x-border)/(float)(x_draw_width))*(float)bands);
		if (band_num > bands-1)
			band_num = bands-1;
		freq_at_pointer = freqmark[band_num];
	}
	if (pt_lock)
	{
		switch ((EventOperation)one_to_fix)
		{
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
					g_print("Changing Spectragram Start point\n");
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
		if (abs(((width-horiz_spec_start) - x_fed)) < 30)
			return (ON_THE_LINE);
	}
	if (mode == VERT_SPECGRAM)
	{
		if (abs(((height-vert_spec_start) - y_fed+40)) < 30)
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

	switch ((DisplayMode)mode)
	{
		case LAND_3D:
			x_draw_width = width - abs(x3d_scroll)-2*border;
			y_draw_height = height - abs(z3d_scroll)-2*border;
			start.x = (x3d_start*x_draw_width);
			start.y = ((1.0-y3d_start)*y_draw_height);
			end.x = (x3d_end*x_draw_width);
			end.y = ((1.0-y3d_end)*y_draw_height);
			x_rel = x_fed - border - abs(x3d_scroll); 
			if (x3d_scroll < 0)
				x_rel -= x3d_scroll;
			y_rel = y_fed - 3*border - (abs(z3d_scroll)/2);
#ifdef DND_DEBUG
			g_print("Mouse Position, %i,%i Axis-start %i,%i Axis-end %i,%i\n",x_rel,y_rel,start.x,start.y, end.x,end.y);
#endif

			/* tests to see if the mouse pointer is close to an end of an */

			if (abs((start.x-x_rel)) < 40)
			{
				if (abs((start.y-y_rel)) < (40 + abs(z3d_scroll/2)))
				{
					return (CHANGE_X_START);
				}
			}
			if (abs((end.x-x_rel)) < 40)
			{
				if (abs((end.y-y_rel)) < 40); /* + levels[0]) */
				{
					return (CHANGE_X_END);
				}
			}

			break;
		case SPIKE_3D:
			x_draw_width = width - abs(xdet_scroll)-2*border;
			y_draw_height = height - abs(zdet_scroll)-2*border;
			start.x = (xdet_start*x_draw_width);
			start.y = ((1.0-ydet_start)*y_draw_height);
			end.x = xdet_end*x_draw_width;
			end.y = (1.0-ydet_end)*y_draw_height;
			x_rel = x_fed - border - xdet_scroll/2; 
			if (xdet_scroll < 0)
				x_rel += xdet_scroll;
			y_rel = y_fed-3*border - abs((zdet_scroll/2));

#ifdef DND_DEBUG
			g_print("fed coords, %i,%i, end %i,%i start %i,%i\n",x_rel,y_rel,end.x,end.y, start.x,start.y);
#endif
			/* tests to see if the mouse pointer is close to an end of an */

			if (abs((start.x-x_rel)) < 40)
			{
				if (abs((start.y-y_rel)) < 40)
				{
					return (CHANGE_X_START);
				}
			}
			if (abs((end.x-x_rel)) < 40)
			{
				if (abs((end.y-y_rel)) < 40)
				{
					return (CHANGE_X_END);
				}
			}
			break;
		case HORIZ_SPECGRAM:
			if (abs(((width-horiz_spec_start) - x_fed)) < 20)
				return (CHANGE_SPEC_START);
			break;
		case VERT_SPECGRAM:
			if (abs(((height-vert_spec_start) - y_fed+45)) < 27)
				return (CHANGE_SPEC_START);
			break;
		default:
			break;
	}

	return -1;
}

void change_spec_start(gint new_pos)
{
	switch ((DisplayMode)mode)
	{
		case HORIZ_SPECGRAM:
			horiz_spec_start = width-new_pos+3;
			if (horiz_spec_start < 60)
				horiz_spec_start = 60;
			if (horiz_spec_start > width)
				horiz_spec_start = width-10;
			break;

		case VERT_SPECGRAM:
			vert_spec_start = height-new_pos+52;
			if (vert_spec_start < 120)
				vert_spec_start = 120;
			if (vert_spec_start > height)
				vert_spec_start = height-10;
			break;

		default:
			break;
	}
	display_markers = TRUE;
	clear_display = TRUE;
}

void change_x_start(gint x_rel, gint y_rel)
{
	int x_draw_width = 0;
	int y_draw_height = 0;
	y_rel -= 3.5*border;
	switch ((DisplayMode)mode)
	{
		case LAND_3D:
			x_rel -= border + x3d_scroll/2;
			x_draw_width = width - abs(x3d_scroll)-2*border;
			y_draw_height = height - abs(z3d_scroll)-2*border;
			break;
		case SPIKE_3D:
			x_rel -= border;
			x_draw_width = width - abs(xdet_scroll)-2*border;
			y_draw_height = height - abs(zdet_scroll)-2*border;
			break;
		default:
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
	switch ((DisplayMode)mode)
	{
		case LAND_3D:
			x3d_start = abs(abs(0.5*x3d_scroll)-(gfloat)x_rel)/(gfloat)x_draw_width;
			y3d_start = 1.0-(abs(abs(0.5*z3d_scroll)-(gfloat)y_rel)/(gfloat)y_draw_height);
			break;
		case SPIKE_3D:
			xdet_start = abs(abs(0.5*xdet_scroll)-(gfloat)x_rel)/(gfloat)x_draw_width;
			ydet_start = 1.0-(abs(abs(0.5*zdet_scroll)-(gfloat)y_rel)/(gfloat)y_draw_height);
			break;

		default:
			break;
	}
}

void change_x_end(gint x_rel, gint y_rel)
{
	int x_draw_width = 0;
	int y_draw_height = 0;
	y_rel -= 3.5*border;
	switch ((DisplayMode)mode)
	{
		case LAND_3D:
			x_rel -= border + x3d_scroll/2;
			x_draw_width = width - abs(x3d_scroll)-2*border;
			y_draw_height = height - abs(z3d_scroll)-2*border;
			break;
		case SPIKE_3D:
			x_rel -= border;
			x_draw_width = width - abs(xdet_scroll)-2*border;
			y_draw_height = height - abs(zdet_scroll)-2*border;
			break;
		default:
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
	switch ((DisplayMode)mode)
	{
		case LAND_3D:
			x3d_end = abs(abs(0.5*x3d_scroll)-(gfloat)x_rel)/(gfloat)x_draw_width;
			y3d_end = 1.0-(abs(abs(0.5*z3d_scroll)-(gfloat)y_rel)/(gfloat)y_draw_height);
			break;
		case SPIKE_3D:
			xdet_end = abs(abs(0.5*xdet_scroll)-(gfloat)x_rel)/(gfloat)x_draw_width;
			ydet_end = 1.0-(abs(abs(0.5*zdet_scroll)-(gfloat)y_rel)/(gfloat)y_draw_height);
			break;
		default:
			break;
	}

}
