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

#define N_SUBDEVICE_TYPES 12
char *subdevice_types[N_SUBDEVICE_TYPES];

/* Prototypes */
#ifdef HAVE_COMEDI
int default_comedi_cmd(comedi_t *dev, comedi_cmd *cmd, float *rate);
int read_comedi_cmd(comedi_cmd *cmd, float *rate);
int write_comedi_cmd(comedi_cmd *cmd, float rate);
unsigned int comedi_command_timed(comedi_t *it, comedi_cmd *cmd, 
				  unsigned int ns);
int free_comedi_cmd(comedi_cmd *cmd);
void get_command_stuff(comedi_t *it,int s);
#endif

/* Prototypes */

#endif
