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

#include <config.h>
#include <enums.h>
#include <fcntl.h>
#include <gtk/gtk.h>  /* needed for error window */
#ifdef HAVE_ESD
#include <esd.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

/* 
   Input data type.  Assume that it is a 2 byte integer.
   Since COMEDI gives samples of type sampl_t = unsigned short,
   we need a type conversion for ringbuffer
*/

typedef short ring_type;
#define INPUT_RING(A) (input_unsigned?((unsigned short *) ringbuffer)[A]: \
                      ((signed short *) ringbuffer)[A])

/* 
   Data ring buffer is shared globally,
   perhaps this should be turned into a structure. 

   The idea here is that the calling program sets the
   size and allocates memory for ringbuffer.

   The input routines set ring_channels and ring_rate.
*/

ring_type *ringbuffer; /* Array of raw audio data from input source */
int input_unsigned;    /* Flag whether input data is unsigned vs. signed */
int ring_end;          /* size of ring buffer in total samples */
int ring_pos;          /* offset of the most recent sample in ringbuffer */
int ring_remainder;    /* if partial sample has been read, leftover bytes */
int ring_channels;     /* number of channels being read into input ring 
			  This should be set with update_ring_channels(...); */
float ring_rate;       /* samples read per second in each channel */

/* time that most recent data in ring buffer was read */
struct timeval input_arrival;

/* Variables needed for updating the input progress monitor window */
/* These must be initialized by routines outside of input.c */
GtkWidget     *buffer_area;   /* Buffer latency display window area */
GdkPixmap     *buffer_pixmap; /* Buffer window backing pixmap pointer */
GdkGC         *latency_monitor_gc; /* Graphics context for Arc in dircontrol */

/* Prototypes */
int input_thread_starter(int );
int input_thread_stopper(int );
void *input_reader_thread(void *);
void *pa_input_reader_thread(void *);
int open_datasource(DataSource);
int close_datasource(int );
int update_ring_channels(int );
void error_close_cb(GtkWidget *, gpointer * );
int open_pavucontrol(void);

/* Prototypes */

#endif
