/*
 * options.c source file for extace
 * 
 * Audio visualization
 * 
 * Copyright (C) 1999-2017 by Dave J. Andruczyk 
 * 
 * Based on the original extace written by The Rasterman and Michael Fulbright
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#include <buttons.h>
#include <config.h>
#include <datawindow.h>
#include <defaults.h>
#include <enums.h>
#include <input.h>
#include <events.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <options.h>
#include <pa_utils.h>

/* See globals.h for variable declarations and DEFINES */

GtkObject *lf_adj;
GtkObject *hf_adj;
GtkObject *lag_adj;
extern gint scope_sync_source;

int setup_options()
{

	gint i;
	gchar *str;
	GtkWidget *options;
	GtkWidget *vbox;
	GtkWidget *eventbox;
	GtkWidget *sub_vbox;
	GtkWidget *vbox1;
	GtkWidget *main_scope_vbox;
	GtkWidget *hbox;
	GtkWidget *hbox1;
	GtkWidget *button;
	GtkWidget *notebook;
	GtkWidget *frame;
	GtkWidget *sep;
	GtkWidget *label;
	GtkWidget *scale;
	GtkWidget *box;
	GtkObject *adj;
	GSList *group;

	/* options window */
	options = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	options_win_ptr = options;
	gtk_window_set_title(GTK_WINDOW(options),"eXtace Options");
	gtk_signal_connect(GTK_OBJECT(options),"destroy_event",
			GTK_SIGNAL_FUNC(close_options),NULL);
	gtk_signal_connect(GTK_OBJECT(options),"delete_event",
			GTK_SIGNAL_FUNC(close_options),NULL);
	gtk_container_set_border_width(GTK_CONTAINER(options),2);

	notebook = gtk_notebook_new ();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
	gtk_container_add(GTK_CONTAINER(options), notebook);
	gtk_widget_show(notebook);

	frame = gtk_frame_new("Global Settings");
	gtk_widget_show(frame);

	label = gtk_label_new("General Options");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), frame, label);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	vbox1 = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox1),5);
	gtk_box_pack_start(GTK_BOX(vbox),vbox1,TRUE,TRUE,0);

	label = gtk_label_new("Refresh Rate in Frames/Sec. (approx)");
	gtk_box_pack_start(GTK_BOX(vbox1),label,TRUE,TRUE,0);

	adj = gtk_adjustment_new((float)refresh_rate,1.0,refresh_max,1.0,1.0,1.0);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),0);
	gtk_box_pack_start(GTK_BOX(vbox1),scale,TRUE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), 
			    GINT_TO_POINTER(REFRESH_RATE));
	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox1),sep,TRUE, TRUE, 0);

	label = gtk_label_new("Scope/FFT Decimation Factors");
	gtk_box_pack_start(GTK_BOX(vbox1),label,TRUE,TRUE,0);

	hbox = gtk_hbox_new(TRUE,0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox),5);
	gtk_box_pack_start(GTK_BOX(vbox1),hbox,TRUE,TRUE,0);
	group=NULL;

	for(i=1; i<=8; i++){
		if(i==1)
			str= g_strdup("1 (None)");
		else
			str = g_strdup_printf("%i",i);
		button = gtk_radio_button_new_with_label(group,str);
		g_free(str);
		gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
		gtk_signal_connect(GTK_OBJECT (button), "clicked",
				   GTK_SIGNAL_FUNC (set_decimation_factor), \
				   GINT_TO_POINTER(i));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), decimation_factor == i);
		group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	}

	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox1),sep,TRUE, TRUE, 0);

	button = gtk_toggle_button_new_with_label("Pause Display");
	gtk_box_pack_start(GTK_BOX(vbox1),button,TRUE,TRUE,5);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), 
			    GINT_TO_POINTER(PAUSE_DISP));

	frame = gtk_frame_new("Sound Source");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(vbox), frame,TRUE,TRUE,0);

	sub_vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width (GTK_CONTAINER (sub_vbox), 5);
	gtk_container_add(GTK_CONTAINER(frame), sub_vbox);

	/* 
	   Do gtk_toggle_button_set_active() before gtk_signal_connect()
	   so set_data_source is not run on initialization.
	*/

#ifdef HAVE_PULSEAUDIO
	button = gtk_combo_box_new_text();
	populate_inputs_from_pulseaudio(button);
	gtk_box_pack_start(GTK_BOX(sub_vbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT(button),"changed",
			   GTK_SIGNAL_FUNC(update_data_source_name),
			   NULL);

	button = gtk_button_new_with_label("Open pavucontrol to link eXtace to your sound source");
	gtk_box_pack_start(GTK_BOX(sub_vbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT(button),"pressed",
			   GTK_SIGNAL_FUNC(open_pavucontrol),
			   NULL);
#endif

	gtk_widget_show_all(vbox);
	
	/*  END of General Options Tab (Options Panel) */

	/*  BEGINNING of Low Res. FFT Options Tab (Options Panel) */
	box = gtk_hbox_new(FALSE,0);

	label = gtk_label_new("Low Res. FFT's");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), box, label);

	frame = gtk_frame_new("Low Resolution FFT Display Options");
	gtk_box_pack_start(GTK_BOX(box),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("Number of bands (low res displays only)");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	adj = gtk_adjustment_new((float)bands,16.0,(float)MAXBANDS+1,1.0,1.0,1.0);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),0);
	gtk_box_pack_start(GTK_BOX(vbox),scale,FALSE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			    GTK_SIGNAL_FUNC (slider_changed), 
			    GINT_TO_POINTER(BANDS));

	label = gtk_label_new("Bar Decay Speed");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	adj = gtk_adjustment_new((float)bar_decay_speed,1.0,bar_decay_max,1.0,1.0,1.0);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),0);
	gtk_box_pack_start(GTK_BOX(vbox),scale,FALSE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed),
			    GINT_TO_POINTER(BAR_DECAY));

	label = gtk_label_new("Peak Hold Time");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	adj = gtk_adjustment_new((float)peak_hold_time,1.0,peak_hold_max,1.0,1.0,1.0);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),0);
	gtk_box_pack_start(GTK_BOX(vbox),scale,FALSE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed),
			    GINT_TO_POINTER(PEAK_HOLD));


	label = gtk_label_new("Peak Decay Speed");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	adj = gtk_adjustment_new((float)peak_decay_speed,1.0,11.0,1.0,1.0,1.0);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),0);
	//    GTK_ADJUSTMENT (adj)->value = peak_decay_speed;
	gtk_box_pack_start(GTK_BOX(vbox),scale,FALSE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), 
			    GINT_TO_POINTER(PEAK_DECAY));

	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),sep,TRUE, TRUE, 0);

	hbox1 = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,TRUE,0);

	button = gtk_toggle_button_new_with_label("Leading Edge Shown");
	gtk_box_pack_start(GTK_BOX(hbox1),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), 
			    GINT_TO_POINTER(LEADING_EDGE));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), show_leader);

	button = gtk_toggle_button_new_with_label("Bar Decay Disabled");
	gtk_box_pack_start(GTK_BOX(hbox1),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), 
			    GINT_TO_POINTER(USE_BAR_DECAY));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), bar_decay);

	button = gtk_toggle_button_new_with_label("Peak Decay Disabled");
	gtk_box_pack_start(GTK_BOX(hbox1),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), 
			    GINT_TO_POINTER(USE_PEAK_DECAY));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), peak_decay);

	hbox1 = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,TRUE,0);

	button = gtk_toggle_button_new_with_label("Invert Landform Y-axis");
	gtk_box_pack_start(GTK_BOX(hbox1),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), 
			    GINT_TO_POINTER(LANDFLIP));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),landflip);

	button = gtk_toggle_button_new_with_label("Smooth 3D Landform");
	gtk_box_pack_start(GTK_BOX(hbox1),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), 
			    GINT_TO_POINTER(OUTLINED));
	if (outlined == TRUE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),TRUE);

	button = gtk_toggle_button_new_with_label("Landform Perspective Tilt Disabled");
	gtk_box_pack_start(GTK_BOX(hbox1),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), 
			    GINT_TO_POINTER(LAND_PERS_TILT));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), landtilt);


	hbox = gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,FALSE,0);

	button = gtk_radio_button_new_with_label(NULL, "Linear Frequency Axis");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (fft_set_axis_type), 
			   GINT_TO_POINTER(LINEAR));
	if (axis_type == LINEAR)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
	}

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group, "Logarithmic Frequency Axis");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (fft_set_axis_type), 
			   GINT_TO_POINTER(LOG));
	if (axis_type == LOG)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
	}

	gtk_widget_show_all(box);


	/*  END of Low Res. FFT Options Tab (Options Panel) */
	/*  BEGINNING of Scope Options Tab (Options Panel) */

	vbox = gtk_vbox_new(FALSE,0);
	main_scope_vbox = vbox;

	label = gtk_label_new("Scope Options");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), vbox, label);

	hbox = gtk_hbox_new(FALSE,5);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);

	frame = gtk_frame_new("Scope Mode");
	gtk_box_pack_start(GTK_BOX(hbox),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("Scope Mode");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	button = gtk_radio_button_new_with_label(NULL, "Gradient Scope");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (scope_mode),
			   GINT_TO_POINTER(GRAD_SCOPE));
	//if (scope_sub_mode == GRAD_SCOPE)
//		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(button),FALSE);


	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group, "Dot Scope");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (scope_mode), 
			   GINT_TO_POINTER(DOT_SCOPE));
	if (scope_sub_mode == DOT_SCOPE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group, "Line Scope");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (scope_mode), 
			   GINT_TO_POINTER(LINE_SCOPE));
	if (scope_sub_mode == LINE_SCOPE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

	frame = gtk_frame_new("Scope Controls");
	gtk_box_pack_start(GTK_BOX(hbox),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,5);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	button = gtk_toggle_button_new_with_label("Trace Stabilizer Disabled");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), 
			    GINT_TO_POINTER(STABILIZED));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), stabilized);

	button = gtk_toggle_button_new_with_label("Scope Graticule Disabled");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), 
			    GINT_TO_POINTER(GRATICULE));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), show_graticule);

	frame = gtk_frame_new("Sync/Trigger Controls");
	gtk_box_pack_start(GTK_BOX(main_scope_vbox),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("Scope Zoom Factor");
	gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,TRUE,0);

	adj = gtk_adjustment_new((float)scope_zoom,0.25,5,0.01,0.01,0);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),2);
	gtk_box_pack_start(GTK_BOX(vbox),scale,TRUE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), 
			    GINT_TO_POINTER(SCOPE_ZOOM));
	sep = gtk_hseparator_new();

	hbox = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);

	button = gtk_radio_button_new_with_label(NULL, "Sync to Left Channel");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (scope_sync_source_set), 
			GINT_TO_POINTER(SYNC_LEFT));
	if (scope_sync_source == SYNC_LEFT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group, "Sync to Right Channel");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (scope_sync_source_set), 
			GINT_TO_POINTER(SYNC_RIGHT));
	if (scope_sync_source == SYNC_RIGHT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group, "Sync Independently");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			   GTK_SIGNAL_FUNC (scope_sync_source_set), 
			   GINT_TO_POINTER(SYNC_INDEP));
	if (scope_sync_source == SYNC_INDEP)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

	gtk_widget_show_all(main_scope_vbox);

	/*  END of Scope Options Tab (Options Panel) */
	/*  BEGINNING of High Res, FFT Options Tab (Options Panel) */

	box = gtk_vbox_new(FALSE,0);

	label = gtk_label_new("High Res. FFT's");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), box, label);

	frame = gtk_frame_new("High Resolution Display Bandwidth");
	gtk_box_pack_start(GTK_BOX(box),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	hbox = gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);
	
	label = gtk_label_new("Low Frequency Limit");
	gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,0);

	sep = gtk_vseparator_new();
	gtk_box_pack_start(GTK_BOX(hbox),sep,FALSE,TRUE,0);

	label = gtk_label_new("High Frequency Limit");
	gtk_box_pack_start(GTK_BOX(hbox),label,TRUE,TRUE,0);

	hbox = gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

	/* Low Frequency Limit slider */
	lf_adj = gtk_adjustment_new(
		low_freq,ring_rate/nsamp,
		high_freq-64.0*((float)ring_rate/(float)decimation_factor/(float)nsamp),
		ring_rate/nsamp,
		ring_rate/nsamp,
		10.0
		);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(lf_adj));
	gtk_scale_set_digits(GTK_SCALE(scale),2);
	gtk_box_pack_start(GTK_BOX(hbox),scale,FALSE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (lf_adj), "value_changed",
			    GTK_SIGNAL_FUNC (slider_changed), 
			    GINT_TO_POINTER(LOW_LIMIT));


	/* High Frequency Limit slider */
	hf_adj = gtk_adjustment_new(
		high_freq,low_freq+64.0*((float)ring_rate/(float)decimation_factor/(float)nsamp),
		(float)ring_rate/(float)(2.0*decimation_factor)+(float)ring_rate/(float)nsamp,ring_rate/nsamp,ring_rate/nsamp,10.0
		);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(hf_adj));
	gtk_scale_set_digits(GTK_SCALE(scale),2);
	gtk_box_pack_start(GTK_BOX(hbox),scale,FALSE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (hf_adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), 
			    GINT_TO_POINTER(HIGH_LIMIT));

	frame = gtk_frame_new("High Resolution FFT Display Options");
	gtk_box_pack_start(GTK_BOX(box),frame,TRUE,TRUE,0);

	hbox = gtk_hbox_new(TRUE,5);
	gtk_container_set_border_width(GTK_CONTAINER(hbox),5);
	gtk_container_add(GTK_CONTAINER(frame), hbox);

	button = gtk_toggle_button_new_with_label("Invert Spike Y-axis");
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), 
			    GINT_TO_POINTER(SPIKEFLIP));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),spikeflip);

	button = gtk_toggle_button_new_with_label("Spikes Perspective Tilt Disabled");
	gtk_box_pack_start(GTK_BOX(hbox),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), 
			    GINT_TO_POINTER(SPIKE_PERS_TILT));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), spiketilt);


	gtk_widget_show_all(box);

	/*  END of High Res, FFT Options Tab (Options Panel) */
	/*  BEGINNING of FFT Options Tab (Options Panel) */

	hbox = gtk_hbox_new(FALSE,5);

	label = gtk_label_new("FFT Options");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), hbox, label);

	frame = gtk_frame_new("Window Functions");
	gtk_box_pack_start(GTK_BOX(hbox),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	button = gtk_radio_button_new_with_label(NULL,"Hamming");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == HAMMING)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			    GTK_SIGNAL_FUNC (setup_datawindow), 
			    GINT_TO_POINTER(HAMMING));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Hanning");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == HANNING)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), 
			    GINT_TO_POINTER(HANNING));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Blackman");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == BLACKMAN)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			    GTK_SIGNAL_FUNC (setup_datawindow), 
			    GINT_TO_POINTER(BLACKMAN));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Blackman-Harris");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == BLACKMAN_HARRIS)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow),
			    GINT_TO_POINTER(BLACKMAN_HARRIS));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Gaussian");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == GAUSSIAN)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), 
			    GINT_TO_POINTER(GAUSSIAN));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Welch");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == WELCH)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), 
			    GINT_TO_POINTER(WELCH));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Parzen");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == PARZEN)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), 
			    GINT_TO_POINTER(PARZEN));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Rectangular");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == RECTANGULAR)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), 
			    GINT_TO_POINTER(RECTANGULAR));

	vbox1 = gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbox),vbox1,TRUE,TRUE,0);

	frame = gtk_frame_new("Window Function Options");
	gtk_box_pack_start(GTK_BOX(vbox1),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("Window Function Active Width");

	button = gtk_radio_button_new_with_label(NULL,"Full Width");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(win_width == FULL)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_window_width), 
			    GINT_TO_POINTER(FULL));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Half Width");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(win_width == HALF)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_window_width), 
			    GINT_TO_POINTER(HALF));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Quarter Width");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(win_width == QUARTER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_window_width), 
			    GINT_TO_POINTER(QUARTER));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Eighth Width");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(win_width == EIGHTH)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_window_width), 
			    GINT_TO_POINTER(EIGHTH));


	frame = gtk_frame_new("FFT DataSource");
	gtk_box_pack_start(GTK_BOX(vbox1),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("FFT Audio Signal Source");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	button = gtk_radio_button_new_with_label(NULL,"Left Channel");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(fft_signal_source == LEFT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_fft_data_to_display), 
			    GINT_TO_POINTER(LEFT));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Right Channel");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(fft_signal_source == RIGHT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_fft_data_to_display), 
			    GINT_TO_POINTER(RIGHT));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Left+Right (L+R)");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(fft_signal_source == LEFT_PLUS_RIGHT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_fft_data_to_display), 
			    GINT_TO_POINTER(LEFT_PLUS_RIGHT));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Left-Right (L-R)");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(fft_signal_source == LEFT_MINUS_RIGHT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_fft_data_to_display), 
			    GINT_TO_POINTER(LEFT_MINUS_RIGHT));

	frame = gtk_frame_new("FFT Width");
	gtk_box_pack_start(GTK_BOX(hbox),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("FFT Size in samples");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	button = gtk_radio_button_new_with_label(NULL,"1024");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(nsamp == 1024)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_fft_size), 
			    GINT_TO_POINTER(S_1024));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"2048");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(nsamp == 2048)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_fft_size), 
			    GINT_TO_POINTER(S_2048));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"4096");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(nsamp == 4096)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_fft_size), 
			    GINT_TO_POINTER(S_4096));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"8192");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(nsamp == 8192)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_fft_size), 
			    GINT_TO_POINTER(S_8192));

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"16384");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(nsamp == 16384)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (set_fft_size), 
			    GINT_TO_POINTER(S_16384));

	/*    group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	 *    button = gtk_radio_button_new_with_label(group,"32768");
	 *    gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	 *    if(nsamp == 32768)
	 *	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	 *    gtk_signal_connect (GTK_OBJECT (button), "toggled",
	 *	    GTK_SIGNAL_FUNC (set_fft_size), GINT_TO_POINTER(S_32768);
	 */

	gtk_widget_show_all(hbox);

	/*  END of FFT Options Tab (Options Panel) */
	/*  BEGINNING of Misc Options Tab (Options Panel) */

	box = gtk_vbox_new(FALSE,0);

	label = gtk_label_new("Misc Options");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), box, label);

	frame = gtk_frame_new("Spectrogram Controls");
	gtk_box_pack_start(GTK_BOX(box), frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("Spectrogram Scroll Speed");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	adj = gtk_adjustment_new((float)tape_scroll,1.0,tape_scroll_max,1.0,1.0,1.0);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),0);
	gtk_box_pack_start(GTK_BOX(vbox),scale,TRUE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), 
			    GINT_TO_POINTER(TAPE_SCROLL));

	label = gtk_label_new("Lag in Milliseconds");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	lag_adj = gtk_adjustment_new(lag,lag_min,(int)(1000*ring_end/ring_rate),
				 1,1,1);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(lag_adj));
	gtk_scale_set_digits(GTK_SCALE(scale),0);
	gtk_box_pack_start(GTK_BOX(vbox),scale,TRUE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (lag_adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), 
			    GINT_TO_POINTER(LAG));

	label = gtk_label_new("Noise Floor");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	adj = gtk_adjustment_new(noise_floor,noise_floor_min,noise_floor_max,0.001,0.1,0.01);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),3);
	gtk_box_pack_start(GTK_BOX(vbox),scale,TRUE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), 
			    GINT_TO_POINTER(NOISE_FLOOR));

	label = gtk_label_new("Vertical Multiplier");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);
	adj = gtk_adjustment_new(multiplier,multiplier_min,multiplier_max,0.01,1.0,0.1);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),2);
	gtk_box_pack_start(GTK_BOX(vbox),scale,TRUE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), 
			    GINT_TO_POINTER(SENSITIVITY));

	frame = gtk_frame_new("Buffer Latency Monitor");
	gtk_box_pack_start(GTK_BOX(box), frame,FALSE,FALSE,0);

	eventbox = gtk_event_box_new();
	gtk_container_set_border_width (GTK_CONTAINER (eventbox), 5);
	gtk_container_add(GTK_CONTAINER(frame), eventbox);
	gtk_widget_set_usize(eventbox,options->allocation.width,100);   
	gtk_widget_realize(eventbox); 

	buffer_area = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(eventbox),buffer_area);

	buffer_pixmap = gdk_pixmap_new(buffer_area->window,400,100,
			gtk_widget_get_visual(buffer_area)->depth);
	gtk_signal_connect( GTK_OBJECT(buffer_area),"configure_event",
			(GtkSignalFunc)configure_event, 
			    GINT_TO_POINTER(BUFFER_AREA));
	gtk_signal_connect( GTK_OBJECT(buffer_area),"expose_event",
			(GtkSignalFunc)expose_event,
			    GINT_TO_POINTER(BUFFER_AREA));
	gdk_window_set_back_pixmap(buffer_area->window,buffer_pixmap,0);


	gtk_window_set_policy(GTK_WINDOW(options), FALSE,	/* allow shrink */
			TRUE, 	/* allow grow */
			FALSE);	/* auto shrink */

	gtk_widget_show_all(box);

	/*  END of Misc Options Tab (Options Panel) */

	return 0;
}
