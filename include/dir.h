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

#ifndef __DIR_H__
#define __DIR_H__

#include <gtk/gtk.h>

/* Prototypes */
gint dir_save_state(GtkWidget *, GdkEventFocus *);
gint update_dircontrol(GtkWidget *);
gint setup_dircontrol(GtkWidget *);
gint dir_motion (GtkWidget *, GdkEventMotion *, gpointer);
gint feed_pointer(gint, gint);
void dir_axis_update(void);
gint update_pointer(void);
/* Prototypes */

#endif
