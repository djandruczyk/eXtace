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

#include <config.h>
#include <enums.h>
#include <gtk/gtk.h>
#ifdef HAVE_ALSA
#include <sys/asoundlib.h>
#endif


/* Function Prototypes for all objects/source files */


void leave(GtkWidget *, gpointer *);
gint button_options(GtkWidget *, gpointer *);
gint close_options(GtkWidget *, gpointer *);
gint slider_changed(GtkWidget *, gpointer *);
gint button_handle(GtkWidget *, gpointer *);
gint change_fftlen(GtkWidget *, gpointer *);
gint button_3d_fft(GtkWidget *, gpointer *);
gint button_2d_fft(GtkWidget *, gpointer *);
gint button_vert_specgram(GtkWidget *, gpointer *);
gint button_horiz_specgram(GtkWidget *, gpointer *);
gint button_oscilloscope(GtkWidget *, gpointer *);
gint button_3d_detailed(GtkWidget *, gpointer *);
gint button_about(GtkWidget *, gpointer *);
gint scope_mode(GtkWidget *, gpointer *);
void init(void);
void update_freq_markers();
void update_time_markers();
int open_sound(void);
void close_sound(void);
void setup_datawindow(GtkWidget *,WindowFunction );
gint setup_dircontrol(GtkWidget *);
gint feed_pointer(gint, gint);
int draw(void);
void draw_stop(void);
void rtc_open(void);
void draw_start(void);
void change_spec_start(gint);
void change_x_start(gint,gint);
void change_x_end(gint,gint);
int GetFFT(void);

gint configure_event(GtkWidget *, GdkEventConfigure *, gpointer );
gint expose_event(GtkWidget *, GdkEventExpose *, gpointer );

gint time_disp_configure(GtkWidget *, GdkEventConfigure *);

void init_colortab();
void handle_read(gpointer , gint , GdkInputCondition );

gint dir_motion (GtkWidget *, GdkEventMotion *, gpointer);
gint update_dircontrol(GtkWidget *);
void buffer_area_update(void);
gint motion_notify_event (GtkWidget *, GdkEventMotion *, gpointer );
gint button_notify_event (GtkWidget *, GdkEventButton *, gpointer );

gint dir_save_state(GtkWidget *, GdkEventFocus *);
gint grad_win_save_state(GtkWidget *, GdkEventFocus *);

void init_gc(GtkWidget *);
gint color_event (GtkWidget *, GdkEventButton *, gpointer);
gint close_dir_win(GtkWidget *, gpointer *);
gint close_grad_win(GtkWidget *, gpointer *);
gint color_button(GtkWidget *, gpointer );
gint update_pointer(void);
void make_extace_dirs(void);
void read_config(void);
void mem_alloc(void);
void mem_dealloc(void);
void save_config(void);
void error_close_cb(GtkWidget *, gpointer * );
void grad_win_create(void);
void update_gradient(GtkWidget *, int );
void gradient_update();
void kt_stars_update_func(GtkWidget *);
void kt_stars_stop(GtkWidget *);
void kt_stars_start(GtkWidget *, gint , gint );
GtkWidget * kt_stars_new(GtkWidget *, GdkPixmap *);
void kt_stars_set_logo_pixmp(GtkWidget *, GdkPixmap *, GdkPixmap *);
gint close_winfun(GtkWidget *, gpointer );
gint test_if_close(int, int);
gint test_on_line(int, int);
void reducer(int, int, int);
void file_ok_save(GtkWidget *, GtkFileSelection *);
void file_ok_load(GtkWidget *, GtkFileSelection *);
void read_colormap(char *);
void draw_land3d_forward(void);
void draw_land3d_reverse(void);
void dir_axis_update(void);
#ifdef HAVE_ALSA
void loopback_data_arrived(void *, char *, size_t );
void loopback_position_change(void *, unsigned int );
void loopback_format_change(void *, snd_pcm_format_t *);
void loopback_silence(void *, size_t );
#endif
void *alsa_jumpstart(void* );
void *rtc_poller(void* );
void alsa_adjust(GtkWidget *, gpointer *);
void draw_land3d_fft(void);
void draw_2d_eq(void);
void draw_scope(void);
void draw_spike_3d(void);
void draw_vert_specgram(void);
void draw_horiz_specgram(void);
void create_initial_colormaps(void);
int audio_thread_starter(void);
int audio_thread_stopper(void);
int alsa_read_bytes(long int);
void *alsa_starter_thread(void * );
void *esd_starter_thread(void * );
void alsa_reader_thread(void *, char *, size_t);
void esd_reader_thread(gpointer , gint , GdkInputCondition );
void reinit_extace(int );
gint setup_options(void);
