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
#include <configfile.h>

#ifdef HAVE_COMEDI
#define N_CHANS 16  // Maximum possible number of channels
char *cmdtest_messages[];
char *subdevice_types[];
#endif


/*  For multiple comedi windows, this would have to be generalized */
#define MAX_STR 32
#define MAX_RANGES 32
#define MAX_SUB 4
/* Definitions specific to a particular comedi window */
static struct {
	gchar device_name[255];
	gchar subdevice[MAX_SUB][20];
	gchar ranges[MAX_STR][20];
	GtkWidget *handle;     /* window to control input device */
}control_window;  


#ifdef HAVE_COMEDI
/* some local variables */
static comedi_cmd *cmd;
static comedi_t *dev;
static int my_handle;

char *cmd_src(int src,char *buf);
void get_command_stuff(comedi_t *it,int s);

/* change comedi subdevice is always called in 
   conjunction with another action (channel or range.) */

int change_comedi_subdevice(GtkWidget *widget, gpointer data)
{
	int subdevice=GPOINTER_TO_INT(data);
	if (GTK_TOGGLE_BUTTON(widget)->active){ /* its pressed */
#if 0
		printf("change_comedi_subdevice %i\n",subdevice);
#endif
		cmd->subdev=subdevice;  /* don't actually do anything */
	}
	return TRUE;
}

int change_comedi_range(GtkWidget *widget, gpointer data)
{
	int ret;
	int range=GPOINTER_TO_INT(data);
	int i;

        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
        {
#if 0
		printf("change_comedi_range %i\n",range);
#endif
		input_thread_stopper(my_handle);
		for(i=0; i<cmd->chanlist_len; i++)
			cmd->chanlist[i]=CR_PACK(
				CR_CHAN(cmd->chanlist[i]),
				range,
				CR_AREF(cmd->chanlist[i]));
		ret=comedi_command_test(dev,cmd);
#ifdef DEBUG
		if(ret != 0){
			fprintf(stderr,__FILE__ ":  fourth COMEDI command_test returned %i\n     See kernel system messages (dmesg)\n",ret);
		}
#endif
		input_thread_starter(my_handle);
	}
	return TRUE;
}

int change_comedi_channel(GtkWidget *widget, gpointer data)
{
	int i;
	int ret;
	int chan=GPOINTER_TO_INT(data);
	int n_chan=cmd->chanlist_len;

	if (GTK_TOGGLE_BUTTON(widget)->active) /* its pressed */
	{
		printf("change_comedi_channel:  add %i\n",chan);
		input_thread_stopper(my_handle);
		cmd->chanlist=realloc(cmd->chanlist,(n_chan+1)*sizeof(cmd->chanlist));
		cmd->chanlist[cmd->chanlist_len++]=CR_PACK(
			chan,
			n_chan>0?CR_RANGE(cmd->chanlist[n_chan-1]):0,
			n_chan>0?CR_AREF(cmd->chanlist[n_chan-1]):AREF_GROUND
			);
		ret=comedi_command_test(dev,cmd);
		if(ret != 0){
			fprintf(stderr,__FILE__ ":  third COMEDI command_test returned %i\n     See kernel system messages (dmesg)\n",ret);
			
		}
		input_thread_starter(my_handle);
	}
	else
	{
		printf("change_comedi_channel:  remove %i\n",chan);
		input_thread_stopper(my_handle);
		for(i=0; i<cmd->chanlist_len; i++)
			if(CR_CHAN(cmd->chanlist[i])==chan)break;
		cmd->chanlist_len--;
                for(; i<cmd->chanlist_len; i++)
                        cmd->chanlist[i]=cmd->chanlist[i+1];
		/* don't do anything more if no channels are opened */
		if(cmd->chanlist_len == 0)return TRUE;
		ret=comedi_command_test(dev,cmd);
		if(ret != 0){
			fprintf(stderr,__FILE__ ":  off COMEDI command_test returned %i\n     See kernel system messages (dmesg)\n",ret);
			
		}
		input_thread_starter(my_handle);		
	}
	return TRUE;
}

int change_comedi_rate(GtkAdjustment *adj, gpointer *data)
{
	float rate = adj->value;
	printf("rate %g\n",rate);
	
	return TRUE;
}

#endif

GtkWidget *comedi_device_control_open(int handle_number)
{
        GtkWidget *vbox;
        GtkWidget *label;
#ifdef HAVE_COMEDI
	int k;
	gint j;
	gint subdev;
	int use_sub;
	int n_subdevices;
	comedi_range *rng;
	gint chan;
	int n_chans;
	int n_ranges;
	/* string containing integers */
	static gchar channel_numbers[N_CHANS][10];
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
	GtkObject *spinner_adj;

	for(j=0; j<N_CHANS; j++)
		sprintf(channel_numbers[j],"%i",j);
#endif

	control_window.handle = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(control_window.handle),"COMEDI input");
	gtk_window_set_policy(GTK_WINDOW(control_window.handle), 
			      FALSE,		/* allow shrink */
			      TRUE,		/* allow grow */
			      FALSE);		/* auto shrink */
	gtk_container_set_border_width(GTK_CONTAINER(control_window.handle),2);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(control_window.handle), vbox);
	if(handle_number==-1)
	{
		label = gtk_label_new("\nComedi input not opened\n");
		gtk_box_pack_start(GTK_BOX(vbox),label,TRUE, TRUE, 0);
	}
	else
	{
#ifdef HAVE_COMEDI
		dev=comedi_dev_pointer(handle_number);
		cmd=comedi_cmd_pointer(handle_number);
		my_handle=handle_number;

		/* the strings have to be persistant */
		str=control_window.device_name;
		sprintf(str,"Data acquistion board:  %s\n"
			"Driver is %s, %i subdevices",
			comedi_get_board_name(dev),
			comedi_get_driver_name(dev),
			n_subdevices=comedi_get_n_subdevices(dev)
			);
		label = gtk_label_new(str);
		gtk_box_pack_start(GTK_BOX(vbox),label,TRUE, TRUE, 0);

#ifdef DEBUG
		printf("  version code: 0x%06x\n",comedi_get_version_code(dev));
#endif	
		
		for(subdev=0, use_sub=0; subdev<n_subdevices; subdev++)
		{
			type=comedi_get_subdevice_type(dev,subdev);
			/* Only show subdevices that could serve as input */
			if(type!=1 && type != 3 && type != 5) continue;
			if(use_sub>=MAX_SUB)
			{
				fprintf(stderr,__FILE__":  Too many subdevices\n");
				exit(11);
			}
			str=control_window.subdevice[use_sub];
			sprintf(str,"Subdevice %i, %s",subdev,subdevice_types[type]);
			subdevice_frame = gtk_frame_new(str);
			gtk_container_set_border_width(GTK_CONTAINER(subdevice_frame), 5);
			gtk_container_add(GTK_CONTAINER(vbox),subdevice_frame);
			vbox2 = gtk_vbox_new(FALSE,0);
			gtk_container_add(GTK_CONTAINER(subdevice_frame), vbox2);
			n_chans=comedi_get_n_channels(dev,subdev);
			if(n_chans > N_CHANS)
			{
				fprintf(stderr,__FILE__":  n_chans=%i > N_CHANS=%i\n",
					n_chans,N_CHANS);
				exit(56);
			}
			
#if DEBUG
			if(!comedi_maxdata_is_chan_specific(dev,subdev)){
				printf("  max data value: %d\n",comedi_get_maxdata(dev,subdev,0));
			}else{
				printf("  max data value: (channel specific)\n");
				for(chan=0; chan<n_chans; chan++){
					printf("    chan%d: %d\n",chan,
					       comedi_get_maxdata(dev,subdev,chan));
				}
			}
#endif			

			if(!comedi_range_is_chan_specific(dev,subdev)){

				frame = gtk_frame_new("Channels");
				gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
				hbox = gtk_hbox_new(FALSE,0);
				for(j=0; j<n_chans; j++){
					button = gtk_toggle_button_new_with_label(channel_numbers[j]);
					gtk_box_pack_start(GTK_BOX(hbox),button,TRUE, TRUE, 0);
					for(k=0; k<cmd->chanlist_len; k++)
						if(CR_CHAN(cmd->chanlist[k])==j)
							gtk_toggle_button_set_active(
								GTK_TOGGLE_BUTTON(button),TRUE);
					gtk_signal_connect(
						GTK_OBJECT(button),"toggled",
						GTK_SIGNAL_FUNC(change_comedi_subdevice),
						GINT_TO_POINTER(subdev)
						);
					gtk_signal_connect(
						GTK_OBJECT(button),"toggled",
						GTK_SIGNAL_FUNC(change_comedi_channel),
						GINT_TO_POINTER(j)
						);
				}
				gtk_container_add(GTK_CONTAINER(frame), hbox);
				gtk_box_pack_start(GTK_BOX(vbox2),frame,TRUE, TRUE, 0);

				n_ranges=comedi_get_n_ranges(dev,subdev,0);
				if(n_chans > MAX_RANGES)
				{
					fprintf(stderr,__FILE__":  n_chans=%i > MAX_RANGES=%i\n",
						n_chans,MAX_RANGES);
					exit(56);
				}

				frame = gtk_frame_new("Voltage ranges for all channels");
				gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
				hbox = gtk_hbox_new(FALSE,0);
				group=NULL;

				for(j=0;j<n_ranges;j++){

					rng=comedi_get_range(dev,subdev,0,j);
					str=control_window.ranges[j];
					sprintf(str," %+g\n%+g",rng->max,rng->min);
					button = gtk_radio_button_new_with_label(group,str);
					gtk_box_pack_start(GTK_BOX(hbox),button,TRUE, TRUE, 0);	   
					gtk_toggle_button_set_active(
						GTK_TOGGLE_BUTTON(button),
						CR_RANGE(cmd->chanlist[0])==j
						);
					gtk_signal_connect(
						GTK_OBJECT(button),"toggled",
						GTK_SIGNAL_FUNC(change_comedi_subdevice),
						GINT_TO_POINTER(subdev)
						);
					gtk_signal_connect(
						GTK_OBJECT(button),"toggled",
						GTK_SIGNAL_FUNC(change_comedi_range),
						GINT_TO_POINTER(j)
						);
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
					button = gtk_toggle_button_new_with_label(channel_numbers[chan]);
					gtk_box_pack_start(GTK_BOX(vbox3),button,TRUE, TRUE, 0);
					for(k=0; k<cmd->chanlist_len; k++)
						if(CR_CHAN(cmd->chanlist[k])==chan)
							gtk_toggle_button_set_active(
								GTK_TOGGLE_BUTTON(button),TRUE);
					gtk_signal_connect(
						GTK_OBJECT(button),"toggled",
						GTK_SIGNAL_FUNC(change_comedi_subdevice),
						GINT_TO_POINTER(subdev)
						);
					gtk_signal_connect(
						GTK_OBJECT(button),"toggled",
						GTK_SIGNAL_FUNC(change_comedi_range),
						GINT_TO_POINTER(chan)
						);
					n_ranges=comedi_get_n_ranges(dev,subdev,chan);
					if(n_ranges*n_chans > MAX_STR || n_ranges>MAX_RANGES)
					{
						fprintf(stderr,__FILE__":  Too many ranges in Comedi, %i>%i,MAX_RANGES=%i\n",n_ranges*n_chans,MAX_STR,MAX_RANGES);
						exit(23);
					}
					group=NULL;
					for(j=0;j<n_ranges;j++){
						rng=comedi_get_range(dev,subdev,chan,j);
						str=control_window.ranges[j*n_chans+chan];
						sprintf(str," %+g\n%+g",rng->max,rng->min);
						button = gtk_radio_button_new_with_label(group,str);
						gtk_box_pack_start(GTK_BOX(vbox3),button,TRUE, TRUE, 0);
					for(k=0; k<cmd->chanlist_len; k++)
						if(CR_CHAN(cmd->chanlist[k])==chan)
							gtk_toggle_button_set_active(
								GTK_TOGGLE_BUTTON(button),CR_RANGE(cmd->chanlist[k])==j);
						gtk_signal_connect(
							GTK_OBJECT(button),"toggled",
							GTK_SIGNAL_FUNC(change_comedi_subdevice),
							GINT_TO_POINTER(subdev)
							);
						gtk_signal_connect(
							GTK_OBJECT(button),"toggled",
							GTK_SIGNAL_FUNC(change_comedi_range),
							GINT_TO_POINTER(chan)
						);
						gtk_signal_connect(
							GTK_OBJECT(button),"toggled",
							GTK_SIGNAL_FUNC(change_comedi_range),
							GINT_TO_POINTER(j)
							);
						group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
					}
					gtk_box_pack_start(GTK_BOX(hbox),vbox3,TRUE, TRUE, 0);
				}
				gtk_box_pack_start(GTK_BOX(vbox2),hbox,TRUE, TRUE, 0);
			}

			frame = gtk_frame_new("Sample rate");
			gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
			hbox = gtk_hbox_new(FALSE,0);
			label = gtk_label_new("samples per second for each channel:");
			gtk_box_pack_start(GTK_BOX(hbox),label,TRUE, TRUE, 0);
			/* These will have to be tweaked, maybe find a way to test allowed values */ 
			spinner_adj = gtk_adjustment_new(
				ring_rate, 0.0, 100000, ring_rate/100,
				ring_rate/10, 0);
			spinner = gtk_spin_button_new(
				GTK_ADJUSTMENT(spinner_adj), 0.001, 5);
			gtk_signal_connect(
				GTK_OBJECT(spinner_adj),"value_changed",
				GTK_SIGNAL_FUNC(change_comedi_rate),
				NULL
				);

			gtk_box_pack_start(GTK_BOX(hbox),spinner,TRUE, TRUE, 0);
			gtk_container_add(GTK_CONTAINER(frame), hbox);
			gtk_container_add(GTK_CONTAINER(vbox), frame);
		
#ifdef DEBUG
			printf("  command:\n");
			get_command_stuff(dev,subdev);
#endif
			use_sub++;
		}
#endif
	}
	
	gtk_widget_show_all(control_window.handle);	
	return control_window.handle;
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

/*
 default values for comedi_cmd structure
 rate is samples per second for each channel.

 returns the result of a comedi_command_test.
*/

int default_comedi_cmd(comedi_t *dev, comedi_cmd *cmd, float *rate)
{
	int ret;
	int *channels=0;
	int n_channels;
	int aref= AREF_GROUND;
	int range=3;  // which voltage range to use

	if(dev == NULL) /* don't do anything if there is no device */
	{
		fprintf(stderr,__FILE__":  dev is null, can't initialize\n");
		return -1;
	}

	/* free any previous arrays */ 
	if(cmd->chanlist_len) free(cmd->chanlist);
	if(cmd->data_len) free(cmd->data);


	cmd->subdev = 0;  // which subdevice

	/* set default channels, but don't put into comedi_cmd */
	channels=malloc(N_CHANS*sizeof(*cmd->chanlist));
	n_channels=0;
	channels[n_channels++]=CR_PACK(8,range,aref);
#if 0
    	channels[n_channels++]=CR_PACK(9,range,aref);
#endif

	/* comedi rate is samples per nanosecond for 
	   all channels being read.  
	   Assume rate in samples per second per channel. */
	*rate=ESD_DEFAULT_RATE;
	ret = comedi_get_cmd_generic_timed(dev,cmd->subdev,cmd,
					   1e9/(*rate*n_channels));
	if(ret<0){
		comedi_perror("comedi_get_cmd_generic_timed\n");
		return ret;
	} 

	/* For some reason, comedi_get_cmd_generic_timed stomps
           on any allocation of cmd->chanlist */
	cmd->chanlist_len=n_channels;
	cmd->chanlist=channels;
	
	cmd->scan_end_arg = cmd->chanlist_len;

	cmd->stop_src = TRIG_NONE;
	cmd->stop_arg = 0;
	
	return comedi_command_test(dev,cmd);
}

/*
  routines to save and load a comedi_cmd structure and
  the rate to and from a config file.

  note that this reallocs two arrays in cmd.
*/

#define COMEDI_CONFIG_FILE "/.eXtace/comedi"

int read_comedi_cmd(comedi_cmd *cmd, float *rate)
{
	int i;
	int temp;
        ConfigFile *cfgfile;
        gchar *filename;
	char str[32];

	filename = g_strconcat(g_get_home_dir(), COMEDI_CONFIG_FILE, NULL);
	cfgfile = cfg_open_file(filename);
	if (cfgfile)
        {		
		cfg_read_float(cfgfile, "other", "rate", rate);
		cfg_read_int(cfgfile, "cmd", "subdev", &cmd->subdev);
		cfg_read_int(cfgfile, "cmd", "flags", &cmd->flags);

		cfg_read_int(cfgfile, "cmd", "start_src", &cmd->start_src);
		cfg_read_int(cfgfile, "cmd", "start_arg", &cmd->start_arg);

		cfg_read_int(cfgfile, "cmd", "begin_src", &cmd->scan_begin_src);
		cfg_read_int(cfgfile, "cmd", "begin_arg", &cmd->scan_begin_arg);

		cfg_read_int(cfgfile, "cmd", "convert_src", &cmd->convert_src);
		cfg_read_int(cfgfile, "cmd", "convert_arg", &cmd->convert_arg);

		cfg_read_int(cfgfile, "cmd", "scan_end_src", &cmd->scan_end_src);
		cfg_read_int(cfgfile, "cmd", "scan_end_arg", &cmd->scan_end_arg);

		cfg_read_int(cfgfile, "cmd", "stop_src", &cmd->stop_src);
		cfg_read_int(cfgfile, "cmd", "stop_arg", &cmd->stop_arg);

		cfg_read_int(cfgfile, "cmd", "chanlist_len", &cmd->chanlist_len);
		cmd->chanlist=realloc(cmd->chanlist,
				     cmd->chanlist_len*sizeof(*cmd->chanlist));
		for(i=0; i<cmd->chanlist_len; i++)
		{
			sprintf(str,"chanlist_%i",i);
			cfg_read_int(cfgfile, "cmd", str, cmd->chanlist+i);
#if 0
			printf("reading channel[%i]=%x\n",i,cmd->chanlist[i]);
#endif
		}

		cfg_read_int(cfgfile, "cmd", "data_len", &cmd->data_len);
		cmd->data=realloc(cmd->data,
				     cmd->data_len*sizeof(*cmd->data));
		for(i=0; i<cmd->data_len; i++)
		{
			sprintf(str,"data_%i",i);
			cfg_read_int(cfgfile, "cmd", str, &temp);
			cmd->data[i]=temp;
		}

                cfg_free(cfgfile);
	}
        else
                printf("Config file not found, using defaults\n");
        g_free(filename);

	return cfgfile?0:-1; /* if successful, return 0 */
}

/*  write the comedi_cmd structure and the rate to the config file */

int write_comedi_cmd(comedi_cmd *cmd, float rate)
{
	int i;
	char str[32];
        ConfigFile *cfgfile;
        gchar *filename;

	filename = g_strconcat(g_get_home_dir(), COMEDI_CONFIG_FILE, NULL);
	cfgfile = cfg_open_file(filename);
	if (!cfgfile)
                cfgfile = cfg_new();

	cfg_write_float(cfgfile, "other", "rate", rate);
	cfg_write_int(cfgfile, "cmd", "subdev", cmd->subdev);
	cfg_write_int(cfgfile, "cmd", "flags", cmd->flags);

	cfg_write_int(cfgfile, "cmd", "start_src", cmd->start_src);
	cfg_write_int(cfgfile, "cmd", "start_arg", cmd->start_arg);
	
	cfg_write_int(cfgfile, "cmd", "begin_src", cmd->scan_begin_src);
	cfg_write_int(cfgfile, "cmd", "begin_arg", cmd->scan_begin_arg);
	
	cfg_write_int(cfgfile, "cmd", "convert_src", cmd->convert_src);
	cfg_write_int(cfgfile, "cmd", "convert_arg", cmd->convert_arg);
	
	cfg_write_int(cfgfile, "cmd", "scan_end_src", cmd->scan_end_src);
	cfg_write_int(cfgfile, "cmd", "scan_end_arg", cmd->scan_end_arg);
	
	cfg_write_int(cfgfile, "cmd", "stop_src", cmd->stop_src);
	cfg_write_int(cfgfile, "cmd", "stop_arg", cmd->stop_arg);
	
	cfg_write_int(cfgfile, "cmd", "chanlist_len", cmd->chanlist_len);
	for(i=0; i<cmd->chanlist_len; i++)
	{
		sprintf(str,"chanlist_%i",i);
		cfg_write_int(cfgfile, "cmd", str, cmd->chanlist[i]);
#if 0
		printf("saving channel[%i]=%x\n",i,cmd->chanlist[i]);
#endif
	}
	
	cfg_write_int(cfgfile, "cmd", "data_len", cmd->data_len);
	for(i=0; i<cmd->data_len; i++)
	{
		sprintf(str,"data_%i",i);
		cfg_write_int(cfgfile, "cmd", str, cmd->data[i]);
	}
	
        cfg_write_file(cfgfile, filename);
	cfg_free(cfgfile);
        g_free(filename);

	return 0;  /* if successful, return 0 */
}

/* free memory used by comedi_cmd structure */

int free_comedi_cmd(comedi_cmd *cmd)
{
	cmd->data_len=0;
	cmd->chanlist_len=0;
	cmd->data=realloc(cmd->data,0);
	cmd->chanlist=realloc(cmd->chanlist,0);

	return 0;
}

#endif




