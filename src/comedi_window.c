#include <init.h>
#include <comedi_window.h>
#include <input.h>
#include <comedi_input.h>
#include <draw.h>
#include <stdlib.h>
#include <stdio.h>

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


/* some local variables */
#define N_NUMBERS 64
GtkWidget *channel_button[N_NUMBERS];
#ifdef HAVE_COMEDI
static comedi_cmd *cmd;
static comedi_t *dev;
static int my_handle;
GtkObject *spinner_adj;

int channel_button_update(comedi_cmd *c)
{
	int k;
	int j;
	int n_chans;

	n_chans = comedi_get_n_channels(dev,c->subdev);
	if(n_chans > N_NUMBERS)n_chans=N_NUMBERS;
	for(j=0; j<n_chans; j++)
	{
		for(k=0; k<c->chanlist_len; k++)
			if(CR_CHAN(c->chanlist[k])==j) break;
		gtk_toggle_button_set_active(
			GTK_TOGGLE_BUTTON(channel_button[j]),
			k < c->chanlist_len);
	}

	return 0;
}

/*
   change comedi subdevice is always called in 
   conjunction with another action (channel or range.) 

   This probably doesn't work, but I have
   no way to test it.
*/

int change_comedi_subdevice(GtkWidget *widget, gpointer data)
{
	int subdevice=GPOINTER_TO_INT(data);
	/* its pressed and new subdevice */
	if (GTK_TOGGLE_BUTTON(widget)->active && 
	    subdevice != cmd->subdev){
		draw_stop();
		input_thread_stopper(my_handle);
		cmd->subdev=subdevice;  /* don't actually do anything */
		input_thread_starter(my_handle);  /* start and apply new cmd */
		draw_start();
	}
	return TRUE;
}

int change_comedi_range(GtkWidget *widget, gpointer data)
{
	int range=GPOINTER_TO_INT(data);
	int i;

        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
        {
		draw_stop();
		input_thread_stopper(my_handle);
		for(i=0; i<cmd->chanlist_len; i++)
			cmd->chanlist[i]=CR_PACK(
				CR_CHAN(cmd->chanlist[i]),
				range,
				CR_AREF(cmd->chanlist[i]));
		input_thread_starter(my_handle);  /* start and apply new cmd */
		draw_start();
	}
	return TRUE;
}

/*
  Change channel.
  Some cards require the channels to be consecutive.
  If an error occurs when adding or subtracting a channel, 
  close all other channels.
*/

int change_comedi_channel(GtkWidget *widget, gpointer data)
{
	int i;
	int slot;
	int ret;
	unsigned int save;
	float save_len=cmd->chanlist_len;
	int chan=GPOINTER_TO_INT(data);
#if 0 /* debug print */
	printf("****in change_comedi_channel, chan=%i\n",chan);
#endif

	if (GTK_TOGGLE_BUTTON(widget)->active) /* its pressed */
	{
		for(slot=0; slot<cmd->chanlist_len; slot++)
		{
			/* channel already there */
			if(CR_CHAN(cmd->chanlist[slot])==chan) return TRUE;
			if(CR_CHAN(cmd->chanlist[slot])>chan) break;
		}

		draw_stop();
		input_thread_stopper(my_handle);
		cmd->chanlist_len++;
		cmd->chanlist=realloc(cmd->chanlist,cmd->chanlist_len*
				      sizeof(*cmd->chanlist));
		for(i=cmd->chanlist_len-1; i>slot; i--)
			cmd->chanlist[i] = cmd->chanlist[i-1];
		cmd->chanlist[slot]=save=CR_PACK(
			chan,
			CR_RANGE(cmd->chanlist[slot?slot-1:0]),
			CR_AREF(cmd->chanlist[slot?slot-1:0])
			);
	}
	else
	{
		for(slot=0; slot<cmd->chanlist_len; slot++)
			if(CR_CHAN(cmd->chanlist[slot])==chan)break;
		/* channel already removed */
		if(slot==cmd->chanlist_len) return TRUE;

		if(cmd->chanlist_len ==1)
		{  
			/* make button active again. */
			channel_button_update(cmd);
			return TRUE;
		}

		draw_stop();
		input_thread_stopper(my_handle);
		cmd->chanlist_len--;
		save=cmd->chanlist[slot];
                for(i=slot; i<cmd->chanlist_len; i++)
                        cmd->chanlist[i]=cmd->chanlist[i+1];
	}

	/* This needs to be run twice.  Why??? */
	comedi_command_test(dev,cmd);
	ret=comedi_command_test(dev,cmd);
	if(ret != 0){
#ifdef DEBUG
		fprintf(stderr,__FILE__ 
			",%i:  channel %i on returned %i,"
			" now %i channels.\n            ",
			__LINE__,chan,ret,cmd->chanlist_len);
		comedi_perror("comedi_command_test result");
#endif
		/* go to just one channel */
		cmd->chanlist[0]=save;
		cmd->chanlist_len=1;
	}

	channel_button_update(cmd);
	ring_rate *= ((float) save_len)/((float) cmd->chanlist_len);
	ring_rate_changed(); /* Fix all gui controls that depend on
			      * ring_rate (adjustments and such) */
	gtk_adjustment_set_value(GTK_ADJUSTMENT(spinner_adj),ring_rate);
	input_thread_starter(my_handle);		
	draw_start();
	
	return TRUE;
}

#if GTK_MAJOR_VERSION >= 2 
int change_comedi_rate(GtkSpinButton *adj, gpointer *data)
#else
int change_comedi_rate(GtkAdjustment *adj, gpointer *spinner)
#endif
{
	static int already = FALSE;  /* this routine is already running */
	float arate;
	unsigned int irate,orate;
	unsigned int new_rate;

	/* don't do anything if already started */
	if(already) return TRUE;

	/* find old and new rate in samples per nanosecond */
	if(cmd->convert_src&TRIG_TIMER)
		orate=cmd->convert_arg;
	else 
		orate=cmd->scan_begin_arg;
#if GTK_MAJOR_VERSION >= 2 
	arate = gtk_spin_button_get_value(adj);
#else
	arate = adj->value;
#endif
	irate=1e9/(cmd->chanlist_len*arate)+0.5;
	/* if change is not significant, don't do anything */
	if(irate == orate) return TRUE;

	already = TRUE;
	draw_stop();
	input_thread_stopper(my_handle);
	new_rate = comedi_command_timed(dev, cmd, irate);
	if(new_rate > 0)
		ring_rate = 1e9/(cmd->chanlist_len*(float) new_rate);
	else 
	{
		fprintf(stderr,__FILE__":  new rate %i not valid,"
			" using old rate %i\n",irate,orate);
		new_rate = comedi_command_timed(dev, cmd, orate);
		if(new_rate < 0)
		{
			fprintf(stderr,__FILE__":  can't do old rate %i\n",orate);
			return TRUE;
		}
	}

	ring_rate_changed(); /* Fix all gui controls that depend on
			      * ring_rate (adjustments and such) */
	gtk_adjustment_set_value(GTK_ADJUSTMENT(adj),ring_rate);
	input_thread_starter(my_handle); /* start and apply new cmd */
	draw_start();
	already = FALSE; /* allow further adjusts */

	return TRUE;
}

#endif


GtkWidget *comedi_device_control_open(int handle_number)
{
	int k;
        GtkWidget *vbox;
        GtkWidget *label;
	/* string containing integers */
 	static gchar channel_numbers[N_NUMBERS][3];
#ifdef HAVE_COMEDI
	gint j;
	gint subdev;
	int use_sub;
	int n_chans;
	int n_subdevices;
	comedi_range *rng;
	gint chan;
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

#endif

	for(k=0; k<N_NUMBERS; k++)
		sprintf(channel_numbers[k],"%i",k);

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

			if(n_chans > N_NUMBERS)
			{
				fprintf(stderr,__FILE__":  %i channels, too many\n",n_chans);
				n_chans = N_NUMBERS;
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
					channel_button[j] = button;
					gtk_box_pack_start(GTK_BOX(hbox),button,TRUE, TRUE, 0);
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
				channel_button_update(cmd);
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
				GTK_ADJUSTMENT(spinner_adj), 0.0, 0);
			gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spinner),
						     TRUE);
#if GTK_MAJOR_VERSION >= 2
			gtk_signal_connect(
				GTK_OBJECT(spinner),"value_changed",
				GTK_CALLBACK(change_comedi_rate),
				NULL
				);
#else
			gtk_signal_connect(
				GTK_OBJECT(spinner_adj),"value_changed",
				GTK_SIGNAL_FUNC(change_comedi_rate),
				(gpointer) spinner
				);
#endif
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
