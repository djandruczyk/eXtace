/*
 * Copyright (C) 2003 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux eXtace Audio Visualizer
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute, etc. this as long as all the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <gtk/gtk.h>

/* Prototypes */
void leave(GtkWidget *, gpointer *);
gint close_dir_win(GtkWidget *, gpointer *);
gint close_grad_win(GtkWidget *, gpointer *);
gint close_options(GtkWidget *, gpointer *);
gint slider_changed(GtkWidget *, gpointer *);
gint button_handle(GtkWidget *, gpointer *);
gint change_display(GtkWidget *, gpointer *);
gint set_decimation_factor(GtkWidget *, gpointer *);
gint scope_mode(GtkWidget *, gpointer *);
/* Prototypes */

#endif
