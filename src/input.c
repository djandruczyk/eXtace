/*
 * input.c extace source file
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

#include <stdio.h>
#include <errno.h>
#include <input.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#ifdef HAVE_PULSEAUDIO
#include <pulse/simple.h>
#include <pulse/error.h>
#endif

static struct {
	int open;        /* device has been successfully opened */
	int read_started;  /* read thread has been successfully sarted */
	/* some devices return pointers, other integers */
	int fd;
	pthread_t input_thread;
	DataSource source;
} handle;

static GtkWidget *errbox;
static int errorbox_up =0;
#ifdef HAVE_PULSEAUDIO
static pa_simple *s = NULL;
#endif
gint tag;		/* Used by gdk_input_* */

/*--- globals to this file */

/* 
   The ringbuffer buffer should be allocated and
   the length ring_end should be set by the calling routine.
*/

/* buffer_area should be constructed by an outside routine */
//GtkWidget *buffer_area = NULL;    
ring_type *ringbuffer = NULL;  /* initialize pointer for realloc later */
int ring_end=0;                /* initialize length to zero */
float ring_rate=-1;             /* initalize rate to nonsense */

int open_datasource(DataSource source)
{
	int i=0;
	gchar * tmpbuf = NULL;
	int error = 0;
	if(sizeof(ring_type) !=sizeof(short))
	{
		fprintf(stderr,__FILE__":  ring_type doesn't"
			" match esound data\n");
		return -1;
	}

	switch (source)
	{
#ifdef HAVE_ESD
		case ESD:
			input_unsigned = FALSE; /* esd gives signed integers */
			/* esd rate is rate for each channel */
			ring_rate=ESD_DEFAULT_RATE;
			update_ring_channels(2);  /* since ESD_STEREO is set */
			handle.fd=esd_monitor_stream(ESD_BITS16|ESD_STEREO|ESD_STREAM|ESD_RECORD,ring_rate,"127.0.0.1","eXtace");
			if (handle.fd > 0) 
				handle.open = 1;
			break;
#endif
#ifdef HAVE_PULSEAUDIO
		case PULSEAUDIO:
			input_unsigned = FALSE; /* esd gives signed integers */
			/* esd rate is rate for each channel */
			ring_rate=ESD_DEFAULT_RATE;
			update_ring_channels(2);  /* since ESD_STEREO is set */
			static const pa_sample_spec ss = {
				.format = PA_SAMPLE_S16NE,
				.rate = 44100,
				.channels = 2
			};
			/* Create the recording stream */
			tmpbuf = g_strdup_printf("eXtace_%i",getpid());
			if (!(s = pa_simple_new(NULL, tmpbuf, PA_STREAM_RECORD, NULL, tmpbuf, &ss, NULL, NULL, &error)))
				fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
			else
				handle.open = 1;
			g_free(tmpbuf);
			break;

#endif

		default:
			fprintf(stderr,__FILE__":  This kind of input has not been implemented, can't open.\n");
			break;
	}
	
	if (handle.open)
	{
		handle.source=source;
		handle.read_started = 0;
		return i;
	}
	
	/* The rest is error handling */

	if (errorbox_up)
		return(-1); /* ERROR window already onscreen */

	errbox = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,GTK_BUTTONS_OK,
			"Error, Cannot connect to input source!!\nPLease check settings in the options panel.\n");


	gtk_dialog_run(GTK_DIALOG(errbox));
	gtk_widget_destroy(errbox);
//	gtk_signal_connect(GTK_OBJECT(errbox), "delete_event",
//			GTK_SIGNAL_FUNC(error_close_cb), NULL);
//	gtk_signal_connect(GTK_OBJECT(errbox), "destroy_event",
//			GTK_SIGNAL_FUNC(error_close_cb), NULL);
//	errorbox_up = 1;

	return -1;  /*error return */
}


int input_thread_starter(int i)
{
	int err = -1;  


	if (handle.read_started)  /* This should not happen */
	{
		fprintf(stderr,__FILE__"Error, input reader thread already running!!\n");
		return -1;
	}

	/*
	   since we want input to NOT be read in the main gtk loop,
	   we'll use the gtk input function as a helper. It will awaken
	   our esound reader thread whenever data is ready. Save us from 
	   having to setup a poll loop by hand, as gdk/gtk does 
	   it pretty well already.
	 */

	switch (handle.source)
	{
		case ESD:
			err = pthread_create(&(handle.input_thread),
					NULL, /*Thread attributes */
					input_reader_thread,
					(void *) &handle.fd /*args passed to thread */
					);
			if (err)
				fprintf(stderr,__FILE__":  Error attempting to create input thread\n");
			else
				handle.read_started = 1;
			break;
#ifdef HAVE_PULSEAUDIO
		case PULSEAUDIO:
			err = pthread_create(&(handle.input_thread),
					NULL, /*Thread attributes */
					pa_input_reader_thread,
					(void *) &s /*args passed to thread */
					);
			if (err)
				fprintf(stderr,__FILE__":  Error attempting to create input thread\n");
			else
				handle.read_started = 1;
			break;
#endif
		default:
			fprintf(stderr,__FILE__":  This kind of input has not been implemented, can't start thread.\n");
					
			break;

	}

	return err;

}

int input_thread_stopper(int i)
{
	int err = -1;

	/* error for stopping without starting */
	if ( !handle.open || !handle.read_started)
	{ 
#if DEBUG 
		fprintf(stderr,__FILE__":  Error stopping thread\n");
#endif
		return -1;  
	}

	switch (handle.source)
	{
		case ESD:
			err=pthread_cancel(handle.input_thread);
			if(err == ESRCH)
				fprintf(stderr,"       Thread for input could not be found\n");
			break;
#ifdef HAVE_PULSEAUDIO
		case PULSEAUDIO:
			err = pthread_cancel(handle.input_thread);
			if(err == ESRCH)
				fprintf(stderr,"       Thread for input could not be found\n");
			err = pthread_join(handle.input_thread,NULL);
			pa_simple_drain(s,&err);
			break;
#endif

		default:
			fprintf(stderr,__FILE__":  This kind of input has not been implemented, can't stop thread.\n");
					
			break;
	}

	handle.read_started = 0;
	return err;
}

int close_datasource(int i)
{
	int err=-1;

	/* error for stopping without starting */
	if(!handle.open) 
	{
#if DEBUG /* debug */
		fprintf(stderr,__FILE__":  Error closing thread\n");
#endif
		return -1; 
	}

	switch(handle.source)
	{
#ifdef HAVE_ESD
		case ESD:
			err=esd_close(handle.fd);
			handle.open=0;
			break;
#endif
#ifdef HAVE_PULSEAUDIO
		case PULSEAUDIO:
			/* For some reason pa_simple_free() hangs */
			//pa_simple_free(s);
			break;
#endif
		default:
			fprintf(stderr,__FILE__":  This kind of input has not been implemented, can't close.\n");
					
			break;
	}


	return err;
}

/*****************************************************************************/

void *input_reader_thread(void *input_handle)
{

	int source=*((int *)input_handle);
	static gint last_marker=0;
	static struct timeval input_arrival_last;
	int count;
	struct pollfd ufds;
	ufds.fd = source;
	ufds.events = POLLIN;
	gint timeo = 100; /* wait 100ms max before timeout */
	gint res = -1;
	gint to_get = 0;

	/* adjust position in ring buffer to be on first channel */
	ring_pos -= ring_pos%ring_channels;
	ring_remainder=0;
	//printf("ring_pos is %p endpoint is %p\n",ringbuffer+ring_pos,ringbuffer+ring_end);

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	/*
	   There should be a "wait for input" line so that this
	   loop is not always hogging CPU?  Does the blocking 
	   read wait without hogging CPU if there is not enough data?

	   Using mmap instead of read would greatly improve speed.

	   In OSS and COMEDI, one can find/set the size of the data buffer.
	   */

	fcntl(source,F_SETFL,O_NONBLOCK); /* Set source to non block I/O */

	while (TRUE)
	{
		pthread_testcancel();
		res = poll(&ufds,1,timeo);
		if (res)  /* Data Arrived */
		{

			to_get = (ring_end-ring_pos)*sizeof(ring_type)-ring_remainder;
			//printf("requesting %i bytes at %p\n",to_get,ringbuffer+ring_pos);
			count = read(source,ringbuffer+ring_pos,to_get);
			if(count < 0)
			{
				continue;
				fprintf(stderr,__FILE__":  input read error, count=%i invalid.\n          ",count);
				perror("input_reader_thread");
				exit (-3);
			}
			//printf("received %i BYTES\n",count);
			/* include partial samples from previous read */
			count += ring_remainder;
			if (count == to_get) /* We read to the end of the ring */
			{
				//printf("We read to the end of the ring\n");
				ring_pos = 0;
				ring_remainder=0;
			}
			else
			{
				//printf("not at the end yet \n");
				ring_pos += count/sizeof(ring_type);
				ring_remainder = count%sizeof(ring_type);
				//printf("ring_position is %p, ring remainder is %i\n",ringbuffer+ring_pos,ring_remainder);
			}

			/* use in debug print */
			input_arrival_last = input_arrival; 
			gettimeofday(&input_arrival, NULL);

#if 0  /* debug prints */
			printf("Moved %i elements of input data\n",count);
			printf("-- Audio READER: current at %.6f, diff %.2fms\n",input_arrival.tv_sec +(double)input_arrival.tv_usec/1000000,((input_arrival.tv_sec +(double)input_arrival.tv_usec/1000000)-(input_arrival_last.tv_sec +(double)input_arrival_last.tv_usec/1000000))*1000);

#endif
		}
		pthread_testcancel();

		/* draw markers in control window "buffer_area" */

		if (buffer_area && gdk_window_is_visible(buffer_area->window))
		{
			gdk_threads_enter();
			gdk_draw_rectangle(buffer_pixmap,
					buffer_area->style->black_gc,TRUE,
					last_marker, 20,
					2,15);

			last_marker=(float)buffer_area->allocation.width
				*(float)ring_pos/(float)ring_end;

			gdk_draw_rectangle(buffer_pixmap,latency_monitor_gc,
					TRUE,
					last_marker, 20,
					2,15);

			gdk_window_clear(buffer_area->window);
			gdk_flush();
			gdk_threads_leave();
		}
	}
}


#ifdef HAVE_PULSEAUDIO
void *pa_input_reader_thread(void *input_handle)
{
	static gint last_marker=0;
	static struct timeval input_arrival_last;
	gint count = 0;
	gint req = 0;
	gint result = 0;
	gint error = 0;
	gint to_get = 0;

	/* adjust position in ring buffer to be on first channel */
	ring_pos -= ring_pos%ring_channels;
	ring_remainder=0;
	//printf("ring_pos is %p endpoint is %p\n",ringbuffer+ring_pos,ringbuffer+ring_end);

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	do
	{
		pthread_testcancel();
		to_get = (ring_end-ring_pos)*sizeof(ring_type)-ring_remainder;
		req = to_get < 10000 ? to_get:10000;
		//printf("requesting %i bytes at %p\n",to_get,ringbuffer+ring_pos);
		result = pa_simple_read(s,ringbuffer+ring_pos,req,&error);
		if(result < 0)
		{
			fprintf(stderr,__FILE__":  input read error, Code: %s, \n          ",pa_strerror(error));

			exit (-3);
		}
		count = req;
//		printf("received %i BYTES\n",count);
		/* include partial samples from previous read */
		count += ring_remainder;
		if (count == to_get) /* We read to the end of the ring */
		{
			//printf("We read to the end of the ring\n");
			ring_pos = 0;
			ring_remainder=0;
		}
		else
		{
			//printf("not at the end yet \n");
			ring_pos += count/sizeof(ring_type);
			ring_remainder = count%sizeof(ring_type);
			//printf("ring_position is %p, ring remainder is %i\n",ringbuffer+ring_pos,ring_remainder);
		}

		/* use in debug print */
		input_arrival_last = input_arrival; 
		gettimeofday(&input_arrival, NULL);

#if 0  /* debug prints */
		printf("Moved %i elements of input data\n",count);
		printf("-- Audio READER: current at %.6f, diff %.2fms\n",input_arrival.tv_sec +(double)input_arrival.tv_usec/1000000,((input_arrival.tv_sec +(double)input_arrival.tv_usec/1000000)-(input_arrival_last.tv_sec +(double)input_arrival_last.tv_usec/1000000))*1000);

#endif
		pthread_testcancel();

		/* draw markers in control window "buffer_area" */

		if (buffer_area && gdk_window_is_visible(buffer_area->window))
		{
			gdk_threads_enter();
			gdk_draw_rectangle(buffer_pixmap,
					buffer_area->style->black_gc,TRUE,
					last_marker, 20,
					2,15);

			last_marker=(float)buffer_area->allocation.width
				*(float)ring_pos/(float)ring_end;

			gdk_draw_rectangle(buffer_pixmap,latency_monitor_gc,
					TRUE,
					last_marker, 20,
					2,15);

			gdk_window_clear(buffer_area->window);
			gdk_flush();
			gdk_threads_leave();
		}
	}while(TRUE);
}
#endif


/*
  Make sure that ring_end is divisible by ring_channels.
  If not, increase length slightly.
  Returns zero on success.
*/
int update_ring_channels(int new)
{
	int remainder;
	if(new==0)
	{
		fprintf(stderr,__FILE__":  Invalid number of input channels %i\n",new);
		return -1;
	}
	remainder=ring_end%new;
	ring_channels=new;
	if(remainder)
	{
		ring_end+=ring_channels-remainder;
		ringbuffer = realloc(ringbuffer,ring_end*sizeof(ring_type));
	}
	return ringbuffer==NULL;
}

void error_close_cb(GtkWidget *widget, gpointer *data)
{
	fprintf(stderr,__FILE__":  Cannot connect to input signal source, check options.\n");
			
	gtk_widget_destroy(errbox);
}

