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
		switch (sound_source)/* hopefully we'll have more than 1 soon*/
		{
			case ESD:
				/* since we want to audio to NOT be read in 
				 * the main gtk loop we'll use the gtk 
				 * input function as a helper. It will awaken
				 * our esound reader thread whenever data 
				 * is ready. Save us from having to setup 
				 * a poll loop by hand, as gdk/gtk does 
				 * it pretty well already 
				 */
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
	int bytes_to_read = 0;

	/* set predicate to block other thread */
	esd_locked = 1;
	/* Things may look funky, but I had to do it this way because of the
	 * buffer used. (Short int). Incrementing a short int pointer 
	 * actually moves you TWO bytes ahead, due to the 16 bit nature
	 * of a short int.  The trouble is the read() system call uses
	 * bytes only, so we use some intermediates to shift between buffer
	 * ELEMENTS and bytes. (2:1 ratio)
	 */

	/* handle the condition of possible buffer overflow */
	
	read:
	if ((elements_to_get + ring_pos) > ring_end)
	{
		/*	printf("WRAP section, two part read\n"); */
		/* Need to read in two parts */
		bytes_to_read = (ring_end - ring_pos)*2;
		count = read(source,audio_ring + ring_pos,bytes_to_read);
		/*	printf("LOOP requesting %i bytes,  Read %i bytes to ring_position %i\n",bytes_to_read,count,ring_pos); */

		if (count < bytes_to_read)
		{
			/* printf("Short read, %i bytes\n",count); */
			/* printf("bytes_to_read= %i\n",bytes_to_read); */
			elements_to_get  -= (count/2);
			ring_pos += (count/2);	/*ELEMENTS not bytes */
			goto read;
		}
		else if (count == bytes_to_read)
		{
			/* printf("Clean fill to end of Ring, wrapping\n"); */
			elements_to_get -= (count/2);
			ring_pos = 0; /* WRAP */
			/* printf("read in %i bytes to fill ring \n",count); */
		}
		else	/* over-run */
		{
			printf("read overrun past end of ring, FAULT!!!\b\n"); 
			exit (-3);
		}
		bytes_to_read = elements_to_get*2;
		count = read(source,audio_ring,bytes_to_read);
		/*printf("LOOP read %i bytes to ring_position %i\n",count,ring_pos); */
		ring_pos = (count/2);	/*ELEMENTS not bytes */
		elements_to_get = nsamp/2;
		/* printf("Wrap complete, read in %i more bytes\n",count); */
	}
	else 	/*normal read, no risk of wrapping the buffer */
	{
		bytes_to_read = elements_to_get*2;
		/* printf("Requesting %i bytes\n",bytes_to_read); */
		count = read(source,audio_ring + ring_pos,bytes_to_read);
		/* printf("NORM read %i bytes to ring_position %i\n",count,ring_pos); */
		if (count == bytes_to_read)
		{
			/* printf("Full good read\n"); */
			ring_pos += (count/2);	/*ELEMENTS not bytes */
			elements_to_get = nsamp/2;
			/* printf("Normal read complete, read in %i more bytes\n",count); */
		}
		else if (count > bytes_to_read)
		{
			printf("BUG\b More data came in than requested, Oh shit!!!\n");
			exit (-3);
		}
		else
		{
			/* printf("Partial good read\n"); */
			ring_pos += (count/2);	/*ELEMENTS not bytes */
			elements_to_get = nsamp/2 ;
			/* printf("Partial read complete, read in %i more bytes\n",count); */
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

