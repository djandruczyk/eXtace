/*
 *
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

#include <asm/errno.h>
#include <config.h>
#include <enums.h>
#include <fcntl.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <input.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif

#ifdef HAVE_ESD
#include <esd.h>
#endif

#ifdef HAVE_COMEDI
#include <comedilib.h>
/* Fix!!  With several cards, there would be more than one device */
char comedi_data_device[]="/dev/comedi0";
/* Fix!! this needs to be chosen from menu. */
int subdevice=0;  /* subdevice on comedi card */
#define N_CHANS 16  // Maximum possible number of channels
unsigned int chanlist[N_CHANS];  /* this list must be non-volitile */
/* Lame initialization function */
int prepare_cmd_lib(comedi_t *dev,int subdevice,comedi_cmd *cmd);
#endif

#define MAX_HANDLES 4
static struct {
	int opened;        /* device has been successfully opened */
	int read_started;  /* read thread has been successfully sarted */
	/* some devices return pointers, other integers */
#ifdef HAVE_ESD
	int esd;
#endif
#ifdef HAVE_COMEDI
	comedi_t *dev; 
#endif
	pthread_t input_thread;
	DataSource source;
} handles[MAX_HANDLES];

GtkWidget *errbox;
int errorbox_up;
gint tag;		/* Used by gdk_input_* */

/*--- globals to this file */


int open_datasource(DataSource source)
{
	int i=0;

	while(handles[i].opened && i<MAX_HANDLES)i++;
	if(i==MAX_HANDLES)
	{
		fprintf(stderr,__FILE__":  Error! Ran out of handles\n");
		return -1;
	}

	switch (source)
	{
#ifdef HAVE_ESD
		case ESD:
			handles[i].esd=esd_monitor_stream(ESD_BITS16|ESD_STEREO|ESD_STREAM|ESD_MONITOR,RATE,NULL,"extace");
			if (handles[i].esd > 0) handles[i].opened = 1;
			break;
#endif
#ifdef HAVE_COMEDI
		case COMEDI:
#ifdef DEBUG  
			comedi_loglevel(3); /* Set high log level for COMEDI */
#endif
			if((handles[i].dev = comedi_open(comedi_data_device)) == NULL)
			{
				comedi_perror(comedi_data_device);
				break;
			}
			handles[i].opened = 1;
			break;
#endif
		case ARTS:
		case GSTREAMER:
		case JACK:
		case ALSA_LIB:
		default:
			fprintf(stderr,__FILE__":  This kind of input has not been implemented, can't open.\n");
			break;
	}

	if (handles[i].opened)
	{
		plan = rfftw_create_plan(nsamp, FFTW_FORWARD, FFTW_ESTIMATE);
		handles[i].source=source;
		handles[i].read_started = 0;
		return i;
	}

	/* The rest is error handling */

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

	return -1;  /*error return */
}


int input_thread_starter(int i)
{
	int err = -1;  
#ifdef HAVE_COMEDI
	int ret;
	comedi_cmd cmd;
#endif


	if(i<0 || i>=MAX_HANDLES || !handles[i].opened)
	{
		fprintf(stderr,__FILE__": Invalid handle %i in input_thread_starter,  opened=%i\n",i,handles[i].opened);
				
		return -1;
	}

	if (handles[i].read_started)  /* This should not happen */
	{
		fprintf(stderr,__FILE__"Error, reader already running!!\n");
		return -1;
	}

	/*
	   since we want to audio to NOT be read in the main gtk loop,
	   we'll use the gtk input function as a helper. It will awaken
	   our esound reader thread whenever data is ready. Save us from 
	   having to setup a poll loop by hand, as gdk/gtk does 
	   it pretty well already.
	 */

	switch (handles[i].source)
	{
#ifdef HAVE_ESD
		case ESD:
			err = pthread_create(&(handles[i].input_thread),
					NULL, /*Thread attributes */
					input_reader_thread,
					(void *) &handles[i].esd /*args passed to thread */
					);
			if (err)
				fprintf(stderr,__FILE__":  Error attempting to create input thread\n");
			else
				handles[i].read_started = 1;
			break;
#endif
#ifdef HAVE_COMEDI
		case COMEDI:
			/* configure and start the comedi device */
			prepare_cmd_lib(handles[i].dev,subdevice,&cmd);
			ret=comedi_command_test(handles[i].dev,&cmd);
			/* do it again (this usually isn't needed) */
			ret = comedi_command_test(handles[i].dev,&cmd);
			if(ret != 0){
				fprintf(stderr,__FILE__ ":  second COMEDI command_test returned %i\n     See kernel system messages (dmesg)\n",ret);
						
			}
			ret = comedi_command(handles[i].dev,&cmd);
			if(ret<0){
				fprintf(stderr,__FILE__":  comedi_command failed for handle %i.\n    ",i);
						
				comedi_perror("comedi_command");
				break;
			}
#if 0 /* maybe later add this */
			comedi_lock(handles[i].dev,subdevice);
#endif 

			/* start up the reading thread */
			handles[i].esd=comedi_fileno(handles[i].dev);
			err = pthread_create(&(handles[i].input_thread),
					NULL, /*Thread attributes */
					input_reader_thread,
					/*args passed to thread */
					(void *) &(handles[i].esd) 
					);
			if (err)
				fprintf(stderr,__FILE__":  Error attempting to create COMEDI input thread\n");
			else
				handles[i].read_started = 1;
			break;
#endif
		case ARTS:
		case GSTREAMER:
		case JACK:
		case ALSA_LIB:
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
	if (i<0 || i>=MAX_HANDLES || !handles[i].opened || !handles[i].read_started)
	{ 
#if DEBUG 
		fprintf(stderr,__FILE__":  Error stopping thread, handle=%i\n",i);
#endif
		return -1;  
	}

	switch (handles[i].source)
	{
#ifdef HAVE_ESD
		case ESD:
			err=pthread_cancel(handles[i].input_thread);
			if(err == ESRCH)
				fprintf(stderr,"        No thread could be found corresponding "
						"to that\n        specified by the thread ID.\n");
			break;
#endif 
#ifdef HAVE_COMEDI
		case COMEDI:
			err=pthread_cancel(handles[i].input_thread);
			if(err == ESRCH)
				fprintf(stderr,"No thread could be found corresponding to that\nspecified by the thread ID.\n");

#if 0 /* maybe later add this */
			comedi_unlock(handles[i].dev,subdevice);
#endif
			err=comedi_cancel(handles[i].dev,subdevice);
			if(err){
				fprintf(stderr,__FILE__":  comedi_cancel failed\n    ");
				comedi_perror("comedi_cancel");
			}
			break;
#endif
		case ARTS:
		case GSTREAMER:
		case JACK:
		case ALSA_LIB:
		default:
			fprintf(stderr,__FILE__":  This kind of input has not been implemented, can't stop thread.\n");
					
			break;
	}

	handles[i].read_started = 0;
	return err;
}

int close_datasource(int i)
{
	int err=-1;

	/* error for stopping without starting */
	if(i<0 || i>=MAX_HANDLES || !handles[i].opened) 
	{
#if DEBUG /* debug */
		fprintf(stderr,__FILE__":  Error closing thread, handle=%i\n",i);
#endif
		return -1; 
	}

	switch(handles[i].source)
	{
#ifdef HAVE_ESD
		case ESD:
			err=esd_close(handles[i].esd);
			handles[i].opened=0;
			break;
#endif
#ifdef HAVE_COMEDI
		case COMEDI:
			err=comedi_close(handles[i].dev);
			if(err)
			{
				fprintf(stderr,__FILE__":  comedi_close failed\n    ");
				comedi_perror("comedi_close");
			}
			handles[i].opened=0;
			break;
#endif
		case ARTS:
		case GSTREAMER:
		case JACK:
		case ALSA_LIB:
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
	static gint last;
	int count = 0;
	int runcount = 0;
	struct pollfd ufds;
	ufds.fd = source;
	ufds.events = POLLIN;
	gint timeo = 100; /* wait 100ms max before timeout */
	gint res = -1;
	gint to_get = 0;
	static gint ring_ptr_size = sizeof(*audio_ring);
	

	/* reset data ring buffer */
	ring_pos=0;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	/*
	   There should be a "wait for input" line so that this
	   loop is not always hogging CPU.

	   Using mmap instead of read would greatly improve speed.

	   In COMEDI, on gave find/set the size of the data buffer.
	 */

	fcntl(source,F_SETFL,O_NONBLOCK); /* Set source to non block I/O */
	
	do
	{
		/* in linux, there are no automatic test points yet */
		runcount++;
		pthread_testcancel();
		res = poll(&ufds,1,timeo);
		if (res)  /* Data Arrived */
		{
			to_get = (ring_end-ring_pos)*ring_ptr_size;
			count = read(source,audio_ring+ring_pos,to_get);
			if(count < 0 )
			{
				fprintf(stderr,__FILE__":  first read error, count=%i invalid.\n          ",count);
				perror("input_reader_thread");
				exit (-3);
			}
			if (count == to_get) /* We read to the end of the ring*/
				ring_pos = 0;
			else
				ring_pos += count/ring_ptr_size;

			audio_arrival_last = audio_arrival;
			gettimeofday(&audio_arrival, NULL);

#if 0  /* debug prints */
			printf("Moved %i elements of input data\n",count);
			printf("-- Audio READER: current at %.6f, diff %.2fms\n",audio_arrival.tv_sec +(double)audio_arrival.tv_usec/1000000,((audio_arrival.tv_sec +(double)audio_arrival.tv_usec/1000000)-(audio_arrival_last.tv_sec +(double)audio_arrival_last.tv_usec/1000000))*1000);
					 
#endif
		}
		pthread_testcancel();
		if (gdk_window_is_visible(buffer_area->window))
		{
			/* Runcount is used to only update the buffer_area
			 * monitor at about 1/10th the input rate.  This is
			 * doen to be nicer to the Xserver,  since we are 
			 * making gtk_*() calls inside a PTHREAD, i.e. 
			 * outside gtk's main loop. a call to XFlush is 
			 * rewuired to keep updating the window properly.
			 * XFlush is expensive, thus we try to do it less
			 * here..
			 */
			/* if (runcount > 10)  */
			{	
				/* Only draw it if its visible.  
				 * Why waste CPU time ??? 
				 */
				gdk_threads_enter();
				gdk_draw_rectangle(buffer_pixmap,
						buffer_area->style->black_gc,TRUE,
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
				gdk_flush();
				gdk_threads_leave();
				runcount = 0; /* reset counter.... */
			}
		}

	}while(TRUE);

}

void error_close_cb(GtkWidget *widget, gpointer *data)
{
	fprintf(stderr,__FILE__":  Cannot connect to sound source, check options.\n");
			
	gtk_widget_destroy(errbox);
}



/*************************************************************************/

/*
     Routines that interface with the comedi library.
     The particular device parameters are now hard coded.  
     Needs fixing!
*/

#ifdef HAVE_COMEDI

int prepare_cmd_lib(comedi_t *dev,int subdevice,comedi_cmd *cmd)
{
	int ret;
	int n_chan=0;  // actual number of channels to read
	int aref= AREF_GROUND;
	int range=1;  // which voltage range to use

	/* comedi rate is samples per nanosecond for all channels */
	ret = comedi_get_cmd_generic_timed(dev,subdevice,cmd,1e9/RATE);
	if(ret<0){
		comedi_perror("comedi_get_cmd_generic_timed\n");
		return ret;
	}

	/* only listen to first two channels */
	n_chan=0;
	chanlist[n_chan++]=CR_PACK(0,range,aref);
	chanlist[n_chan++]=CR_PACK(1,range,aref);


	cmd->chanlist = chanlist;
	cmd->chanlist_len = n_chan;

	cmd->scan_end_arg = n_chan;

	//if(cmd->stop_src==TRIG_COUNT)cmd->stop_arg = 1000;
	cmd->stop_src = TRIG_NONE;
	cmd->stop_arg = 0;

	return 0;
}

#endif
