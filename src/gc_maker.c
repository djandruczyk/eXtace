/*
 * gc_maker.c source file for extace
 * 
 * /GDK/GNOME sound (esd) system output display program
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
#include <defines.h>
#include <gtk/gtk.h>

void init_gc(GtkWidget *widget)
{
	GdkColormap     *cmap;
	GdkGCValues values;
	GdkColor arc_color;
	GdkColor graticule_color;
	GdkColor trace_color;
	GdkColor latency_monitor;

	cmap = gtk_widget_get_colormap(widget);

	graticule_color.red = RASTER_COLOR_RED << 8;
	graticule_color.green = RASTER_COLOR_GREEN << 8;
	graticule_color.blue = RASTER_COLOR_BLUE << 8;
	gdk_color_alloc(cmap, &graticule_color);

	values.foreground = graticule_color;
	/*    values.function = GDK_XOR; */

	graticule_gc = gdk_gc_new_with_values(widget->window,
			&values,
			GDK_GC_FOREGROUND);


	trace_color.red = TRACE_COLOR_RED << 8;
	trace_color.green = TRACE_COLOR_GREEN << 8;
	trace_color.blue = TRACE_COLOR_BLUE << 8;
	gdk_color_alloc(cmap, &trace_color);

	values.foreground = trace_color;
	/*    values.function = GDK_XOR; */

	trace_gc = gdk_gc_new_with_values(widget->window,
			&values,
			GDK_GC_FOREGROUND);



	cmap = gtk_widget_get_colormap(options_win_ptr);

	latency_monitor.red = 60000;
	latency_monitor.green = 3000;
	latency_monitor.blue = 3000;
	gdk_color_alloc(cmap, &latency_monitor);

	values.foreground = latency_monitor;
	/*    values.function = GDK_XOR; */

	latency_monitor_gc = gdk_gc_new_with_values(widget->window,
			&values,
			GDK_GC_FOREGROUND);


	cmap = gtk_widget_get_colormap(dir_win_ptr);

	arc_color.red = 30000;
	arc_color.green = 30000;
	arc_color.blue = 30000;
	gdk_color_alloc(cmap, &arc_color);

	values.foreground = arc_color;
	/*    values.function = GDK_XOR; */

	arc_gc = gdk_gc_new_with_values(widget->window,
			&values,
			GDK_GC_FOREGROUND);
}
