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
#include <init.h>
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
char *cmdtest_messages[];
char *subdevice_types[];
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

#define MAX_STR 99
#ifdef HAVE_COMEDI
static struct {
	gchar device_name[255];
	gchar subdevice[MAX_STR][20];
	gchar ranges[MAX_STR][20];
	GtkWidget *handle;     /* window to control input device */
}control_window[MAX_HANDLES];  

static gchar channel_numbers[MAX_STR][10];  /* string containing integers */
#endif
GtkWidget *errbox;
int errorbox_up;
gint tag;		/* Used by gdk_input_* */

/*--- globals to this file */

/* 
   The ringbuffer buffer should be allocated and
   the length ring_end should be set by the calling routine.
*/

ring_type *ringbuffer = NULL;  /* initialize pointer for realloc later */
int ring_end=0;                /* initialize length to zero */
float ring_rate=-1;             /* initalize rate to nonsense */

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
			/* esd rate is rate for each channel */
			ring_rate=ESD_DEFAULT_RATE;
			handles[i].esd=esd_monitor_stream(ESD_BITS16|ESD_STEREO|ESD_STREAM|ESD_MONITOR,ring_rate,NULL,"extace");
			update_ring_channels(2); /* since ESD_STEREO is set */
			ring_ptr_size=sizeof(short);
			if (handles[i].esd > 0) 
				handles[i].opened = 1;
			break;
#endif
#ifdef HAVE_COMEDI
		case COMEDI:
#ifdef DEBUG  
			comedi_loglevel(3); /* Set high log level for COMEDI */
#endif
			if((handles[i].dev = comedi_open(comedi_data_device)) == NULL)
				comedi_perror(comedi_data_device);
			else
			{
				comedi_device_control_open(i);
				handles[i].opened = 1;
			}
			ring_ptr_size=sizeof(sampl_t);
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
		ring_rate_changed(); /* Fix all gui controls that depend on
				      * ring_rate (adjustments and such
				      */
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
			comedi_device_control_close(control_window[i].handle,
						    NULL);
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
	static gint last_marker=0;
	int count;
	struct pollfd ufds;
	ufds.fd = source;
	ufds.events = POLLIN;
	gint timeo = 100; /* wait 100ms max before timeout */
	gint res = -1;
	gint to_get = 0;
     
	/* reset data ring buffer */
	ring_pos=0;
	ring_remainder=0;

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	/*
	   There should be a "wait for input" line so that this
	   loop is not always hogging CPU?  Does the blocking 
	   read wait without hogging CPU if there is not enough data?

	   Using mmap instead of read would greatly improve speed.

	   In COMEDI, one can find/set the size of the data buffer.
	 */

	fcntl(source,F_SETFL,O_NONBLOCK); /* Set source to non block I/O */
	
	do
	{
		/* in linux, there are no automatic test points yet */
		pthread_testcancel();
		res = poll(&ufds,1,timeo);
		if (res)  /* Data Arrived */
		{
			to_get = (ring_end-ring_pos)*ring_ptr_size-ring_remainder;
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
				ring_pos += count/ring_ptr_size;
				ring_remainder = count%ring_ptr_size;
			}

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

/*
  Make sure that ring_end is divisible by ring_channels.
  If not, increase length slightly.
*/

int update_ring_channels(int new)
{
	int remainder=ring_end%new;
	ring_channels=new;
	if(remainder)
	{
		ring_end+=ring_channels-remainder;
		ringbuffer = realloc(ringbuffer,ring_end*ring_ptr_size);
	}
	return ringbuffer==NULL;
}

void error_close_cb(GtkWidget *widget, gpointer *data)
{
	fprintf(stderr,__FILE__":  Cannot connect to sound source, check options.\n");
			
	gtk_widget_destroy(errbox);
}

/*************************************************************************/

/* 
   routines to set sound input
*/


/*************************************************************************/

/*
     Routines that interface with the comedi library.
     The particular device parameters are now hard coded.  
     Needs fixing!
*/

#ifdef HAVE_COMEDI

int prepare_cmd_lib(comedi_t *dev,int subdevice,comedi_cmd *cmd)
{
	int i;
	int ret;
	int aref= AREF_GROUND;
	int range=3;  // which voltage range to use

	/* set channels */
	i=0;
	chanlist[i++]=CR_PACK(8,range,aref);
/*	chanlist[i++]=CR_PACK(9,range,aref);*/
	update_ring_channels(i);


	/* comedi rate is samples per nanosecond for 
	   all channels being read.  
	   Assume rate in samples per second per channel. */
	ring_rate=ESD_DEFAULT_RATE;
	ret = comedi_get_cmd_generic_timed(dev,subdevice,cmd,
					   1e9/(ring_rate*ring_channels));
	if(ret<0){
		comedi_perror("comedi_get_cmd_generic_timed\n");
		return ret;
	}

	cmd->chanlist = chanlist;
	cmd->chanlist_len =ring_channels;

	cmd->scan_end_arg = ring_channels;

	//if(cmd->stop_src==TRIG_COUNT)cmd->stop_arg = 1000;
	cmd->stop_src = TRIG_NONE;
	cmd->stop_arg = 0;

	return 0;
}

int change_comedi_range(GtkWidget *widget, gpointer *data)
{
	return TRUE;
}

/*
   This demo reads information about a comedi device and
   displays the information in a human-readable form.
 */

char *cmd_src(int src,char *buf);
void get_command_stuff(comedi_t *it,int s);

int comedi_device_control_open(int handle_number)
{
	int i,j;
	int n_subdevices,type;
	int chan,n_chans;
	int n_ranges;
	comedi_range *rng;
	comedi_t *it=NULL;
	gchar *str;
	GSList *group;
	GtkWidget *button;
	GtkWidget *frame;
	GtkWidget *subdevice_frame;
        GtkWidget *hbox;
        GtkWidget *vbox;
        GtkWidget *vbox2;
        GtkWidget *vbox3;
        GtkWidget *label;
	GtkWidget *comedi_window;
	GtkWidget *spinner;
	GtkAdjustment *spinner_adj;

	
	if(handle_number!=-1)
		it=handles[handle_number].dev;
	
	comedi_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	control_window[handle_number].handle=comedi_window;
	gtk_window_set_title(GTK_WINDOW(comedi_window),"COMEDI input");
	gtk_window_set_policy(GTK_WINDOW(comedi_window), 
			      FALSE,		/* allow shrink */
			      TRUE,		/* allow grow */
			      FALSE);		/* auto shrink */
	/* closing is probelmatic (how to reopen?) */
	gtk_signal_connect(GTK_OBJECT(comedi_window),"destroy_event",
			   GTK_SIGNAL_FUNC(comedi_device_control_close),
			   NULL);
	gtk_container_set_border_width(GTK_CONTAINER(comedi_window),2);

#if 0  /* too many frames */	
	frame = gtk_frame_new("Comedi input device");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
        gtk_container_add(GTK_CONTAINER(comedi_window),frame);
	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
#else
	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(comedi_window), vbox);
#endif	
	if(handle_number==-1)
	{
		label = gtk_label_new("Not opened");
		gtk_box_pack_start(GTK_BOX(vbox),label,TRUE, TRUE, 0);
	}
	else
	{
		/* the strings have to be persistant */
		str=control_window[handle_number].device_name;
		sprintf(str,"Data acquistion board:  %s\n"
			"Driver is %s, %i subdevices",
			comedi_get_board_name(it),
			comedi_get_driver_name(it),
			n_subdevices=comedi_get_n_subdevices(it)
			);
		label = gtk_label_new(str);
		gtk_box_pack_start(GTK_BOX(vbox),label,TRUE, TRUE, 0);

#ifdef DEBUG
		printf("  version code: 0x%06x\n",comedi_get_version_code(it));
#endif	
		
		for(i=0;i<n_subdevices;i++)
		{
			type=comedi_get_subdevice_type(it,i);
			/* Only show subdevices that could serve as input */
			if(type!=1 && type != 3 && type != 5) continue;
			str=control_window[handle_number].subdevice[i];
			sprintf(str,"Subdevice %i, %s",i,subdevice_types[type]);
			subdevice_frame = gtk_frame_new(str);
			gtk_container_set_border_width (GTK_CONTAINER (subdevice_frame), 5);
			gtk_container_add(GTK_CONTAINER(vbox),subdevice_frame);
			vbox2 = gtk_vbox_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(subdevice_frame), vbox2);
			n_chans=comedi_get_n_channels(it,i);

#if DEBUG
			if(!comedi_maxdata_is_chan_specific(it,i)){
				printf("  max data value: %d\n",comedi_get_maxdata(it,i,0));
			}else{
				printf("  max data value: (channel specific)\n");
				for(chan=0;chan<n_chans;chan++){
					printf("    chan%d: %d\n",chan,
					       comedi_get_maxdata(it,i,chan));
				}
			}
#endif

			if(!comedi_range_is_chan_specific(it,i)){

				frame = gtk_frame_new("Channels");
				gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
				hbox = gtk_hbox_new(FALSE,0);
				for(j=0; j<n_chans; j++){
					str=channel_numbers[j];
					sprintf(str,"%i",j);
					button = gtk_radio_button_new_with_label(NULL,str);
					gtk_box_pack_start(GTK_BOX(hbox),button,TRUE, TRUE, 0);
					gtk_signal_connect(
						GTK_OBJECT(button),"toggled",
						GTK_SIGNAL_FUNC(change_comedi_range),
						(gpointer) &j);
				}
				gtk_container_add(GTK_CONTAINER(frame), hbox);
				gtk_box_pack_start(GTK_BOX(vbox2),frame,TRUE, TRUE, 0);

				n_ranges=comedi_get_n_ranges(it,i,0);
				frame = gtk_frame_new("Voltage ranges for all channels");
				gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
				hbox = gtk_hbox_new(FALSE,0);
				group=NULL;
				for(j=0;j<n_ranges;j++){
					rng=comedi_get_range(it,i,0,j);
					str=control_window[handle_number].ranges[j];
					sprintf(str," %+g\n%+g",rng->min,rng->max);
					button = gtk_radio_button_new_with_label(group,str);
					gtk_box_pack_start(GTK_BOX(hbox),button,TRUE, TRUE, 0);	      
					gtk_signal_connect(
						GTK_OBJECT(button),"toggled",
						GTK_SIGNAL_FUNC(change_comedi_range),
						(gpointer) &j);
					group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
				}
				gtk_container_add(GTK_CONTAINER(frame), hbox);
				gtk_box_pack_start(GTK_BOX(vbox2),frame,TRUE, TRUE, 0);
			}
			else
			{
				hbox = gtk_hbox_new(FALSE,0);
				for(chan=0;chan<n_chans;chan++){
					vbox3 = gtk_vbox_new(FALSE,0);
					str=channel_numbers[chan];
					sprintf(str,"%i",chan);
					label = gtk_label_new(str);
					gtk_box_pack_start(GTK_BOX(vbox3),label,TRUE, TRUE, 0);
					n_ranges=comedi_get_n_ranges(it,i,chan);
					if(n_ranges*n_chans > MAX_STR)
					{
						fprintf(stderr,__FILE__":  Too many ranges in Comedi\n");
						exit(23);
					}
					for(j=0;j<n_ranges;j++){
						rng=comedi_get_range(it,i,chan,j);
						str=control_window[handle_number].ranges[j*n_chans+chan];
						sprintf(str," [%g,%g]",rng->min,rng->max);
						label = gtk_label_new(str);
						gtk_box_pack_start(GTK_BOX(vbox3),label,TRUE, TRUE, 0);
					}
					gtk_box_pack_start(GTK_BOX(hbox),vbox3,TRUE, TRUE, 0);
				}
			}
			gtk_box_pack_start(GTK_BOX(vbox2),hbox,TRUE, TRUE, 0);

			frame = gtk_frame_new("Sample rate");
			gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
			hbox = gtk_hbox_new(FALSE,0);
			label = gtk_label_new("samples per second for each channel:");
			gtk_box_pack_start(GTK_BOX(hbox),label,TRUE, TRUE, 0);
			/* These will have to be tweaked, maybe find a way to test allowed values */ 
			spinner_adj = (GtkAdjustment *) gtk_adjustment_new(
				2.500, 0.0, 100000, 1, 0.1, 0.1);
			spinner = gtk_spin_button_new (spinner_adj, 0.001, 5);
			gtk_box_pack_start(GTK_BOX(hbox),spinner,TRUE, TRUE, 0);
			gtk_container_add(GTK_CONTAINER(frame), hbox);
			gtk_container_add(GTK_CONTAINER(vbox), frame);
		
#ifdef DEBUG
			printf("  command:\n");
			get_command_stuff(it,i);
#endif
		}
	}
	
	gtk_widget_show_all(comedi_window);	
	return 0;
}

int comedi_device_control_close(GtkWidget *widget, gpointer *handle)
{
	gtk_widget_destroy(GTK_WIDGET(widget));
	return TRUE;
}

void probe_max_1chan(comedi_t *it,int s)
{
	comedi_cmd cmd;
	char buf[100];

	printf("  command fast 1chan:\n");
	if(comedi_get_cmd_generic_timed(it,s,&cmd,1)<0){
		printf("    not supported\n");
	}else{
		printf("    start: %s %d\n",
			cmd_src(cmd.start_src,buf),cmd.start_arg);
		printf("    scan_begin: %s %d\n",
			cmd_src(cmd.scan_begin_src,buf),cmd.scan_begin_arg);
		printf("    convert: %s %d\n",
			cmd_src(cmd.convert_src,buf),cmd.convert_arg);
		printf("    scan_end: %s %d\n",
			cmd_src(cmd.scan_end_src,buf),cmd.scan_end_arg);
		printf("    stop: %s %d\n",
			cmd_src(cmd.stop_src,buf),cmd.stop_arg);
	}
}

void get_command_stuff(comedi_t *it,int s)
{
	comedi_cmd cmd;
	char buf[100];

	if(comedi_get_cmd_src_mask(it,s,&cmd)<0){
		printf("    not supported\n");
	}else{
		printf("    start: %s\n",cmd_src(cmd.start_src,buf));
		printf("    scan_begin: %s\n",cmd_src(cmd.scan_begin_src,buf));
		printf("    convert: %s\n",cmd_src(cmd.convert_src,buf));
		printf("    scan_end: %s\n",cmd_src(cmd.scan_end_src,buf));
		printf("    stop: %s\n",cmd_src(cmd.stop_src,buf));
	
		probe_max_1chan(it,s);
	}
}


char *cmd_src(int src,char *buf)
{
        buf[0]=0;

        if(src&TRIG_NONE)strcat(buf,"none|");
        if(src&TRIG_NOW)strcat(buf,"now|");
        if(src&TRIG_FOLLOW)strcat(buf, "follow|");
        if(src&TRIG_TIME)strcat(buf, "time|");
        if(src&TRIG_TIMER)strcat(buf, "timer|");
        if(src&TRIG_COUNT)strcat(buf, "count|");
        if(src&TRIG_EXT)strcat(buf, "ext|");
        if(src&TRIG_INT)strcat(buf, "int|");
#ifdef TRIG_OTHER
        if(src&TRIG_OTHER)strcat(buf, "other|");
#endif

        if(strlen(buf)==0){
                sprintf(buf,"unknown(0x%08x)",src);
        }else{
                buf[strlen(buf)-1]=0;
        }

        return buf;
}

/*
  The following are from the comedilib demo directory.
  They should be part of the comedilib API ...
*/

char *cmdtest_messages[]={
        "success",
        "invalid source",
        "source conflict",
        "invalid argument",
        "argument conflict",
        "invalid chanlist",
};

char *subdevice_types[]={
	"unused",
	"analog input",
	"analog output",
	"digital input",
	"digital output",
	"digital I/O",
	"counter",
	"timer",
	"memory",
	"calibration",
	"processor"
};

#endif
