/*
 * Copyright (C) 2003 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux eXtace Audio visualizer
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute, etc. this as long as all the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#ifndef __EVENTS_H__
#define __EVENTS_H__

#include <gtk/gtk.h>

/* Prototypes */
gint configure_event(GtkWidget *, GdkEventConfigure *, gpointer );
gint expose_event(GtkWidget *, GdkEventExpose *, gpointer );
gint button_notify_event (GtkWidget *, GdkEventButton *, gpointer );
gint motion_notify_event (GtkWidget *, GdkEventMotion *, gpointer );
gint test_on_line(int, int);
gint test_if_close(int, int);
void change_spec_start(gint);
void change_x_start(gint,gint);
void change_x_end(gint,gint);
/* Prototypes */

#endif
