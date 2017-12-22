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
#include <enums.h>

DataSource	data_source;  /* input source of data */
int             data_handle;  /* handle for particular data_source */

/* Prototypes */
void leave(GtkWidget *, gpointer *);
gint close_dir_win(GtkWidget *, gpointer *);
gint close_grad_win(GtkWidget *, gpointer *);
gint close_options(GtkWidget *, gpointer *);
gint scope_sync_source_set(GtkWidget *, gpointer);
gint change_display_mode(GtkWidget *, gpointer);
gint set_data_source();
gint set_window_width(GtkWidget *, gpointer);
gint set_fft_data_to_display(GtkWidget *, gpointer);
gint set_fft_size(GtkWidget *, gpointer);
gint fft_set_axis_type(GtkWidget * , gpointer);
gint slider_changed(GtkWidget *, gpointer);
gint button_handle(GtkWidget *, gpointer);
gint change_display(GtkWidget *, gpointer *);
gint set_decimation_factor(GtkWidget *, gpointer);
gint scope_mode(GtkWidget *, gpointer);
gint update_data_source_name(GtkWidget *widget, gpointer data);

/* Prototypes */

#endif
