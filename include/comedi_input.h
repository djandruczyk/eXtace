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

#ifndef __COEMDI_INPUT_H__
#define __COMEDI_INPUT_H__

#include <config.h>
#include <gtk/gtk.h>
#ifdef HAVE_COMEDI
#include <comedilib.h>
#endif

/* Prototypes */
/* Lame initialization function */
#ifdef HAVE_COMEDI
int prepare_cmd_lib(comedi_t *dev,int *subdevice, comedi_cmd *cmd);
#endif
GtkWidget *comedi_device_control_open(int input_handle);
int comedi_device_control_close(GtkWidget *widget, gpointer *dummy);
/* Prototypes */

#endif











