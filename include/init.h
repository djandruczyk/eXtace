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

#ifndef __INIT_H__
#define __INIT_H__

#include <gtk/gtk.h>

/* Prototypes */
void init(void);
void read_config(void);
void save_config(GtkWidget *);
void make_extace_dirs(void);
void mem_alloc(void);
void mem_dealloc(void);
void reinit_extace(int );
void ring_rate_changed(void);
/* Prototypes */

 #endif
