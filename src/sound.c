/*
 * sound.c extace source file
 * 
 * esd (Esound) sound monitor program
 * 
 * Copyright (C) 1999 by Dave J. Andruczyk 
 * 
 * Based on the original extace written by The Rasterman and Michael Fulbright
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#include <config.h>
#include <globals.h>
#include <protos.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include <esd.h>
#include <asm/errno.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

/*--- globals to this file */
GtkWidget *errbox;
int errorbox_up;
/* Experimental variables to get threading to behave... */

static int read_started = 0;
pthread_t esound_thread;
gint esd_handle = 0;
int esd_locked = 0;
int bytes_2_move = 0;
int bytes_moved = 0;

/*--- globals to this file */


int open_sound(void)
{
	int handle = -1;

	switch (sound_source)
	{
		case ESD:
			esd_handle=esd_monitor_stream(ESD_BITS16|ESD_STEREO|ESD_STREAM|ESD_MONITOR,RATE,NULL,"extace");
			if (esd_handle > 0)
			{
				handle = 1;
				read_started = 0;
			}

			break;


	}
	if (handle < 0)
	{
		GtkWidget *label;
		if (errorbox_up)
			return(-1); /* ERROR window already onscreen */
		errbox = gtk_window_new(GTK_WINDOW_DIALOG);

		gtk_window_set_title(GTK_WINDOW(errbox),"ERROR!!!");
		label = gtk_label_new("Error, Cannot connect to Sound source!!\n. PLease make sure you have the proper setting in the options panel.\n");
		gtk_container_add(GTK_CONTAINER(errbox), label);
		gtk_widget_show_all(errbox);

		gtk_signal_connect(GTK_OBJECT(errbox), "delete_event",
				GTK_SIGNAL_FUNC(error_close_cb), NULL);
		gtk_signal_connect(GTK_OBJECT(errbox), "destroy_event",
				GTK_SIGNAL_FUNC(error_close_cb), NULL);
		errorbox_up = 1;
		return(-1);
	}
	else
	{
		plan = rfftw_create_plan(nsamp, FFTW_FORWARD, FFTW_ESTIMATE);
		return(0);
	}
}
void close_sound(void)
{
	switch(sound_source)
	{
		case ESD:
			/*	    printf("closing esd_handle\n");  */
			esd_close(esd_handle);
			break;
		default:
			break;
	}
}
        
int audio_thread_stopper()
{
    int retcode = 0;
    if (read_started)
    {
	switch (sound_source)
	{
	    case ESD:
		gtk_input_remove(tag);
		retcode = pthread_cancel(esound_thread);
//		if (retcode != 0)
//		    printf("Error attempting cancel esd thread\n");
//		else 
/*		    printf("ESD thread stopped successfully\n");  */
		esd_locked = 0;
		break;
	    default:
		break;
	}
    }
    read_started = 0;
    return 0;
}

int audio_thread_starter()
{
	int retcode = 0;
	if (read_started)
		printf("Error, reader already running!!\n");
	else
	{
		switch (sound_source)
		{
			case ESD:
				/* since we want to audio to NOT be read in the main gtk loop
				 * we'll use the gtk input function as a helper. IT will awaken
				 * our esound reader thread whenever data is ready. Save us 
				 * from having to setup a poll loop by hand, as gdk/gtk does
				 * it pretty well already */
				retcode = pthread_create(&esound_thread,
						NULL, /*Thread attributes */
						esd_starter_thread,
						NULL /* args passed to thread */);
				if (retcode != 0)
					printf("Error attempting to create Esound thread\n");
				break;
			default:
				break;

		}
	}
	read_started = 1;
	return(0);

}
    
/* esd_reader_thread isn't a true thread as it runs in the GTK main lock
 * context, its called by the gtk main loop whenever data becomes available 
 * on the esd filedescriptor.
 * I'd like to STOP using gtk's input handler and put that into its own thread
 * so that the sound i/o routines are completely independant of the GUI so
 * that they can be broken out into shared objects easier. (future plans)
 */
void esd_reader_thread(gpointer data, gint source, GdkInputCondition condition)
{
	static gint last;
	int count = 0;

	/* set predicate to block other thread */
	esd_locked = 1;
	bytes_moved = 0;

	/* copy fd so that reader thread can copy data to ringbuffer */
	count = read(source, incoming_buf, to_get); 
	//    printf("%i requested, %i bytes read from Esound\n",to_get,count);
	if (count > 0)
	{
		bytes_2_move = count;
		if (ring_pos+(bytes_2_move/2) > ring_end)
		{

			/* fill up to end of ring buffer, but don't jump boundary */
			memcpy(audio_ring+ring_pos,
					incoming_buf,
					(ring_end - ring_pos)*2); /* bytes NOT elements */

			bytes_moved = (ring_end - ring_pos)*2;

			/* wrap to beginning */
			/* We have to use bytes_moved/2 for the mem address, because
			 * incoming_buf is a SHORT *, thus an index increment of 1 = 2 bytes
			 */
			memcpy(audio_ring,
					incoming_buf + bytes_moved/2, 
					bytes_2_move - bytes_moved);
			/* mark where we are .. */
			ring_pos =  (bytes_2_move - bytes_moved)/2; /* need elements not BYTES */

		}
		else
		{
			memcpy(audio_ring+ring_pos,
					incoming_buf,
					bytes_2_move);
			//	    printf("NOWRAP %i bytes moved to %p\n",bytes_2_move,audio_ring+ring_pos);
			ring_pos += bytes_2_move/2; /*mark where we are in ringbuffer*/
		}                  
	}

	audio_arrival_last = audio_arrival;
	gettimeofday(&audio_arrival, NULL);
	//    printf("Moved %i bytes of input data\n",count);

	//    printf("-- Audio READER: current at %.6f, diff %.2fms\n", audio_arrival.tv_sec +(double)audio_arrival.tv_usec/1000000,((audio_arrival.tv_sec +(double)audio_arrival.tv_usec/1000000)-(audio_arrival_last.tv_sec +(double)audio_arrival_last.tv_usec/1000000))*1000);

	if (gdk_window_is_visible(buffer_area->window))
	{
		// Only draw it if its visible.  Why waste CPU time ??? 
		gdk_threads_enter();

		gdk_draw_rectangle(buffer_pixmap,buffer_area->style->black_gc,
				TRUE,
				last, 20,
				2,15);

		gdk_draw_rectangle(buffer_pixmap,latency_monitor_gc,
				TRUE,
				(float)buffer_area->allocation.width\
				*((float)ring_pos/(float)ring_end), 20,
				2,15);

		last = (float)buffer_area->allocation.width\
			*((float)ring_pos/(float)ring_end);

		gdk_window_clear(buffer_area->window);
		gdk_threads_leave();
	}
	/* unset predicate */
	esd_locked = 0;
}

void *esd_starter_thread(void * params)
{
	/* Creates the GTK input handler for Esound, then evaporates */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

	/*	fcntl(esd_handle, F_SETFL, O_NONBLOCK); */
	tag = gdk_input_add(esd_handle, 
			GDK_INPUT_READ | GDK_INPUT_EXCEPTION,
			esd_reader_thread, NULL);
	return(0);
}

void error_close_cb(GtkWidget *widget, gpointer *data)
{
	printf("Cannot connect to sound source, check options.\n");
	gtk_widget_destroy(errbox);
}

