/*
 * color_win.c source file for extace (color picker)
 * 
 * /GDK/GNOME sound (esd) system output display program
 *
 * Based on the original extace written by The Rasterman and Michael Fulbright
 * 
 * Hacked by Dave J. Andruczyk <djandruczyk@yahoo.com>
 * to be fully scalable with lots of new options, for tilting the axis's
 * and various other cool stuff.
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#include <buttons.h>
#include <color_win.h>
#include <config.h>
#include <configfile.h>
#include <enums.h>
#include <events.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <string.h>


/* See globals.h for variable declarations and DEFINES */
GtkWidget *grad_disp;
GtkWidget *colorseldlg;
GdkImlibImage *im;
gint colortab_ready = 0;
gint grad_x_origin;
gint grad_y_origin;
gint selection_open = 0;

gint color_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{

	gdouble colsel_colors[3];
	gint handled = FALSE;


	/* Check if we've received a button pressed event */

	if (event->type == GDK_BUTTON_PRESS && colorseldlg != NULL)
	{
		/* where did the user press the button */
		color_loc = widget->allocation.height - event->y;
		colsel_colors[0] = cr[color_loc]/256.0;
		colsel_colors[1] = cg[color_loc]/256.0;
		colsel_colors[2] = cb[color_loc]/256.0;


		/* Set the color in the selction dialog to wherever we clicked */

		gtk_color_selection_set_color (GTK_COLOR_SELECTION(colorseldlg),colsel_colors);


	}

	return handled;

}
int color_button(GtkWidget *widget, gpointer data)
{
	GtkWidget * filew;
	switch ((ColorOperation)data)
	{
		case SET_COLOR:
			update_gradient(NULL, color_loc);
			init_colortab();
			gradient_update();
			break;

		case CLOSE:
			gtk_widget_hide(grad_win_ptr);
			grad_win_present = 0;
			break;

		case SAVE:
			if (selection_open == 0)
			{
				selection_open = 1;
				filew = gtk_file_selection_new("Save Colormap");
				gtk_signal_connect (GTK_OBJECT(filew), 
						"destroy",
						(GtkSignalFunc) 
							close_fileselection, 
						GTK_OBJECT (filew));
				gtk_signal_connect(GTK_OBJECT 
						(GTK_FILE_SELECTION 
							(filew)->ok_button),
						"clicked", 
						(GtkSignalFunc) 
							save_colormap, 
						filew);
				gtk_signal_connect_object(GTK_OBJECT 
						(GTK_FILE_SELECTION
							(filew)->cancel_button),
						"clicked", 
						(GtkSignalFunc) 
							close_fileselection,
						GTK_OBJECT (filew));
				gtk_file_selection_set_filename(
						GTK_FILE_SELECTION(filew),
						g_strconcat(g_get_home_dir(),
							"/.eXtace/ColorMaps/", 
							NULL));
				gtk_widget_show(filew);
			}
			break;
		case LOAD:
			if (selection_open == 0)
			{
				selection_open = 1;
				filew = gtk_file_selection_new("Load Colormap");
				gtk_signal_connect (GTK_OBJECT(filew), 
						"destroy",
						(GtkSignalFunc) 
							close_fileselection, 
						GTK_OBJECT (filew));
				gtk_signal_connect(GTK_OBJECT 
						(GTK_FILE_SELECTION 
							(filew)->ok_button),
						"clicked", 
						(GtkSignalFunc) load_colormap, 
						filew);
				gtk_signal_connect_object(GTK_OBJECT 
						(GTK_FILE_SELECTION
							(filew)->cancel_button),
						"clicked", 
						(GtkSignalFunc) 
							close_fileselection,
						GTK_OBJECT (filew));
				gtk_file_selection_set_filename(
						GTK_FILE_SELECTION(filew),
						g_strconcat(g_get_home_dir(), 
							"/.eXtace/ColorMaps/", 
							NULL));
				gtk_widget_show(filew);
			}
			break;
		default:
			break;
	}
	return TRUE;
}
gint close_fileselection(GtkWidget *widget, gpointer *data)
{
	selection_open = 0;
	gtk_widget_destroy(GTK_WIDGET(widget));
	return TRUE;
}
	
void create_initial_colormaps(void)
{

	gchar val[20];
	gchar * filename;
	ConfigFile *cfgfile;
	gint i = 0;
	gint x = 0;
	gint j = 0;
	gfloat tmp=0;
	gint temp_array[] = { \
		/* 5 RGB triplets deterimine the color gradient */
		/* more points could be used, but functions below assume only 5
		 * so be carefull
		 *  steps R1  G1  B1  R2  G2  B2  R3  G3  B3  R4  G4  B4  R5  G5  B5  */
		/*AuthFav*/  5,   19, 17, 18,160, 40,140,210,130, 20,240,200, 20,255,240, 80, \
			/*Autumn*/   5,  255,  0,  0,255, 64,  0,255,128,  0,255,191,  0,255,255,  0, \
			/*B&W*/	     5,    1,  1,  1, 65, 63, 65,118,113,116,182,182,188,240,241,246, \
			/*Bone*/     5,    0,  0, 32, 56, 88, 88,122,143,143,189,199,199,255,255,255, \
			/*Cool*/     5,    0,255,255, 64,191,255,128,128,255,191, 64,255,255,  0,255, \
			/*Copper*/   5,    0,  0,  0, 80, 50, 32,159,100, 63,239,149, 95,255,199,127, \
			/*Default*/  5,   30,  0,160,160, 40,140,210,130, 20,240,200, 20,255,240, 80, \
			/*FireEngRd*/5,   16,  0,  0, 67,  2,  5,139,  1,  7,136,  0,  0,247,  3, 10, \
			/*Flag*/     5,  255,  0,  0,255,255,255,  0,  0,255,  0,  0,  0,255,  0,  0, \
			/*GreenScrn*/5,    0,  2,  0,  4, 45,  1,  9,104,  5, 21,184,  7, 16,245, 18, \
			/*Hot*/      5,  255,  0,  0,255,255,  0,255,255, 85,255,255,170,255,255,255, \
			/*HSV*/      5,  255,  0,  0,204,255,  0,  0,255,102,  0,102,255,204,  0,255, \
			/*Inv B&W*/  5,  245,245,254,191,185,193,118,113,116, 78, 76, 79,  1,  1,  1, \
			/*Jet*/	     5,    0,  0,128,  0,255,255,255,255,255,255,255,  0,128,  0,  0, \
			/*Pink*/     5,  147,  0,  0,180,180,104,208,208,170,233,233,217,255,255,255, \
			/*Spring*/   5,  255,  0,255,255, 64,191,255,128,128,255,191, 64,255,255,  0, \
			/*Summer*/   5,    0,128,102, 64,159,102,128,191,102,191,223,102,255,255,102, \
			/*Blues*/    5,    0,  1, 18,  3,  6, 83,  6,  4,147, 11, 10,192,  6,  9,251, \
			/*Winter*/   5,    0,  0,255,  0, 64,223,  0,128,191,  0,191,159,  0,255,128 };

	gchar * names[] = {"Authors_Favorite","Autumn","Black_n_White","Bone","Cool","Copper", "Default","Fire_Engine_Red","Flag","Green_Screen","Hot","HSV","Inverse_BW","Jet","Pink","Spring","Summer","The_Blues","Winter","NULL"};

	/* Setup at least one default one */
	start->red=30;start->green=0;start->blue=160;
	pt2->red=160;pt2->green=40;pt2->blue=140;
	pt3->red=210;pt3->green=130;pt3->blue=20;
	pt4->red=240;pt4->green=200;pt4->blue=20;
	end->red=255;end->green=240;end->blue=80;

	while (strcmp("NULL",names[i]))
	{
		tmp = 0.0;
		filename = g_strconcat(g_get_home_dir(), "/.eXtace/ColorMaps/", names[i],NULL);
		cfgfile = cfg_open_file(filename);
		if (!cfgfile)
			cfgfile = cfg_new();
		else
		{
			x+=16; /* Move up to next set of vars for colormap */
			i++;
			cfg_free(cfgfile);
			g_free(filename);
			continue;
		}
		cfg_write_int(cfgfile, "General", "steps", temp_array[x++]);

		/* This assumes only 5 points in above table.  be carefull */
		for (j=1;j<=5;j++)
		{
			sprintf(val,"step_%i",j);
			cfg_write_float(cfgfile, "General", val, tmp);
			tmp+=0.25;
			sprintf(val,"red_%i",j);
			cfg_write_int(cfgfile, "Colormap", val, temp_array[x++]);
			sprintf(val,"green_%i",j);
			cfg_write_int(cfgfile, "Colormap", val, temp_array[x++]);
			sprintf(val,"blue_%i",j);
			cfg_write_int(cfgfile, "Colormap", val, temp_array[x++]);
		}
		cfg_write_file(cfgfile, filename);
		cfg_free(cfgfile);

		g_free(filename);
		i++;
	}
}
void save_colormap(GtkWidget * widget, GtkFileSelection *filesel)
{
	/* This routine needs to be rewritten for the new colortable file format
	 * The new format has a variable number of steps (up to 50 or so).
	 * Thus the file has to store hte number of steps, and their positions 
	 * in the map. (percentage of height sound simple enuf to me..)
	 */

	int j=0;
	gchar * filename;
	gint * trip_ptr = NULL;
	gfloat * loc_ptr = NULL;
	gint steps = 0;
	gchar val[20];
	ConfigFile *cfgfile;

	filename = g_strconcat(gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel)),NULL);

	cfgfile = cfg_open_file(filename);
	if (!cfgfile)
		cfgfile = cfg_new();

	steps = Color_map.steps;
	trip_ptr = Color_map.triplets;
	loc_ptr = Color_map.locations;

	cfg_write_int(cfgfile, "General", "steps", steps);

	for (j=1;j<=steps;j++)
	{
		sprintf(val,"step_%i",j);
		cfg_write_float(cfgfile, "General", val, *loc_ptr);
		loc_ptr++;

		sprintf(val,"red_%i",j);
		cfg_write_int(cfgfile, "Colormap", val, *trip_ptr);
		trip_ptr++;
		sprintf(val,"green_%i",j);
		cfg_write_int(cfgfile, "Colormap", val, *trip_ptr);
		trip_ptr++;
		sprintf(val,"blue_%i",j);
		cfg_write_int(cfgfile, "Colormap", val, *trip_ptr);
		trip_ptr++;
	}

	cfg_write_file(cfgfile, filename);
	cfg_free(cfgfile);

	Color_map.filename = g_strdup(filename);

	g_free(filename);

	gtk_widget_hide(GTK_WIDGET(filesel));
}

void load_colormap(GtkWidget * widget, GtkFileSelection *filesel)
{
	gchar * filename;
	filename = g_strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel)));
	read_colormap(filename);
	gtk_widget_destroy(GTK_WIDGET(filesel));
	init_colortab();
	gradient_update();
	selection_open = 0;
}
void read_colormap(char * filename)
{
	/* This routine needs to be rewritten for the new colortable file format
	 * The new format has a variable number of steps (up to 50 or so).
	 * Thus the file has to store hte number of steps, and their positions 
	 * in the map. (percentage of height sound simple enuf to me..)
	 */

	ConfigFile *cfgfile;
	gint steps=0;
	gint i = 0;
	gint *trip_ptr=NULL;
	gfloat *loc_ptr=NULL;
	gchar val[20];


	cfgfile = cfg_open_file(filename);
	/* For some reason I can't figure out I cannot use the read functions
	 * below to read directly into the structures (start.red and so on) so
	 * I used this functional hack, which I'd like to remove eventually.
	 * (using the temporary storage array.. :( 
	 */
	if (!cfgfile)
	{
		/* NOT FOUND, load default instead */
		cfgfile = cfg_open_file(g_strconcat(g_get_home_dir(), "/.eXtace/ColorMaps/", "Default",NULL));
	}
	if (cfgfile)
	{
		cfg_read_int(cfgfile, "General", "Steps", &steps);
		if (steps == 0)
		{
			/*	    This is an older file, we should convert it to the newer format. */
			printf("WARNING, old colormap file found,  converting on the fly *\n");
			steps = 5;  /* Default for old format files */
			Color_map.triplets = malloc(steps*3*sizeof(gint));
			Color_map.locations = malloc(steps*sizeof(gfloat));
			Color_map.steps = steps;
			trip_ptr = Color_map.triplets;
			loc_ptr = Color_map.locations;
			cfg_read_int(cfgfile, "Colormap", "red_start", trip_ptr++);
			cfg_read_int(cfgfile, "Colormap", "green_start", trip_ptr++);
			cfg_read_int(cfgfile, "Colormap", "blue_start", trip_ptr++);
			*loc_ptr = 0.0;
			loc_ptr++;
			cfg_read_int(cfgfile, "Colormap", "red_pt2", trip_ptr++);
			cfg_read_int(cfgfile, "Colormap", "green_pt2", trip_ptr++);
			cfg_read_int(cfgfile, "Colormap", "blue_pt2", trip_ptr++);
			*loc_ptr = 0.25;
			loc_ptr++;
			cfg_read_int(cfgfile, "Colormap", "red_pt3", trip_ptr++);
			cfg_read_int(cfgfile, "Colormap", "green_pt3", trip_ptr++);
			cfg_read_int(cfgfile, "Colormap", "blue_pt3", trip_ptr++);
			*loc_ptr = 0.5;
			loc_ptr++;
			cfg_read_int(cfgfile, "Colormap", "red_pt4", trip_ptr++);
			cfg_read_int(cfgfile, "Colormap", "green_pt4", trip_ptr++);
			cfg_read_int(cfgfile, "Colormap", "blue_pt4", trip_ptr++);
			*loc_ptr = 0.75;
			loc_ptr++;
			cfg_read_int(cfgfile, "Colormap", "red_end", trip_ptr++);
			cfg_read_int(cfgfile, "Colormap", "green_end", trip_ptr++);
			cfg_read_int(cfgfile, "Colormap", "blue_end", trip_ptr);
			*loc_ptr = 1.0;

		}
		else
		{
			// Allocate memory for the incoming data
			Color_map.triplets = malloc(steps*3*sizeof(gint));
			Color_map.locations = malloc(steps*sizeof(gfloat));
			Color_map.steps = steps;

			trip_ptr = Color_map.triplets;
			loc_ptr = Color_map.locations;

			for (i=1;i<=steps;i++)
			{
				/* 		SEQUENCE
				 * create variable name that corresponds with config file
				 * read value from file
				 * check limits and adjust if too high
				 * incrment ptr for next read
				 */
				/*  Location point */
				sprintf(val,"step_%i",i);
				cfg_read_float(cfgfile, "General",val , loc_ptr);
				*loc_ptr = *loc_ptr < 1.0 ? *loc_ptr :1.0;
				loc_ptr++;

				/* Red Value for above location pt */
				sprintf(val,"red_%i",i);
				cfg_read_int(cfgfile, "Colormap",val , trip_ptr);
				*trip_ptr = *trip_ptr < 255 ? *trip_ptr :255;
				trip_ptr++;

				/* Green Value for above location pt */
				sprintf(val,"green_%i",i);
				cfg_read_int(cfgfile, "Colormap",val, trip_ptr);
				*trip_ptr = *trip_ptr < 255 ? *trip_ptr :255;
				trip_ptr++;

				/* Blue Value for above location pt */
				sprintf(val,"blue_%i",i);
				cfg_read_int(cfgfile, "Colormap",val, trip_ptr);
				*trip_ptr = *trip_ptr < 255 ? *trip_ptr :255;
				trip_ptr++;
			}

		}

		cfg_free(cfgfile);

		Color_map.filename = g_strdup(filename);
	}
	else
	{
		g_free(Color_map.filename);
		Color_map.filename = (gchar *)NULL;
	}
	g_free(filename);
}

void update_gradient(GtkWidget *widget, int y)
{
	gdouble color[3];

	/* Get current color where we clicked */
	gtk_color_selection_get_color (GTK_COLOR_SELECTION(colorseldlg),color);

	/* Fit to a unsigned 16 bit integer (0..65535) and
	 * insert into the GdkColor structure */

	temp_color.red = (guint16)(color[0]*255);
	temp_color.green = (guint16)(color[1]*255);
	temp_color.blue = (guint16)(color[2]*255);

	/*    y = MAXBANDS-y; */
	if (y <= 1*(MAXBANDS/8)) /* start color */
		*start = temp_color;
	if ((y > 1*(MAXBANDS/8)) && (y <= 3*(MAXBANDS/8))) /* part 2 color */
		*pt2 = temp_color;
	if ((y > 3*(MAXBANDS/8)) && (y <= 5*(MAXBANDS/8))) /* part 3 color */
		*pt3 = temp_color;
	if ((y > 5*(MAXBANDS/8)) && (y <= 7*(MAXBANDS/8))) /* part 4 color */
		*pt4 = temp_color;
	if (y > 7*(MAXBANDS/8)) /* end color */
		*end = temp_color;
}

void init_colortab()
{
	/* this DOES NOT WORK RIGHT yet... */

	gint i,ii,j;
	gint w, h;
	gint r,g,b;
	gint divisor;
	gint amount;
	gushort this_red,this_green,this_blue,next_red,next_green,next_blue;
	unsigned char *data;

	j=0;

	/* This section is ONLY valid if all points are evenly spaced in the map */
	/* divisor is the number of times we need to run thru the interpolator
	 * to "fill in the blanks" betwwne steps for a smooth gradient
	 * It should be "steps-1". i.e 5 steps, has four patches of gradient fill 
	 * in between the steps. (don't forget the ends)
	 */
	divisor = Color_map.steps-1;
	for (i=0;i<divisor;i++)
	{
		this_red=Color_map.triplets[(i*3)+0];
		this_green=Color_map.triplets[(i*3)+1];
		this_blue=Color_map.triplets[(i*3)+2];

		next_red=Color_map.triplets[((i+1)*3)+0];
		next_green=Color_map.triplets[((i+1)*3)+1];
		next_blue=Color_map.triplets[((i+1)*3)+2];
		amount = (gint)(1.0/(Color_map.locations[i+1]-Color_map.locations[i]));
		for(ii=0;ii<(MAXBANDS/amount);ii++)
		{
			cr[j]=(((((MAXBANDS/amount)-1)-ii)*this_red)+(ii*next_red))/((MAXBANDS/amount)-1);
			cg[j]=(((((MAXBANDS/amount)-1)-ii)*this_green)+(ii*next_green))/((MAXBANDS/amount)-1);
			cb[j]=(((((MAXBANDS/amount)-1)-ii)*this_blue)+(ii*next_blue))/((MAXBANDS/amount)-1);
			j++;
		}
	}
	/* This section is ONLY valid if all points are evenly spaced in the map */


	for(i=0;i<MAXBANDS;i++)
	{
		for(j=0;j<MAXBANDS;j++)
		{
			r=cr[j]+((i-16)*2);
			g=cg[j]+((i-16)*2);
			b=cb[j]+((i-16)*2);
			if (r < 0) r=0;
			else if (r > 255) r=255;
			if (g < 0) g=0;
			else if (g > 255) g=255;
			if (b < 0) b=0;
			else if (b > 255) b=255;
			colortab[i][j]=gdk_imlib_best_color_match(&r,&g,&b);
		}
	}
	data=malloc(MAXBANDS*3);
	memset((void *)data, 0, MAXBANDS*3);
	for(i=0;i<MAXBANDS;i++)
	{
		data[i*3]=cr[i];
		data[(i*3)+1]=cg[i];
		data[(i*3)+2]=cb[i];
	}
	im=gdk_imlib_create_image_from_data(data,NULL,1,MAXBANDS);
	free(data);
	w=MAXBANDS;
	h=im->rgb_height;

	for(i=128;i<256;i++)
	{
		gdk_imlib_render(im,1,i-127);
		grad[i]=gdk_imlib_move_image(im);
	}
	gdk_imlib_flip_image_vertical(im);
	for(i=0;i<127;i++)
	{
		gdk_imlib_render(im,1,127-i);
		grad[i]=gdk_imlib_move_image(im);
	}
	colortab_ready = 1;
}

void grad_win_create()
{
	GtkWidget * hbox = NULL;
	GtkWidget * grad_win = NULL;
	GtkWidget * vbox = NULL;
	GtkWidget * eventbox = NULL;
	GtkWidget *	frame = NULL;
	GtkWidget * button = NULL;
	GtkWidget * sep = NULL;

	if(!grad_win)
	{
		grad_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		grad_win_ptr = grad_win;
		gtk_widget_set_uposition(grad_win,grad_x_origin,grad_y_origin);
		gtk_window_set_title(GTK_WINDOW(grad_win),"Color Picker");
		gtk_window_set_policy(GTK_WINDOW(grad_win), 
				FALSE,		/* allow shrink */
				FALSE,		/* allow grow */
				FALSE);		/* auto shrink */
		gtk_signal_connect(GTK_OBJECT(grad_win),"delete_event",
				GTK_SIGNAL_FUNC(close_grad_win),NULL);

		hbox = gtk_hbox_new(FALSE,0);
		gtk_container_add(GTK_CONTAINER(grad_win), hbox);

		frame = gtk_frame_new("Colormap");
		gtk_box_pack_start(GTK_BOX(hbox),frame,TRUE,TRUE,0);

		eventbox = gtk_event_box_new();
		gtk_container_add(GTK_CONTAINER(frame),eventbox);
		gtk_widget_set_events (eventbox, GDK_BUTTON_PRESS_MASK);
		gtk_signal_connect(GTK_OBJECT(eventbox), "button_press_event",
				(GtkSignalFunc) color_event, NULL);
		gtk_signal_connect(GTK_OBJECT(eventbox), "focus_out_event",
				(GtkSignalFunc) grad_win_save_state, NULL);
		gtk_signal_connect(GTK_OBJECT(eventbox), "motion_notify_event",
				(GtkSignalFunc) motion_notify_event, NULL);
		gtk_widget_set_events (eventbox,GDK_BUTTON_PRESS_MASK
				| GDK_EXPOSURE_MASK
				| GDK_FOCUS_CHANGE_MASK);

		gtk_widget_set_usize(eventbox,256,256);
		gtk_widget_realize(eventbox);


		grad_disp = gtk_drawing_area_new();
		gtk_container_add(GTK_CONTAINER(eventbox),grad_disp);

		gtk_widget_show(grad_disp);
		gtk_widget_realize(grad_disp);

		vbox = gtk_vbox_new(FALSE,0);
		gtk_box_pack_start(GTK_BOX(hbox),vbox,TRUE,TRUE,0);

		frame = gtk_frame_new("Color Selector");
		gtk_box_pack_start(GTK_BOX(vbox),frame,FALSE,TRUE,0);

		colorseldlg = gtk_color_selection_new();

		gtk_container_add(GTK_CONTAINER(frame),colorseldlg);

		sep = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(vbox),sep,TRUE,TRUE,0);

		hbox = gtk_hbox_new(TRUE,0);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

		button = gtk_button_new_with_label("Set Color");
		gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
		gtk_signal_connect(GTK_OBJECT(button), "clicked",
				(GtkSignalFunc)color_button, (gpointer)SET_COLOR);

		button = gtk_button_new_with_label("Close");
		gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
		gtk_signal_connect(GTK_OBJECT(button), "clicked",
				(GtkSignalFunc)color_button, (gpointer)CLOSE);

		hbox = gtk_hbox_new(TRUE,0);
		gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

		button = gtk_button_new_with_label("Save");
		gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
		gtk_signal_connect(GTK_OBJECT(button), "clicked",
				(GtkSignalFunc)color_button, (gpointer)SAVE);

		button = gtk_button_new_with_label("Load");
		gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,0);
		gtk_signal_connect(GTK_OBJECT(button), "clicked",
				(GtkSignalFunc)color_button, (gpointer)LOAD);

		gtk_widget_show_all(grad_win);
	}
	if (!grad_pixmap)
	{

		grad_pixmap = gdk_pixmap_new(grad_disp->window,width,height,
				gtk_widget_get_visual(grad_disp)->depth);
		//	 init_colortab();
	}
}

void gradient_update()
{
	int h = MAXBANDS;
	int w = MAXBANDS;

	if (grad_win_ptr)
	{
		if (colortab_ready == 0)
		{
			init_colortab();
		}
		gdk_imlib_render(im,w,h);
		grad_pixmap=gdk_imlib_move_image(im);

		gdk_window_set_back_pixmap(grad_disp->window,grad_pixmap,0);

		gdk_draw_pixmap(grad_disp->window,
				grad_disp->style->fg_gc[GTK_WIDGET_STATE (grad_disp)],
				grad_pixmap,
				0,0,
				0,0,
				w,h);
		if (!grad_win_present)
		{
			gtk_widget_show(grad_win_ptr);
			gtk_widget_set_uposition(grad_win_ptr,grad_x_origin,grad_y_origin);
			grad_win_present = 1;
		}
	}
}



gint grad_win_save_state(GtkWidget *widget, GdkEventFocus *event)
{
	int x,y;
	if(!grad_win_present)
		return TRUE;
	if (!event->in)
	{
		gdk_window_get_root_origin((gpointer) grad_win_ptr->window, &x, &y);
		grad_x_origin = x;
		grad_y_origin = y;
	}

	return FALSE;
}
