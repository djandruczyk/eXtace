/*
 * main.c source file for extace
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
#include <globals.h>
#include <protos.h>
#include <math.h>
#include <esd.h>
#include <gtk/gtk.h>

/* See globals.h for variable declarations and DEFINES */

int main(int argc, char **argv)
{

	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *main_window;
	GtkWidget *button;
	GtkTooltips *tip;

	g_thread_init(NULL);
	gtk_init(&argc, &argv);
	gdk_imlib_init();

	init();		/* initialize all global variables */
	make_extace_dirs();	/* make conf dir if it doesn't exist */
	read_config();	/* read config file */
	mem_alloc();	/* Alloate memory for the buffers */
	create_initial_colormaps();	/* Create colormaps if you don't have em */
	read_colormap(Color_map.filename);

	gtk_widget_push_visual(gdk_imlib_get_visual());
	gtk_widget_push_colormap(gdk_imlib_get_colormap());

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	main_win_ptr = main_window;
	gtk_widget_set_uposition(main_window, main_x_origin, main_y_origin);
	gtk_window_set_title(GTK_WINDOW(main_window),"eXtace " VERSION);
	gtk_container_set_border_width(GTK_CONTAINER(main_window),0);
	gtk_signal_connect(GTK_OBJECT(main_window),"destroy_event",
			GTK_SIGNAL_FUNC(leave),NULL);
	gtk_signal_connect(GTK_OBJECT(main_window),"delete_event",
			GTK_SIGNAL_FUNC(leave),NULL);
	gtk_signal_connect (GTK_OBJECT (main_window), "button_press_event",
			(GtkSignalFunc) button_notify_event, NULL);
	gtk_signal_connect (GTK_OBJECT (main_window), "button_release_event",
			(GtkSignalFunc) button_notify_event, NULL);
	gtk_signal_connect (GTK_OBJECT (main_window), "motion_notify_event",
			(GtkSignalFunc) motion_notify_event, NULL);

	gtk_widget_set_events (main_window, GDK_EXPOSURE_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_POINTER_MOTION_MASK);
	gtk_window_set_policy(GTK_WINDOW(main_window), TRUE,	/* allow shrink */
			TRUE, 	/* allow grow */
			FALSE);	/* auto shrink */

	tip = gtk_tooltips_new();


	gtk_widget_realize(main_window);

	vbox=gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(main_window), vbox);
	hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

	button=gtk_button_new_with_label("Wire 3D");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_tooltips_set_tip(tip, button, "3D Wireframe FFT", NULL);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",
			GTK_SIGNAL_FUNC(button_3d_fft),(gpointer)WIRE_3D);

	button=gtk_button_new_with_label("Landform 3D");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_tooltips_set_tip(tip, button, "3D Landform FFT", NULL);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",
			GTK_SIGNAL_FUNC(button_3d_fft),(gpointer)FILL_3D);

	button=gtk_button_new_with_label("Graphic EQ");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_tooltips_set_tip(tip, button, "Graphic Equalizer", NULL);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",
			GTK_SIGNAL_FUNC(button_2d_fft),NULL);

	button=gtk_button_new_with_label("Scope");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_tooltips_set_tip(tip, button, "Oscilliscope", NULL);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",
			GTK_SIGNAL_FUNC(button_oscilloscope),NULL);

	button=gtk_button_new_with_label("3D Spikes");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_tooltips_set_tip(tip, button, "3D Hi-resolution FFT", NULL);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",
			GTK_SIGNAL_FUNC(button_3d_detailed),NULL);

	button=gtk_button_new_with_label("Horiz Spectrogram");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_tooltips_set_tip(tip, button, "Horizontal Spectrogram", NULL);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",
			GTK_SIGNAL_FUNC(button_horiz_specgram),NULL);

	button=gtk_button_new_with_label("Vert Spectrogram");
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_tooltips_set_tip(tip, button, "Vertical Spectrogram", NULL);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",
			GTK_SIGNAL_FUNC(button_vert_specgram),NULL);

	button=gtk_button_new_with_label("About");
	about_but_ptr = button;
	gtk_tooltips_set_tip(tip, button, "About eXtace", NULL);
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",
			GTK_SIGNAL_FUNC(button_about),NULL);

	button=gtk_toggle_button_new_with_label("Options");
	optionsbut=button;
	gtk_tooltips_set_tip(tip, optionsbut, "eXtace Advanced Options", NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(optionsbut), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox),optionsbut,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT(optionsbut),"toggled",
			GTK_SIGNAL_FUNC(button_options),NULL);

	button=gtk_button_new_with_label("Close");
	gtk_tooltips_set_tip(tip, button, "Close eXtace!!", NULL);
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
	gtk_signal_connect(GTK_OBJECT(button),"clicked",
			GTK_SIGNAL_FUNC(leave),NULL);


	main_display=gtk_drawing_area_new();
	gtk_box_pack_start(GTK_BOX(vbox),main_display,TRUE,TRUE,0);
	gtk_widget_realize(main_display);
	g_print("Width being set to %i\n",width);
	gtk_widget_set_usize(main_display,width,height);
	//    win=main_display->window;
	gc=gdk_gc_new(main_display->window);
	gdk_gc_copy(gc,main_display->style->white_gc);
	main_pixmap=gdk_pixmap_new(main_display->window,width,height,
			gtk_widget_get_visual(main_display)->depth);
	gdk_draw_rectangle(main_pixmap,
			main_display->style->black_gc,
			TRUE, 0,0,
			width,height);
	gtk_signal_connect( GTK_OBJECT(main_display),"configure_event",
			(GtkSignalFunc)configure_event, (gpointer)MAIN_DISPLAY);
	gtk_signal_connect( GTK_OBJECT(main_display),"expose_event",
			(GtkSignalFunc)expose_event, (gpointer)MAIN_DISPLAY);
	gdk_window_set_back_pixmap(main_display->window,main_pixmap,0);
	gtk_tooltips_set_tip(tip,main_display, "The middle button will get you the color picker", NULL);

	dir_win = gtk_window_new(GTK_WINDOW_DIALOG);
	dir_win_ptr = dir_win;
	gtk_window_set_title(GTK_WINDOW(dir_win),"Direction");
	gtk_widget_set_usize(dir_win,dir_width,dir_height);
	gtk_widget_set_uposition(dir_win, dir_x_origin, dir_y_origin);
	gtk_container_set_border_width(GTK_CONTAINER(dir_win),2);

	gtk_widget_set_events (dir_win, GDK_EXPOSURE_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_FOCUS_CHANGE_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_POINTER_MOTION_MASK);

	gtk_signal_connect(GTK_OBJECT(dir_win),"delete_event",
			GTK_SIGNAL_FUNC(close_dir_win),NULL);
	gtk_signal_connect (GTK_OBJECT (dir_win), "motion_notify_event",
			(GtkSignalFunc) dir_motion, NULL);
	gtk_signal_connect (GTK_OBJECT (dir_win), "focus_out_event",
			(GtkSignalFunc) dir_save_state, NULL);

	gtk_widget_realize(dir_win);

	dir_area = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(dir_win), dir_area);
	gtk_widget_realize(dir_area);
	gtk_drawing_area_size (GTK_DRAWING_AREA (dir_area), 
			dir_win->allocation.width, 
			dir_win->allocation.height);
	dir_pixmap=gdk_pixmap_new(dir_area->window,dir_width,dir_height,
			gtk_widget_get_visual(dir_area)->depth);
	gdk_draw_rectangle( dir_pixmap,
			dir_area->style->black_gc,
			TRUE, 0,0,
			dir_width,dir_height);
	gtk_signal_connect( GTK_OBJECT(dir_area),"configure_event",
			(GtkSignalFunc)configure_event, (gpointer)DIR_AREA);
	gtk_signal_connect( GTK_OBJECT(dir_area),"expose_event",
			(GtkSignalFunc)expose_event, (gpointer)DIR_AREA);
	gdk_window_set_back_pixmap(dir_area->window,dir_pixmap,0);

	gtk_tooltips_set_tip(tip, dir_area, "Drag the pointer ", NULL);

	gtk_widget_show(dir_area);

	gtk_window_set_policy(GTK_WINDOW(dir_win), FALSE,	/* allow shrink */
			TRUE, 	/* allow grow */
			FALSE);	/* auto shrink */


	setup_datawindow(NULL,(WindowFunction)window_func);

	setup_options();
	init_colortab();
	init_gc(main_window);
	gtk_widget_show_all(main_window);
	gtk_tooltips_enable(tip);

	if (dir_win_present)
	{
		gtk_widget_show(dir_win);
		update_pointer();
	}


	if (open_sound() >= 0)
	{
		audio_thread_starter();
		draw_start();
	}
	if (mode == STARS)/* gotta emit it by hand due to config file */
		gtk_signal_emit_by_name(GTK_OBJECT(about_but_ptr),"clicked");
	ready = 1;		/* All set */
	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();
	return 0;
}
