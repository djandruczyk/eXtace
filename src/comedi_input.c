/*
 *
 * comedi_input.c extace source file
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
#include <input.h>
#include <comedi_input.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_COMEDI
#define N_CHANS 16  // Maximum possible number of channels
unsigned int chanlist[N_CHANS];  /* this list must be non-volitile */
char *cmdtest_messages[];
char *subdevice_types[];
#endif

#define MAX_STR 999
static struct {
	gchar device_name[255];
	gchar subdevice[MAX_STR][20];
	gchar ranges[MAX_STR][20];
	GtkWidget *handle;     /* window to control input device */
}control_window;  

#ifdef HAVE_COMEDI
static gchar channel_numbers[MAX_STR][10];  /* string containing integers */
char *cmd_src(int src,char *buf);
void get_command_stuff(comedi_t *it,int s);
#endif

#ifdef HAVE_COMEDI

int prepare_cmd_lib(comedi_t *dev,int *subdevice,comedi_cmd *cmd)
{
	int i;
	int ret;
	int aref= AREF_GROUND;
	int range=3;  // which voltage range to use


	/* Fix!! this needs to be chosen from menu. */
	*subdevice = 0; // which subdevice

	/* set channels */
	i=0;
	chanlist[i++]=CR_PACK(8,range,aref);
/*	chanlist[i++]=CR_PACK(9,range,aref);*/
	update_ring_channels(i);


	/* comedi rate is samples per nanosecond for 
	   all channels being read.  
	   Assume rate in samples per second per channel. */
	ring_rate=ESD_DEFAULT_RATE;
	ret = comedi_get_cmd_generic_timed(dev,*subdevice,cmd,
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

	ret=comedi_command_test(dev,cmd);
	/* do it again (this usually isn't needed) */
	ret = comedi_command_test(dev,cmd);

	return ret;
}
#endif

int change_comedi_range(GtkWidget *widget, gpointer *data)
{
	return TRUE;
}


GtkWidget *comedi_device_control_open(int handle_number)
{

#ifdef HAVE_COMEDI
	int i,j;
	int n_subdevices;
	comedi_range *rng;
	comedi_t *it=NULL;
	int chan,n_chans;
	int n_ranges;
	int type;
	gchar *str;
	GSList *group;
	GtkWidget *button;
	GtkWidget *frame;
	GtkWidget *spinner;
        GtkWidget *hbox;
        GtkWidget *vbox2;
        GtkWidget *vbox3;
	GtkWidget *subdevice_frame;
	GtkAdjustment *spinner_adj;
#endif
        GtkWidget *vbox;
        GtkWidget *label;

	control_window.handle = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(control_window.handle),"COMEDI input");
	gtk_window_set_policy(GTK_WINDOW(control_window.handle), 
			      FALSE,		/* allow shrink */
			      TRUE,		/* allow grow */
			      FALSE);		/* auto shrink */
	gtk_signal_connect(GTK_OBJECT(control_window.handle),"delete_event",
			   GTK_SIGNAL_FUNC(comedi_device_control_close),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(control_window.handle),"destroy_event",
			   GTK_SIGNAL_FUNC(comedi_device_control_close),
			   NULL);
	gtk_container_set_border_width(GTK_CONTAINER(control_window.handle),2);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(control_window.handle), vbox);
	if(handle_number==-1)
	{
		label = gtk_label_new("Comedi input not opened");
		gtk_box_pack_start(GTK_BOX(vbox),label,TRUE, TRUE, 0);
	}
	else
	{
#ifdef HAVE_COMEDI
		it=comedi_dev(handle_number);

		/* the strings have to be persistant */
		str=control_window.device_name;
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
			str=control_window.subdevice[i];
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
					button = gtk_toggle_button_new_with_label(str);
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
					str=control_window.ranges[j];
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
				label = gtk_label_new("Channels and ranges");
				gtk_box_pack_start(GTK_BOX(vbox2),label,TRUE, TRUE, 0);
				hbox = gtk_hbox_new(FALSE,0);
				for(chan=0;chan<n_chans;chan++){
					vbox3 = gtk_vbox_new(FALSE,0);
					str=channel_numbers[chan];
					sprintf(str,"%i",chan);
					button = gtk_toggle_button_new_with_label(str);
					gtk_box_pack_start(GTK_BOX(vbox3),button,TRUE, TRUE, 0);
					gtk_signal_connect(
						GTK_OBJECT(button),"toggled",
						GTK_SIGNAL_FUNC(change_comedi_range),
						(gpointer) &j);
					n_ranges=comedi_get_n_ranges(it,i,chan);
					if(n_ranges*n_chans > MAX_STR)
					{
						fprintf(stderr,__FILE__":  Too many ranges in Comedi, %i>%i\n",n_ranges*n_chans,MAX_STR);
						exit(23);
					}
					group=NULL;
					for(j=0;j<n_ranges;j++){
						rng=comedi_get_range(it,i,chan,j);
						str=control_window.ranges[j*n_chans+chan];
						sprintf(str," %+g\n%+g",rng->min,rng->max);
						button = gtk_radio_button_new_with_label(group,str);
						gtk_box_pack_start(GTK_BOX(vbox3),button,TRUE, TRUE, 0);
						gtk_signal_connect(
							GTK_OBJECT(button),"toggled",
							GTK_SIGNAL_FUNC(change_comedi_range),
							(gpointer) &j);
						group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
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
#endif
	}
	
	gtk_widget_show_all(control_window.handle);	
	return control_window.handle;
}

/* This is pretty empty now, maybe later it may free memory
   allocated for window */

int comedi_device_control_close(GtkWidget *widget, gpointer *dummy)
{
	gtk_widget_destroy(GTK_WIDGET(control_window.handle));
	return TRUE;
}

/***************************************************************************

  Routine borrowed from Comedi demo programs

*/

#ifdef HAVE_COMEDI

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
