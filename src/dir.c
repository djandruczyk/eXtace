/*
 * dir.c source file for extace
 * 
 * /GDK/GNOME sound (esd) system output display program
 * 
 * Based on the original extace written by The Rasterman and Michael Fulbright
 *  
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#include <config.h>
#include <dir.h>
#include <enums.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <math.h>

/* See globals.h for variable declarations and DEFINES */
gfloat x_disp;         /* X displacement */
gfloat y_disp;         /* Y displacement */
gfloat old_x_disp;     /* X displacement */
gfloat old_y_disp;     /* Y displacement */
gfloat land_axis_angle;/* angle of 3D axis in degrees */
gfloat det_axis_angle; /* angle of 3D axis in degrees */

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



gint dir_save_state(GtkWidget *widget, GdkEventFocus *event)
{
	int x,y;
	if (!dir_win_present)
		return TRUE;
	if (!event->in)
	{
		gdk_window_get_root_origin((gpointer) dir_win_ptr->window, &x, &y);
		dir_x_origin = x+6;
		dir_y_origin = y+22;
	}

	return FALSE;
}


gint update_dircontrol(GtkWidget *widget)
{
	gdk_draw_rectangle (dir_pixmap,
			widget->style->black_gc,
			TRUE, 0, 0,
			widget->allocation.width,
			widget->allocation.height);
	setup_dircontrol(widget);
	dir_axis_update();
	update_pointer();
	gdk_window_clear(dir_area->window);
	return 0;
}


gint setup_dircontrol(GtkWidget *widget)
{
	/* Top main circle (360 degrees) */
	gdk_draw_arc(dir_pixmap,
			widget->style->white_gc,
			0,  /* FILLED */
			0,
			0,
			widget->allocation.width,
			widget->allocation.height,
			0*64,
			360*64);
	/* Y axis "front half" arc, notice its "lighter" (180 degrees) */
	gdk_draw_arc(dir_pixmap,
			widget->style->white_gc,
			0,  /* FILLED */
			.33*widget->allocation.width,
			0,
			.33*widget->allocation.width,
			widget->allocation.height,
			90*64,
			180*64);
	/* Y axis "back half" arc, notice its "darker" (180 degrees) */
	gdk_draw_arc(dir_pixmap,
			arc_gc,
			0,  /* FILLED */
			.33*widget->allocation.width,
			0,
			.33*widget->allocation.width,
			widget->allocation.height,
			-90*64,
			180*64);
	/* Z axis "front half" arc, notice its "lighter" (180 degrees) */
	gdk_draw_arc(dir_pixmap,
			widget->style->white_gc,
			0,  /* FILLED */
			0,
			.33*widget->allocation.height,
			widget->allocation.width,
			.33*widget->allocation.height,
			0*64,
			-180*64);
	/* Z axis "back half" arc, notice its "darker" (180 degrees) */
	gdk_draw_arc(dir_pixmap,
			arc_gc,
			0,  /* FILLED */
			0,
			.33*widget->allocation.height,
			widget->allocation.width,
			.33*widget->allocation.height,
			0*64,
			180*64);
	return TRUE;
}
gint dir_motion (GtkWidget *widget, GdkEventMotion *event, gpointer data)
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

	if (state & GDK_BUTTON1_MASK)
	{
		feed_pointer(x,y);     /*call checking function */
		update_dircontrol(dir_area);

	}
	return TRUE;
}

gint feed_pointer(gint x, gint y)
{
	float x_percent;
	float z_percent;
	x_percent = 100*((float)x/(float)dir_area->allocation.width);
	z_percent = 100*((float)y/(float)dir_area->allocation.height);
	switch ((DisplayMode)mode)
	{
		case (LAND_3D):
			if (z_percent < 50)
				z3d_scroll = (50 - z_percent)*0.5;
			else
				z3d_scroll = -(z_percent-50)*0.5;

			if (x_percent < 50)
				x3d_scroll = (50 - x_percent)*0.5;
			else
				x3d_scroll = -(x_percent-50)*0.5;
			break;
		case (SPIKE_3D):
			if (z_percent < 50)
				zdet_scroll = (50 - z_percent)*0.5;
			else
				zdet_scroll = -(z_percent-50)*0.5;

			if (x_percent < 50)
				xdet_scroll = (50 - x_percent)*0.5;
			else
				xdet_scroll = -(x_percent-50)*0.5;
			break;
		default:
			break;
	}
	return 0;
}
void dir_axis_update()
{
	switch ((DisplayMode)mode)
	{
		case (LAND_3D):
			x_disp = -(x3d_start-x3d_end)*(width - abs(x3d_scroll)-2*x_border);
			y_disp = -(y3d_start-y3d_end)*(height - abs(z3d_scroll)-2*y_border);
			land_axis_angle = atan2(y_disp, x_disp);
			//g_print("angle is %f degrees\n",land_axis_angle*90/M_PI_2); 
			x_disp = -cos(land_axis_angle)*((float)dir_area->allocation.width/2.0);
			y_disp = sin(land_axis_angle)*((float)dir_area->allocation.height/2.0);
			break;
		case (SPIKE_3D):
			x_disp = -(xdet_start-xdet_end)*(width - abs(xdet_scroll)-2*x_border);
			y_disp = -(ydet_start-ydet_end)*(height - abs(zdet_scroll)-2*y_border);
			det_axis_angle = atan2(y_disp, x_disp);
			//g_print("dir.c det_axis_angle is %f degrees\n",det_axis_angle*90/M_PI_2);  
			x_disp = -cos(det_axis_angle)*((float)dir_area->allocation.width/2.0);
			y_disp = sin(det_axis_angle)*((float)dir_area->allocation.height/2.0);
			break;
		default:
			break;
	}
	/*   g_print("x_disp = %f, y_disp = %f",x_disp, y_disp); */
	gdk_draw_line(dir_pixmap,
			dir_area->style->white_gc,
			dir_area->allocation.width/2, /*center */
			dir_area->allocation.height/2, /*center */
			dir_area->allocation.width/2-x_disp,
			dir_area->allocation.height/2-y_disp);


	gdk_window_clear(dir_area->window);

}

gint update_pointer()
{

	switch ((DisplayMode)mode)
	{
		case (LAND_3D):
			x_disp = ((float)x3d_scroll/25.0)*((float)dir_area->allocation.width/2.0);
			y_disp = ((float)z3d_scroll/25.0)*((float)dir_area->allocation.height/2.0);
			break;
		case (SPIKE_3D):
			x_disp = ((float)xdet_scroll/25.0)*((float)dir_area->allocation.width/2.0);
			y_disp = ((float)zdet_scroll/25.0)*((float)dir_area->allocation.height/2.0);
			break;
		default:
			break;
	}

	gdk_draw_line(dir_pixmap,
			dir_area->style->white_gc,
			dir_area->allocation.width/2, /*center */
			dir_area->allocation.height/2, /*center */
			dir_area->allocation.width/2-x_disp,
			dir_area->allocation.height/2-y_disp);

	gdk_window_clear(dir_area->window);

	return 0;
}
