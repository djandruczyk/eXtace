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

#include <comedi_input.h>
#include <stdio.h>
#include <errno.h>
#include <input.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#ifdef HAVE_ALSA
#include <sys/ioctl.h>
#endif

#ifdef HAVE_COMEDI
#include <comedilib.h>
/* With several cards, there would be more than one device 
   This needs to be fixed. */
char comedi_data_device[]="/dev/comedi0";
#endif

#define MAX_HANDLES 4
static struct {
	int opened;        /* device has been successfully opened */
	int read_started;  /* read thread has been successfully sarted */
	/* some devices return pointers, other integers */
	int esd;
#ifdef HAVE_ALSA
	snd_pcm_t *alsa;
#endif
#ifdef HAVE_COMEDI
	comedi_t *dev; 
	comedi_cmd cmd;
#endif
	pthread_t input_thread;
	DataSource source;
} handles[MAX_HANDLES];

static GtkWidget *errbox;
static int errorbox_up =0;
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
	int tmp;
#ifdef HAVE_ALSA
	snd_pcm_hw_params_t* hwparams;
#endif

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
		if(sizeof(ring_type) !=sizeof(short))
		{
			fprintf(stderr,__FILE__":  ring_type doesn't"
				" match esound data\n");
			return -1;
		}
		input_unsigned = FALSE; /* esd gives signed integers */
			/* esd rate is rate for each channel */
		ring_rate=ESD_DEFAULT_RATE;
		update_ring_channels(2);  /* since ESD_STEREO is set */
		handles[i].esd=esd_monitor_stream(ESD_BITS16|ESD_STEREO|ESD_STREAM|ESD_MONITOR,ring_rate,NULL,"extace");
		if (handles[i].esd > 0) 
			handles[i].opened = 1;
		break;
#endif
#ifdef HAVE_OSS
	case OSS:
		handles[i].esd=open("/dev/dsp",O_RDONLY|O_NONBLOCK,0);
		if (handles[i].esd < 0){
 			perror("open /dev/dsp");
			break; 
		}
		
		/* Copied from xine-lib audio_oss_out.c */
		/* We wanted non blocking open but now put it back to normal */
		if( fcntl(handles[i].esd, F_SETFL, 
			  fcntl(handles[i].esd, F_GETFL)&~O_NONBLOCK) < 0 )
		{
 			perror("Can't set non-blocking");
			close(handles[i].esd);
			break;
		}

		tmp=1;  /* stereo */
		if(ioctl (handles[i].esd, SNDCTL_DSP_STEREO, &tmp) <0)
		{
			perror("Can't set stereo");
			close(handles[i].esd);
			break;
		}
		update_ring_channels(tmp?2:1); 

		/* don't know if I need to set the blocksize with  
		   SNDCTL_DSP_GETBLKSIZE */
#if 0 /* don't know if I need to set the fragment size */
		if (ioctl(handles[i].esd, SNDCTL_DSP_SETFRAGMENT, &fragsize)==-1)
		{
			perror("Can't set buffer size");
			close(handles[i].esd);
			break;
		}
#endif
		
		tmp = AFMT_S16_NE;
		if(ioctl(handles[i].esd, SNDCTL_DSP_SETFMT, &tmp) == -1
		   || tmp != AFMT_S16_NE)
		{
			fprintf(stderr, "Unable to set 2 bytes, returned"
				" 0x%x !=  0x%x\n  big=0x%x little=0x%x",
				tmp,AFMT_S16_NE,AFMT_S16_BE,AFMT_S16_LE);
			close(handles[i].esd);
			break;
		}
		if(8*sizeof(ring_type) != 16)
		{
			fprintf(stderr,__FILE__":  ring_type doesn't"
				" match esound data\n");
			close(handles[i].esd);
			return -1;
		}
		input_unsigned = FALSE; /*  read signed integers */
				
		tmp = 44100;  /* sample rate */
		if (ioctl (handles[i].esd, SNDCTL_DSP_SPEED, &tmp) == -1)
		{
			fprintf (stderr, "Unable to set audio rate %i\n",tmp);
			close(handles[i].esd);
			break;
		}
		ring_rate=tmp;
		
		handles[i].opened = 1;
		break;
#endif
#ifdef HAVE_ALSA
	case ALSA:
		if(snd_pcm_open (&handles[i].alsa, "plughw:0,0", 
				 SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK) < 0)
		{
			perror("open plughw:0,0");
			break;
		}
		handles[i].esd = -1;  /* no file descripter */

		if(snd_pcm_nonblock (handles[i].alsa, 0) < 0)
		{
			fprintf (stderr, "Unable to set alsa non-blocking\n");
			break;
		}
		
		snd_pcm_hw_params_alloca(&hwparams);
		if (snd_pcm_hw_params_any (handles[i].alsa, hwparams) < 0) 
		{
			fprintf (stderr, "Unable to set hwparams\n");
			break;
		}

		if(snd_pcm_hw_params_set_access (handles[i].alsa, hwparams,
                                        SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
		{
			fprintf (stderr, "Unable to set interleaved\n");
			break;
		}

		if(snd_pcm_hw_params_set_format(handles[i].alsa, hwparams, SND_PCM_FORMAT_S16_LE) < 0)
		{
			fprintf (stderr, "Unable to set 2 byte sample\n");
			break;
		}
		if(8*sizeof(ring_type) != 16)
		{
				fprintf(stderr,__FILE__":  ring_type doesn't"
					" match alsa data\n");
				return -1;
		}
		input_unsigned = FALSE; /* read signed integers */
	
		tmp=44100; 
		if((ring_rate=snd_pcm_hw_params_set_rate_near(
			    handles[i].alsa, hwparams, &tmp, 0)) < 0)
		{
			fprintf (stderr, "Unable to set input rate\n");
			break;
		}
		
		tmp=2;
		if(snd_pcm_hw_params_set_channels (handles[i].alsa, hwparams,tmp) < 0)
		{
			fprintf (stderr, "Unable to set stereo\n");
			break;
		}
		update_ring_channels(tmp);

		/* I have no idea what this is */
		/* I am guessing that it is the number of channels? */
		if (snd_pcm_hw_params_set_periods(handles[i].alsa, hwparams,tmp, 0) < 0) 
		{
			fprintf (stderr, "Unable to set periods?\n");
			break;
		}

		tmp = 2*8192; /* I took this from an example, ?????? */
		if(snd_pcm_hw_params_set_buffer_size (handles[i].alsa, hwparams, tmp) < 0)
		{
			fprintf (stderr, "Unable to set buffer size\n");
			break;
		}

		if(snd_pcm_hw_params (handles[i].alsa, hwparams) < 0)
		{
			fprintf (stderr, "Unable to set device\n");
			break;
		}

		handles[i].opened = 1;
		break;
#endif
#ifdef HAVE_COMEDI
	case COMEDI:
		if(sizeof(ring_type) !=sizeof(sampl_t))
		{
			fprintf(stderr,__FILE__":  ring_type doesn't"
				" match sampl_t\n");
			return -1;
		}
		input_unsigned = TRUE; /* Comedi sampl_t is unsigned */
#ifdef DEBUG  
		comedi_loglevel(3); /* Set high log level for COMEDI */
#endif
		if((handles[i].dev = comedi_open(comedi_data_device)) == NULL)
			comedi_perror(comedi_data_device);
		else
		{
			handles[i].opened = 1;
			/* read config file or set default values */
			if(read_comedi_cmd(&handles[i].cmd,&ring_rate))
				default_comedi_cmd(handles[i].dev,
						   &handles[i].cmd,&ring_rate);
		}
		break;
#endif
	case ARTS:
	case GSTREAMER:
	case JACK:
	default:
		fprintf(stderr,__FILE__":  This kind of input has not been implemented, can't open.\n");
		break;
	}
	
	if (handles[i].opened)
	{
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
	label = gtk_label_new(
		"Error, Cannot connect to input source!!\n"
		"PLease check settings in the options panel.\n");
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
#endif


	if(i<0 || i>=MAX_HANDLES || !handles[i].opened)
	{
		fprintf(stderr,__FILE__": Invalid handle %i in input_thread_starter,  opened=%i\n",i,handles[i].opened);
				
		return -1;
	}

	if (handles[i].read_started)  /* This should not happen */
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

	switch (handles[i].source)
	{
		case ESD:
		case OSS:
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
		case ALSA:
			break;
#ifdef HAVE_COMEDI
		case COMEDI:
                        ret = comedi_command_test(handles[i].dev, 
						  &handles[i].cmd);
			if(ret != 0){
				fprintf(stderr,__FILE__ ":  second COMEDI command_test returned %i\n     See kernel system messages (dmesg)\n",ret);
						
			}
			update_ring_channels(handles[i].cmd.chanlist_len);
			ret = comedi_command(handles[i].dev, &handles[i].cmd);
			if(ret<0){
				fprintf(stderr,__FILE__":  comedi_command failed for handle %i.\n    ",i);
						
				comedi_perror("comedi_command");
				break;
			}
#if 0 /* maybe later add this */
			comedi_lock(handles[i].dev,handels[i].subdevice);
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
		case ESD:
		case OSS:
			err=pthread_cancel(handles[i].input_thread);
			if(err == ESRCH)
				fprintf(stderr,"       Thread for input could not be found\n");
			break;
		case ALSA:
			break;
#ifdef HAVE_COMEDI
		case COMEDI:
			err=pthread_cancel(handles[i].input_thread);
			if(err == ESRCH)
				fprintf(stderr,__FILE__":  Thread for input could not be found.\n");

#if 0 /* maybe later add this */
			comedi_unlock(handles[i].dev,handles[i].cmd.subdev);
#endif
			err=comedi_cancel(handles[i].dev,
					  handles[i].cmd.subdev);
			if(err){
				fprintf(stderr,__FILE__":  comedi_cancel failed\n    ");
				comedi_perror("comedi_cancel");
			}
			break;
#endif
		case ARTS:
		case GSTREAMER:
		case JACK:
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
#ifdef HAVE_OSS
		case OSS:
			err=close(handles[i].esd);
			handles[i].opened=0;
			break;
#endif
#ifdef HAVE_ALSA
		case ALSA:
			err=snd_pcm_close(handles[i].alsa);
			handles[i].opened=0;
			break;
#endif
#ifdef HAVE_COMEDI
		case COMEDI:
			write_comedi_cmd(&handles[i].cmd,ring_rate);
			/* free memory used by handle */
			free_comedi_cmd(&handles[i].cmd);
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
	
	do
	{
		/* in linux, there are no automatic test points yet */
		pthread_testcancel();
		res = poll(&ufds,1,timeo);
		if (res)  /* Data Arrived */
		{
			to_get = (ring_end-ring_pos)*sizeof(ring_type)-ring_remainder;
			count = read(source,ringbuffer+ring_pos,to_get);
			if(count < 0)
			{
				fprintf(stderr,__FILE__":  input read error, count=%i invalid.\n          ",count);
				perror("input_reader_thread");
				exit (-3);
			}
			/* include partial samples from previous read */
			count += ring_remainder;
			if (count == to_get) /* We read to the end of the ring */
			{
				ring_pos = 0;
				ring_remainder=0;
			}
			else
			{
				ring_pos += count/sizeof(ring_type);
				ring_remainder = count%sizeof(ring_type);
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
		

	}while(TRUE);
	
}

#if 0
			to_get = (ring_end-ring_pos)/handels[i].channels;
			count = snd_pcm_readi(source,ringbuffer+ring_pos,to_get);
			
			if(count < 0)
			{
				fprintf(stderr,__FILE__":  ALSA input read error, count=%i invalid.\n          ",count);
				exit (-3);
			}
			if (count == to_get) /* We read to the end of the ring */
				ring_pos = 0;
			else
				ring_pos += count*handels[i].channels;
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

#ifdef HAVE_COMEDI
comedi_t *comedi_dev_pointer(int i)
{
	if(handles[i].opened)
		return handles[i].dev;
	else
		return NULL;
}

comedi_cmd *comedi_cmd_pointer(int i)
{
	if(handles[i].opened)
		return &handles[i].cmd;
	else
		return NULL;
}
#endif
