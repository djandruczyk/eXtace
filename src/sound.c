/*
 * sound.c extace source file
 * 
 * /GDK/GNOME sound (esd) system output display program
 * 
 * Copyright (C) 1999 by Dave J. Andruczyk 
 * 
 * Based on the original extace written by The Rasterman and Michael Fulbright
 *  
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

#ifdef HAVE_ALSA_05
#include <sys/asoundlib.h>
#endif

/*--- globals to this file */
#ifdef HAVE_ALSA_05
snd_pcm_loopback_t *alsa_handle;
snd_pcm_format_t *format;
#endif
GtkWidget *errbox;
int errorbox_up;
/* Experimental variables to get threading to behave... */

static int read_started = 0;
pthread_t esound_thread;
gint esd_handle = 0;
int esd_locked = 0;
int bytes_2_move = 0;
int bytes_moved = 0;

#ifdef HAVE_ALSA_05
int alsa_locked = 0;
pthread_t alsa_thread;
#endif

/*--- globals to this file */


int open_sound(void)
{
    int handle = -1;
#ifdef HAVE_ALSA_05
    int err = 0;
    format = malloc(sizeof(snd_pcm_format_t));
    memset((void *)format, 0, sizeof(snd_pcm_format_t));
#endif


    switch (sound_source)
    {
	case ESD:
	    esd_handle=esd_monitor_stream(ESD_BITS16|ESD_STEREO|ESD_STREAM|ESD_MONITOR,RATE,NULL,"extace");
	    if (esd_handle)
	    {
		handle = 1;
		read_started = 0;
	    }

	    break;


#ifdef HAVE_ALSA_05
	case ALSA:
#ifdef ALSA_DEBUG
	    printf("opening alsa device \n");
#endif
	    err = snd_pcm_loopback_open(&alsa_handle, alsa_card, alsa_device, alsa_sub_dev, SND_PCM_LB_OPEN_PLAYBACK);
	    if (err <0)
	    {
		printf("open failed: %s\n", snd_strerror(err)); 
		handle = -1;
	    }
	    else
	    {
		handle = 1;
#ifdef ALSA_DEBUG
		printf("ALSA loopback device opened successfully\n");
#endif
		snd_pcm_loopback_block_mode(alsa_handle, 1);
		format->format = SND_PCM_SFMT_S16_LE;
		format->rate = RATE;
		format->voices = 2;
		err = snd_pcm_loopback_format(alsa_handle, format);
		if (err <0)
		{
		    printf("format info failed: %s\n", snd_strerror(err)); 
		    handle = -1; 
		}
#ifdef ALSA_DEBUG
		else
		{
		    printf("ALSA format setup worked OK\n");
		    printf("rate %i, channels %i\n",format->rate, format->voices);
		}
#endif
	    }
	    g_free(format);
	    break;
#endif

    }
    if (handle < 0)
    {
	GtkWidget *label;
	if (errorbox_up)
	    return(-1); /* ERROR window already onscreen */
	errbox = gtk_window_new(GTK_WINDOW_DIALOG);
	
	gtk_window_set_title(GTK_WINDOW(errbox),"ERROR!!!");
	label = gtk_label_new("Error, Cannot connect to Sound source!!\n. PLease make sure you havethe proper setting in the options panel.\n");
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
#ifdef HAVE_ALSA_05
	case ALSA:
/*	    printf("closing alsa_handle\n");  */
	    snd_pcm_loopback_close(alsa_handle);
	    break;
#endif
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
#ifdef HAVE_ALSA_05
	    case ALSA:
		retcode = pthread_cancel(alsa_thread);
//		if (retcode != 0)
//		    printf("Error attempting cancel main ALSA thread\n");
//		else 
/*		    printf("ALSA main thread stopped successfully\n");  */
		alsa_locked = 0;
		break;
#endif
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
#ifdef HAVE_ALSA_05
	    case ALSA:
		retcode = pthread_create(&alsa_thread,
			NULL,
			alsa_starter_thread,
			alsa_handle);
		if (retcode != 0)
		    printf("Error attempting to create MAIN ALSA thread\n");
		break;
#endif

	    default:
		break;

	}
    }
    read_started = 1;
    return(0);

}
    
#ifdef HAVE_ALSA_05
/* alsa_reader_thread is the callback function fed into ALSA lib from
 * alsa_starter_thread.  It is called whenever data is ready to be read
 * from the ALSA loopback buffer
 */
void alsa_reader_thread(void *private_data, char *buffer, size_t count)
{
    static gint last;

    /* set predicate to block any other threads */
    alsa_locked = 1;
    bytes_2_move = count;
    bytes_moved = 0;
    /* ring_pos is in ELEMENTS, NOT BYTES!!!!, since the buffer is of shorts
     * incrementing ring_pos by 1 actually moves 2 bytes ahead */

    if (ring_pos+(bytes_2_move/2) > ring_end) 
    {
	/* fill up to end of ring buffer, but don't jump boundary */
	memcpy(audio_ring + ring_pos,
		buffer,
		(ring_end - ring_pos)*2); /*need bytes, not elements */
	bytes_moved = (ring_end - ring_pos)*2;

	/* wrap to beginning */
	memcpy(audio_ring,
		buffer + bytes_moved,
		bytes_2_move - bytes_moved);
	/* mark where we are .. */
	ring_pos = (bytes_2_move - bytes_moved)/2; /* need elements NOT bytes*/
    }         
    else
    {
	memcpy(audio_ring + ring_pos,
		buffer,
		bytes_2_move);
	ring_pos += bytes_2_move/2; /* mark where we are in ringbuffer */
    } 


    audio_arrival_last = audio_arrival;
    gettimeofday(&audio_arrival, NULL);

    if (gdk_window_is_visible(buffer_area->window))
    {
	/* Only draw it if its visible.  Why waste CPU time ??? */
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

    //    printf("Moved %i bbytes of input data\n",count);

    //    printf("Last AUDIO at %f, current at %f, diff %fms\n",audio_arrival_last.tv_sec +(double)audio_arrival_last.tv_usec/1000000, audio_arrival.tv_sec +(double)audio_arrival.tv_usec/1000000,((audio_arrival.tv_sec +(double)audio_arrival.tv_usec/1000000)-(audio_arrival_last.tv_sec +(double)audio_arrival_last.tv_usec/1000000))*1000);
    /* unset predicate */
    alsa_locked = 0;
}
#endif

/* esd_reader_thread isn't a true thread as it runs in the GTK main lock
 * context, its called by the gtk main loop whenever data becomes available 
 * on the esd filedescriptor.
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
	    memcpy(audio_ring,
		    incoming_buf + bytes_moved,
		    bytes_2_move - bytes_moved);
	    /* mark where we are .. */
	    ring_pos =  (bytes_2_move - bytes_moved)/2; /* need elements not BYTES */

	}
	else
	{
	    memcpy(audio_ring+ring_pos,
		    incoming_buf,
		    bytes_2_move);
	    ring_pos += bytes_2_move/2; /*mark where we are in ringbuffer*/
	}                  
    }

    audio_arrival_last = audio_arrival;
    gettimeofday(&audio_arrival, NULL);

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
//    printf("Moved %i bbytes of input data\n",count);

//    printf("Last at %f, current at %f, diff %fms\n",audio_arrival_last.tv_sec +(double)audio_arrival_last.tv_usec/1000000, audio_arrival.tv_sec +(double)audio_arrival.tv_usec/1000000,((audio_arrival.tv_sec +(double)audio_arrival.tv_usec/1000000)-(audio_arrival_last.tv_sec +(double)audio_arrival_last.tv_usec/1000000))*1000);
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

#ifdef HAVE_ALSA_05

void *alsa_starter_thread(void * handle)
{
    /* Creates the ALSA callback handler for ALSA, then sleeps indefinitely */
    int retcode=0;
    pthread_t callback_thread;
    snd_pcm_loopback_callbacks_t callbacks;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.data = alsa_reader_thread;
    callbacks.position_change = loopback_position_change;
    callbacks.silence = loopback_silence;
    callbacks.format_change = loopback_format_change;
    callbacks.max_buffer_size = callback_buffer_size;

    snd_pcm_loopback_read(handle, &callbacks);
    if (keep_reading == 1)
    {
	/*    printf("loopback died for some stupid reason, restarting\n"); */
	retcode = pthread_create(&callback_thread, NULL, alsa_starter_thread,alsa_handle);
    }
    /*	#printf("alsa shutting down, don't restart callback thread \n"); */
	return(0);
}

void loopback_position_change(void *private_data,unsigned int pos) 
{
    printf("Alsa POSITION change callback called, doing nothing\n");
}

void loopback_format_change(void *private_data,snd_pcm_format_t *format) 
{
    printf("Alsa FORMAT change callback called, doing nothing\n");
}

void loopback_silence(void *private_data,size_t count) 
{
    printf("Alsa silence callback called, doing nothing\n");
}
#endif

void error_close_cb(GtkWidget *widget, gpointer *data)
{
      printf("Cannot connect to sound source, check options.\n");
	gtk_widget_destroy(errbox);
}

void alsa_adjust(GtkWidget *widget, gpointer *data)
{
    switch ((gint)data)
    {
	case ALSA_CARD:
	    alsa_card = GTK_ADJUSTMENT(widget)->value;
	    break;
	case ALSA_DEVICE:
	    alsa_device = GTK_ADJUSTMENT(widget)->value;
	    break;
	case ALSA_SUB_DEV:
	    alsa_sub_dev = GTK_ADJUSTMENT(widget)->value;
	    break;
	default:
	    break;
    }
    if (sound_source == ALSA)
    {
	keep_reading = 0;
	audio_thread_stopper();
	usleep(2000);
	close_sound();
	keep_reading = 1;
	if (open_sound() >= 0)
	    audio_thread_starter();
    }
}

