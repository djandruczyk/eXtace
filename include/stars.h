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

#ifndef __STARS_H__
#define __STARS_H__

#include <gtk/gtk.h>

/* Prototypes */
void kt_stars_update_func(GtkWidget *);
void kt_stars_stop(GtkWidget *);
void kt_stars_start(GtkWidget *, gint , gint );
GtkWidget * kt_stars_new(GtkWidget *, GdkPixmap *);
void kt_stars_set_logo_pixmp(GtkWidget *, GdkPixmap *, GdkPixmap *);
/* Prototypes */


#endif
