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

#ifndef __COMEDI_WINDOW_H__
#define __COMEDI_WINDOW_H__

#include <config.h>
#include <gtk/gtk.h>
#ifdef HAVE_COMEDI
#include <comedilib.h>
#endif

/* Prototypes */

GtkWidget *comedi_device_control_open(int input_handle);

/* Prototypes */

#endif
