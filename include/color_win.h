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

#ifndef __COLOR_WIN_H__
#define __COLOR_WIN_H__

#include <gtk/gtk.h>

/* Prototypes */
gint color_event (GtkWidget *, GdkEventButton *, gpointer);
gint color_button(GtkWidget *, gpointer );
void create_initial_colormaps(void);
void save_colormap(GtkWidget *, GtkFileSelection *);
void load_colormap(GtkWidget *, GtkFileSelection *);
void read_colormap(char *);
void update_gradient(GtkWidget *, int );
void init_colortab();
void grad_win_create(void);
gint grad_win_save_state(GtkWidget *, GdkEventFocus *);
void gradient_update();
gint close_fileselection(GtkWidget*, gpointer *);
/* Prototypes */

#endif
