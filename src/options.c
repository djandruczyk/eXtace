/*
 * options.c source file for extace
 * 
 * /GDK/GNOME sound (esd) system output display program
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
#include <defaults.h>
#include <globals.h>
#include <protos.h>
#include <gtk/gtk.h>

/* See globals.h for variable declarations and DEFINES */

int setup_options()
{

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
			GTK_SIGNAL_FUNC(close_options),optionsbut);
	gtk_signal_connect(GTK_OBJECT(options),"delete_event",
			GTK_SIGNAL_FUNC(close_options),optionsbut);
	gtk_container_set_border_width(GTK_CONTAINER(options),2);

	notebook = gtk_notebook_new ();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
	gtk_container_add(GTK_CONTAINER(options), notebook);
	gtk_widget_show(notebook);


	frame = gtk_frame_new("General Controls");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_widget_show(frame);

	label = gtk_label_new("General Options");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), frame, label);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("Main window refresh rate in Frames/Sec. (approx)");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	adj = gtk_adjustment_new((float)refresh_rate,1.0,refresh_max,1.0,1.0,1.0);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),0);
	gtk_box_pack_start(GTK_BOX(vbox),scale,TRUE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), (gpointer)REFRESH_RATE);
	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),sep,TRUE, TRUE, 0);

	label = gtk_label_new("Scope/FFT Decimation Factors");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);


	hbox = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);

	button = gtk_radio_button_new_with_label(NULL, "1 (None)");
        gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
        gtk_signal_connect(GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (set_decimation_factor), \
			(gpointer)NO_DECIMATION);
        if (decimation_factor == NO_DECIMATION)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

        group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
        button = gtk_radio_button_new_with_label(group, "2");
        gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
        gtk_signal_connect(GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (set_decimation_factor), \
			(gpointer)DECIMATE_BY_2);
        if (decimation_factor == DECIMATE_BY_2)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

        group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
        button = gtk_radio_button_new_with_label(group, "3");
        gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
        gtk_signal_connect(GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (set_decimation_factor), \
			(gpointer)DECIMATE_BY_3);
        if (decimation_factor == DECIMATE_BY_3)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

        group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
        button = gtk_radio_button_new_with_label(group, "4");
        gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
        gtk_signal_connect(GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (set_decimation_factor), \
			(gpointer)DECIMATE_BY_4);
        if (decimation_factor == DECIMATE_BY_4)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

        group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
        button = gtk_radio_button_new_with_label(group, "5");
        gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
        gtk_signal_connect(GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (set_decimation_factor), \
			(gpointer)DECIMATE_BY_5);
        if (decimation_factor == DECIMATE_BY_5)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

        group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
        button = gtk_radio_button_new_with_label(group, "6");
        gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
        gtk_signal_connect(GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (set_decimation_factor), \
			(gpointer)DECIMATE_BY_6);
        if (decimation_factor == DECIMATE_BY_6)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

        group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
        button = gtk_radio_button_new_with_label(group, "7");
        gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
        gtk_signal_connect(GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (set_decimation_factor), \
			(gpointer)DECIMATE_BY_7);
        if (decimation_factor == DECIMATE_BY_7)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

        group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
        button = gtk_radio_button_new_with_label(group, "8");
        gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
        gtk_signal_connect(GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (set_decimation_factor), \
			(gpointer)DECIMATE_BY_8);
        if (decimation_factor == DECIMATE_BY_8)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);


	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),sep,TRUE, TRUE, 0);

	button = gtk_toggle_button_new_with_label("Pause Display");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)PAUSE_DISP);

	button = gtk_toggle_button_new_with_label("Backing Pixmap Disabled");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)BACK_PIXMAP);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), use_back_pixmap);


	frame = gtk_frame_new("Sound Source");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(vbox), frame,TRUE,TRUE,0);

	sub_vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame), sub_vbox);

	label = gtk_label_new("Choose Sound Source ");
	gtk_box_pack_start(GTK_BOX(sub_vbox),label,TRUE,TRUE,0);

	button = gtk_radio_button_new_with_label(NULL, "Use Esound");
	gtk_box_pack_start(GTK_BOX(sub_vbox),button,TRUE,TRUE,0);
	if (sound_source == ESD)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
	gtk_signal_connect(GTK_OBJECT(button),"toggled",
			GTK_SIGNAL_FUNC(button_handle),(gpointer)ESD);

	gtk_widget_show_all(vbox);
	
	/*  END of General Options Tab (Options Panel) */

	/*  BEGINNING of Low Res. FFT Options Tab (Options Panel) */
	box = gtk_hbox_new(FALSE,0);

	label = gtk_label_new("Low Res. FFT's");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), box, label);

	frame = gtk_frame_new("Low Resolution FFT Display Options");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(box),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
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
			GTK_SIGNAL_FUNC (slider_changed), (gpointer)BANDS);

	label = gtk_label_new("Bar Decay Speed");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	adj = gtk_adjustment_new((float)bar_decay_speed,1.0,bar_decay_max,1.0,1.0,1.0);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),0);
	gtk_box_pack_start(GTK_BOX(vbox),scale,FALSE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), (gpointer)BAR_DECAY);

	label = gtk_label_new("Peak Hold Time");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	adj = gtk_adjustment_new((float)peak_hold_time,1.0,peak_hold_max,1.0,1.0,1.0);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),0);
	gtk_box_pack_start(GTK_BOX(vbox),scale,FALSE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), (gpointer)PEAK_HOLD);


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
			GTK_SIGNAL_FUNC (slider_changed), (gpointer)PEAK_DECAY);

	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),sep,TRUE, TRUE, 0);

	hbox1 = gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,TRUE,0);

	button = gtk_toggle_button_new_with_label("Leading Edge Shown");
	gtk_box_pack_start(GTK_BOX(hbox1),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)LEADING_EDGE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), !show_leader);

	button = gtk_toggle_button_new_with_label("Bar Decay Disabled");
	gtk_box_pack_start(GTK_BOX(hbox1),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)BAR_DECAY);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), bar_decay);

	button = gtk_toggle_button_new_with_label("Peak Decay Disabled");
	gtk_box_pack_start(GTK_BOX(hbox1),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)PEAK_DECAY);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), peak_decay);

	hbox1 = gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,TRUE,0);

	button = gtk_toggle_button_new_with_label("Invert Landform Y-axis");
	gtk_box_pack_start(GTK_BOX(hbox1),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)LANDFLIP);
	if (landflip == -1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),1);

	button = gtk_toggle_button_new_with_label("Landform Perspective Tilt Disabled");
	gtk_box_pack_start(GTK_BOX(hbox1),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)LAND_PERS_TILT);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), landtilt);


	hbox = gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,FALSE,0);

	button = gtk_radio_button_new_with_label(NULL, "Linear Frequency Axis");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)LINEAR);
	if (axis_type == LINEAR)
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);
	}

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group, "Logarithmic Frequency Axis");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)LOG);
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

	hbox = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,TRUE,TRUE,0);

	frame = gtk_frame_new("Scope Mode");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(hbox),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("Scope Mode");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	button = gtk_radio_button_new_with_label(NULL, "Gradient Scope");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (scope_mode), (gpointer)GRAD_SCOPE);
	if (scope_sub_mode == GRAD_SCOPE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group, "Dot Scope");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (scope_mode), (gpointer)DOT_SCOPE);
	if (scope_sub_mode == DOT_SCOPE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group, "Line Scope");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (scope_mode), (gpointer)LINE_SCOPE);
	if (scope_sub_mode == LINE_SCOPE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

	frame = gtk_frame_new("Scope Controls");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(hbox),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	button = gtk_toggle_button_new_with_label("Trace Stabilizer Disabled");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)STABLE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), stabilized);

	button = gtk_toggle_button_new_with_label("Scope Graticule Disabled");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)GRATICULE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), show_graticule);

	frame = gtk_frame_new("Sync/Trigger Controls");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(main_scope_vbox),frame,TRUE,TRUE,0);

	hbox = gtk_hbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame), hbox);

	button = gtk_radio_button_new_with_label(NULL, "Sync to Left Channel");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)SYNC_LEFT);
	if (sync_to_left)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group, "Sync to Right Channel");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)SYNC_RIGHT);
	if (sync_to_right)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group, "Sync Independently");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)SYNC_INDEP);
	if (sync_independant)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), TRUE);

	gtk_widget_show_all(main_scope_vbox);

	/*  END of Scope Options Tab (Options Panel) */
	/*  BEGINNING of High Res, FFT Options Tab (Options Panel) */

	box = gtk_hbox_new(FALSE,0);

	label = gtk_label_new("High Res. FFT's");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), box, label);

	frame = gtk_frame_new("High Resolution Display Bandwidth");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(box),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(TRUE,0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	button = gtk_radio_button_new_with_label(NULL,"5512.5 Hz");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(bandwidth == 5512.5)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)5512);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"11025 Hz");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(bandwidth == 11025)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)11025);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"22050 Hz");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(bandwidth == 22050)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)22050);

	frame = gtk_frame_new("High Resolution FFT Display Options");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(box),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(TRUE,0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	button = gtk_toggle_button_new_with_label("Invert Spike Y-axis");
	gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)SPIKEFLIP);
	if (spikeflip == -1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),1);

	button = gtk_toggle_button_new_with_label("Spikes Perspective Tilt Disabled");
	gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)SPIKE_PERS_TILT);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), spiketilt);


	gtk_widget_show_all(box);

	/*  END of High Res, FFT Options Tab (Options Panel) */
	/*  BEGINNING of FFT Options Tab (Options Panel) */

	hbox = gtk_hbox_new(FALSE,0);

	label = gtk_label_new("FFT Options");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), hbox, label);

	frame = gtk_frame_new("Window Functions");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(hbox),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);
	button = gtk_radio_button_new_with_label(NULL,"Hamming");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == HAMMING)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), (gpointer)HAMMING);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Hanning");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == HANNING)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), (gpointer)HANNING);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Blackman");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == BLACKMAN)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), (gpointer)BLACKMAN);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Blackman-Harris");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == BLACKMAN_HARRIS)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), (gpointer)BLACKMAN_HARRIS);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Gaussian");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == GAUSSIAN)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), (gpointer)GAUSSIAN);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Welch");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == WELCH)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), (gpointer)WELCH);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Parzen");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == PARZEN)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), (gpointer)PARZEN);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Rectangular");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(window_func == RECTANGULAR)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "pressed",
			GTK_SIGNAL_FUNC (setup_datawindow), (gpointer)RECTANGULAR);

	vbox1 = gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbox),vbox1,TRUE,TRUE,0);

	frame = gtk_frame_new("Window Function Options");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(vbox1),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("Window Function Active Width");

	button = gtk_radio_button_new_with_label(NULL,"Full Width");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(winstyle == FULL)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)FULL);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Half Width");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(winstyle == HALF)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)HALF);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Quarter Width");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(winstyle == QUARTER)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)QUARTER);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Eighth Width");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(winstyle == EIGHTH)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)EIGHTH);




	frame = gtk_frame_new("FFT DataSource");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(vbox1),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("FFT Audio Signal Source");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	button = gtk_radio_button_new_with_label(NULL,"Left Channel");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(fft_signal_source == LEFT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)LEFT);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Right Channel");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(fft_signal_source == RIGHT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)RIGHT);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Left+Right (Composite)");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(fft_signal_source == COMPOSITE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)COMPOSITE);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"Left-Right (Difference)");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(fft_signal_source == DIFFERENCE)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)DIFFERENCE);

	frame = gtk_frame_new("FFT Width");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(hbox),frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	label = gtk_label_new("FFT Width in samples");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	button = gtk_radio_button_new_with_label(NULL,"1024");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(nsamp == 1024)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)1024);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"2048");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(nsamp == 2048)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)2048);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"4096");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(nsamp == 4096)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)4096);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"8192");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(nsamp == 8192)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)8192);

	group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	button = gtk_radio_button_new_with_label(group,"16384");
	gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	if(nsamp == 16384)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	gtk_signal_connect (GTK_OBJECT (button), "toggled",
			GTK_SIGNAL_FUNC (button_handle), (gpointer)16384);

	/*    group = gtk_radio_button_group (GTK_RADIO_BUTTON (button));
	 *    button = gtk_radio_button_new_with_label(group,"32768");
	 *    gtk_box_pack_start(GTK_BOX(vbox),button,TRUE,TRUE,0);
	 *    if(nsamp == 32768)
	 *	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (button), TRUE);
	 *    gtk_signal_connect (GTK_OBJECT (button), "toggled",
	 *	    GTK_SIGNAL_FUNC (button_handle), (gpointer)32768);
	 */

	gtk_widget_show_all(hbox);

	/*  END of FFT Options Tab (Options Panel) */
	/*  BEGINNING of Misc Options Tab (Options Panel) */

	box = gtk_vbox_new(FALSE,0);

	label = gtk_label_new("Misc Options");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), box, label);

	frame = gtk_frame_new("Spectrogram Controls");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(box), frame,TRUE,TRUE,0);

	vbox = gtk_vbox_new(FALSE,0);
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
			GTK_SIGNAL_FUNC (slider_changed), (gpointer)TAPE_SCROLL);

	label = gtk_label_new("Lag in Milliseconds");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	lag_adj = gtk_adjustment_new(lag,lag_min,(int)(1000*((float)(BUFFER/2)/(float)RATE)),1,1,1);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(lag_adj));
	gtk_scale_set_digits(GTK_SCALE(scale),0);
	gtk_box_pack_start(GTK_BOX(vbox),scale,TRUE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (lag_adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), (gpointer)LAG);

	label = gtk_label_new("Noise Floor");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);

	adj = gtk_adjustment_new(noise_floor,noise_floor_min,noise_floor_max,0.001,0.1,0.01);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),3);
	gtk_box_pack_start(GTK_BOX(vbox),scale,TRUE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), (gpointer)NOISE_FLOOR);

	label = gtk_label_new("Vertical Multiplier");
	gtk_box_pack_start(GTK_BOX(vbox),label,TRUE,TRUE,0);
	adj = gtk_adjustment_new(multiplier,multiplier_min,multiplier_max,0.01,1.0,0.1);
	scale = gtk_hscale_new(GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(scale),2);
	gtk_box_pack_start(GTK_BOX(vbox),scale,TRUE,TRUE,0);
	gtk_range_set_update_policy(GTK_RANGE (scale),
			GTK_UPDATE_CONTINUOUS);
	gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
			GTK_SIGNAL_FUNC (slider_changed), (gpointer)SENSITIVITY);

	frame = gtk_frame_new("Buffer Latency Monitor");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
	gtk_box_pack_start(GTK_BOX(box), frame,FALSE,FALSE,0);

	eventbox = gtk_event_box_new();
	gtk_container_add(GTK_CONTAINER(frame), eventbox);
	gtk_widget_set_usize(eventbox,options->allocation.width,100);   
	gtk_widget_realize(eventbox); 

	buffer_area = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(eventbox),buffer_area);

	buffer_pixmap = gdk_pixmap_new(buffer_area->window,400,100,
			gtk_widget_get_visual(buffer_area)->depth);
	gtk_signal_connect( GTK_OBJECT(buffer_area),"configure_event",
			(GtkSignalFunc)configure_event, (gpointer)BUFFER_AREA);
	gtk_signal_connect( GTK_OBJECT(buffer_area),"expose_event",
			(GtkSignalFunc)expose_event,(gpointer)BUFFER_AREA);
	gdk_window_set_back_pixmap(buffer_area->window,buffer_pixmap,0);


	gtk_window_set_policy(GTK_WINDOW(options), FALSE,	/* allow shrink */
			TRUE, 	/* allow grow */
			FALSE);	/* auto shrink */

	gtk_widget_show_all(box);

	/*  END of Misc Options Tab (Options Panel) */

	return 0;
}
