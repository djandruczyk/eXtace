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

#ifndef __INPUT_H__
#define __INPUT_H__

#include <gtk/gtk.h>

typedef gshort ring_type;

/* 
   Data ring buffer is shared globally,
   perhaps this should be turned into a structure. 

   The idea here is that the calling program sets the
   size and allocates memory for ringbuffer.

   The input routines set ring_channels and ring_rate.
*/

ring_type *ringbuffer; /* Array of raw audio data from input source */
int ring_end;          /* size of ring buffer in total samples */
int ring_pos;          /* offset in ringbuffer that is most recently updated*/
int ring_channels;     /* number of channels being read into input ring */
float ring_rate;       /* samples read per second in each channel */
                       /* eventually, this should take over the functionality
		       of RATE */
/* Prototypes */
int input_thread_starter(int );
int input_thread_stopper(int );
void *input_reader_thread(void *);
int open_datasource(DataSource );
int close_datasource(int );
void error_close_cb(GtkWidget *, gpointer * );

/* Prototypes */



#endif


