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

#ifndef __SOUND_H__
#define __SOUND_H__

#include <gtk/gtk.h>

/* Prototypes */
int audio_thread_starter(void);
int audio_thread_stopper(void);
void *esd_starter_thread(void * );
void esd_reader_thread(gpointer , gint , GdkInputCondition );
int open_sound(void);
void close_sound(void);
void error_close_cb(GtkWidget *, gpointer * );
/* Prototypes */

#endif
