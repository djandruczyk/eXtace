/*
 /GDK/GNOME sound (esd) system output Audio Visualiztion program
 * 
 * Copyright (C) 1999 by Dave J. Andruczyk 
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

#ifndef _PROTOS_H_
#define _PROTOS_H_ 1

#include <config.h>
#include <enums.h>
#include <gtk/gtk.h>


/* Function Prototypes for all objects/source files */


/* buttons.c */
void leave(GtkWidget *, gpointer *);
gint close_dir_win(GtkWidget *, gpointer *);
gint close_grad_win(GtkWidget *, gpointer *);
gint close_options(GtkWidget *, gpointer *);
gint slider_changed(GtkWidget *, gpointer *);
gint button_handle(GtkWidget *, gpointer *);
gint change_display(GtkWidget *, gpointer *);
gint set_decimation_factor(GtkWidget *widget, gpointer *data);
gint scope_mode(GtkWidget *, gpointer *);
/* buttons.c */

/* init.c */
void init(void);
void read_config(void);
void save_config(GtkWidget *);
void make_extace_dirs(void);
void mem_alloc(void);
void mem_dealloc(void);
void reinit_extace(int );
/* init.c */

/* markers.c */
void update_time_markers();
void buffer_area_update(void);
void update_freq_markers();
/* markers.c */

/* datawindow.h */
void setup_datawindow(GtkWidget *,WindowFunction );
/* datawindow.h */

/* draw.c */
void draw_start(void);
void draw_stop(void);
int draw(void);
/* draw.c */


/* events.c */
gint configure_event(GtkWidget *, GdkEventConfigure *, gpointer );
gint expose_event(GtkWidget *, GdkEventExpose *, gpointer );
gint button_notify_event (GtkWidget *, GdkEventButton *, gpointer );
gint motion_notify_event (GtkWidget *, GdkEventMotion *, gpointer );
gint test_on_line(int, int);
gint test_if_close(int, int);
void change_spec_start(gint);
void change_x_start(gint,gint);
void change_x_end(gint,gint);
/* events.c */
	
/* dir.c */
gint dir_save_state(GtkWidget *, GdkEventFocus *);
gint update_dircontrol(GtkWidget *);
gint setup_dircontrol(GtkWidget *);
gint dir_motion (GtkWidget *, GdkEventMotion *, gpointer);
gint feed_pointer(gint, gint);
void dir_axis_update(void);
gint update_pointer(void);
/* dir.c */

/* gc_maker.c */
void init_gc(GtkWidget *);
/* gc_maker.c */

/* color_win.c */
gint color_event (GtkWidget *, GdkEventButton *, gpointer);
gint color_button(GtkWidget *, gpointer );
void create_initial_colormaps(void);
void save_colormap(GtkWidget *, GtkFileSelection *);
void load_colormap(GtkWidget *, GtkFileSelection *);
void read_colormap(char *);
void update_gradient(GtkWidget *, int );
void init_colortab();
void grad_win_create(void);
gint grad_win_save_state(GtkWidget *, GdkEventFocus *);
void gradient_update();
gint close_fileselection(GtkWidget*, gpointer *);
/* color_win.c */

void kt_stars_update_func(GtkWidget *);
void kt_stars_stop(GtkWidget *);
void kt_stars_start(GtkWidget *, gint , gint );
GtkWidget * kt_stars_new(GtkWidget *, GdkPixmap *);
void kt_stars_set_logo_pixmp(GtkWidget *, GdkPixmap *, GdkPixmap *);

/* reducer.c */
void reducer(int, int, int);
/* reducer.c */

/* land_3d.c */
void draw_land3d_fft(void);
void draw_land3d_fft(void);
void draw_land3d_forward(void);
void draw_land3d_reverse(void);
/* land_3d.c */

/* 2d_eq.c */
void draw_2d_eq(void);
/* 2d_eq.c */

/* scope.c */
void draw_scope(void);
/* scope.c */

/* spike_3d.c */
void draw_spike_3d(void);
/* spike_3d.c */

/* vert_specgram.c */
void draw_vert_specgram(void);
/* vert_specgram.c */

/* horiz_specgram.c */
void draw_horiz_specgram(void);
/* horiz_specgram.c */

/* sound.c */
int audio_thread_starter(void);
int audio_thread_stopper(void);
void *esd_starter_thread(void * );
void esd_reader_thread(gpointer , gint , GdkInputCondition );
int open_sound(void);
void close_sound(void);
void error_close_cb(GtkWidget *, gpointer * );
/* sound.c */

/* options.c */
gint setup_options(void);
/* options.c */

/* audio_processing.c */
void run_fft(void);
int audio_chewer(void);
void split_and_decimate(void);
/* audio_processing.c */

#endif
