/*

Keg Tracker Stars.
Copyright (C) 1998 Rasterman

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <globals.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stars.h>
#include <stdlib.h>
#include <time.h>

#define STARS_NUM 640

GtkWidget *stars;

static void
kt_stars_rotate_point(gfloat *px, gfloat *py, gfloat *pz,
		      gfloat ax, gfloat ay, gfloat az)
{
	gfloat xx,yy,zz;
	gfloat x,y,z;

	x=*px;
	y=*py;
	z=*pz;

	xx=x*cos((float)az);
	yy=x*sin((float)az);
	x=xx+(y*cos((float)az+M_PI_2));
	y=yy+(y*sin((float)az+M_PI_2));

	xx=x*cos((float)ay);
	zz=x*sin((float)ay);
	x=xx+(z*cos((float)ay+M_PI_2));
	z=zz+(z*sin((float)ay+M_PI_2));

	zz=z*cos((float)ax);
	yy=z*sin((float)ax);
	z=zz+(y*cos((float)ax+M_PI_2));
	y=yy+(y*sin((float)ax+M_PI_2));

	*px=x;
	*py=y;
	*pz=z;
}


void kt_stars_update_func(GtkWidget *area)
{
	static GdkGC     *gc = NULL;
	static GdkColor   black, cols[8];
	static gfloat     stars[STARS_NUM][3];
	static int        sstars[3][STARS_NUM][2];
	static gfloat     angle = 0.0;
	static GdkPixmap *pmap = NULL;

	GdkColormap     *cmap;
	gint             x, y, ww, hh, i;
	gint             w, h, sx, sy;
	gint             br;
	gfloat           a1, a2, a3;
	GdkPixmap       *lp, *lm;

	pmap = (GdkPixmap *)gtk_object_get_data(GTK_OBJECT(area), "main_pixmap");
	gdk_window_get_size(pmap, &w, &h);
	gdk_window_get_size(area->window, &ww, &hh);
	if ((w != ww) || (h != hh))
	{
		gdk_pixmap_unref(pmap);
		pmap = gdk_pixmap_new(area->window, ww, hh, -1);
		gdk_window_set_back_pixmap(area->window, pmap, FALSE);
		gtk_object_set_data(GTK_OBJECT(area), "main_pixmap", pmap);
	}
	if (gtk_object_get_data(GTK_OBJECT(area), "reset") != NULL)
	{
		/* random generation */
		if (rand()%2)
		{
			for (i = 0; i < STARS_NUM; i++)
			{
				stars[i][0] = (gfloat)(rand()%1024 - 512) / 5;
				stars[i][1] = (gfloat)(rand()%1024 - 512) / 5;
				stars[i][2] = (gfloat)(rand()%1024 - 512) / 5;
			}  
		}
		else
		{
			for (i = 0; i < STARS_NUM; i++)
			{
				a1 = ((gfloat)i / STARS_NUM) + 0.001;

				stars[i][0] = (cos(20 * a1) * 200 * a1) +
					((gfloat)(rand()%1024 - 512) * a1 / 30);
				stars[i][1] = ((gfloat)(rand()%1024 - 512) * a1 / 30);
				stars[i][2] = (sin(20 * a1) * 200 * a1) +
					((gfloat)(rand()%1024 - 512) * a1 / 30);
			}
		}      
		gtk_object_set_data(GTK_OBJECT(area), "reset", NULL);
	}
	if (!gc)
	{
		cmap = gtk_widget_get_colormap(area);
		gc = gdk_gc_new(pmap);
		black.red   = 0;
		black.green = 0;
		black.blue  = 0;
		cols[0].red   = 32 << 8;
		cols[0].green = 32 << 8;
		cols[0].blue  = 32 << 8;
		cols[7].red   = 255 << 8;
		cols[7].green = 255 << 8;
		cols[7].blue  = 255 << 8;
		gdk_color_alloc(cmap, &black);
		for (i = 0; i < 8; i++)
		{
			cols[i].red = cols[0].red + (i * (cols[7].red  - cols[0].red) / 7);
			cols[i].green = cols[0].green + (i * (cols[7].green  - cols[0].green) / 7);
			cols[i].blue = cols[0].blue + (i * (cols[7].blue  - cols[0].blue) / 7);
			gdk_color_alloc(cmap, &(cols[i]));
		}
		/* random generation */
		if (rand()%2)
		{
			for (i = 0; i < STARS_NUM; i++)
			{
				stars[i][0] = (gfloat)(rand()%1024 - 512) / 5;
				stars[i][1] = (gfloat)(rand()%1024 - 512) / 5;
				stars[i][2] = (gfloat)(rand()%1024 - 512) / 5;
			}  
		}
		else
		{
			for (i = 0; i < STARS_NUM; i++)
			{
				a1 = ((gfloat)i / STARS_NUM) + 0.001;

				stars[i][0] = (cos(20 * a1) * 200 * a1) +
					((gfloat)(rand()%1024 - 512) * a1 / 30);
				stars[i][1] = ((gfloat)(rand()%1024 - 512) * a1 / 30);
				stars[i][2] = (sin(20 * a1) * 200 * a1) +
					((gfloat)(rand()%1024 - 512) * a1 / 30);
			}
		}
	}
	gdk_window_get_size(area->window, &w, &h);
	gdk_gc_set_foreground(gc, &black);
	gdk_draw_rectangle(pmap, gc, 1, 0, 0, w, h);

	a1 = (sin(angle)) / 20;
	a2 = (sin(angle * 2)) / 15;
	a3 = (cos(angle * 3)) / 10;
	for (i = 0; i < STARS_NUM; i++)
	{
		kt_stars_rotate_point(&(stars[i][0]), &(stars[i][1]), &(stars[i][2]),
				a1, a2 ,a3);
		/*
		   sx = (w / 2) + (gint)(stars[i][0] / ((stars[i][2] + 600)/ 400));
		   sy = (h / 2) + (gint)(stars[i][1] / ((stars[i][2] + 600)/ 400));
		 */
		sx = (w / 2) + (gint)(((gfloat)w / 256) * stars[i][0] +((stars[i][2] + 600)/ 400));
		sy = (h / 2) + (gint)(((gfloat)w / 256) * stars[i][1] +((stars[i][2] + 600)/ 400));
		br = (gint)(stars[i][2] + 64) / 16;
		if (br < 0) 
			br = 0;
		else if (br > 7)
			br = 7;
		gdk_gc_set_foreground(gc, &(cols[7 - br]));
		gdk_draw_line(pmap, gc, 
				sstars[0][i][0], sstars[0][i][1], 
				sx, sy);
		gdk_gc_set_foreground(gc, &(cols[(7 - br) / 2]));
		gdk_draw_line(pmap, gc, 
				sstars[1][i][0], sstars[1][i][1], 
				sstars[0][i][0], sstars[0][i][1]);
		gdk_gc_set_foreground(gc, &(cols[(7 - br) / 4]));
		gdk_draw_line(pmap, gc, 
				sstars[2][i][0], sstars[2][i][1], 
				sstars[1][i][0], sstars[1][i][1]);
		sstars[2][i][0] = sstars[1][i][0];
		sstars[2][i][1] = sstars[1][i][1];
		sstars[1][i][0] = sstars[0][i][0];
		sstars[1][i][1] = sstars[0][i][1];
		sstars[0][i][0] = sx;
		sstars[0][i][1] = sy;
	}  
	lp = (GdkPixmap *)gtk_object_get_data(GTK_OBJECT(area), "logo_pmap");
	lm = (GdkPixmap *)gtk_object_get_data(GTK_OBJECT(area), "logo_mask");

	if ((lp) && (lm))
	{
		gdk_gc_set_clip_mask(gc, lm);
		gdk_window_get_size(lp, &ww, &hh);
		x = (w - ww) / 2;
		y = (h - hh) / 2;
		gdk_gc_set_clip_origin(gc, x, y);
		gdk_draw_pixmap(pmap, gc, lp, 0, 0, x, y, ww, hh);
		gdk_gc_set_clip_mask(gc, NULL);
	}

	angle += 0.02;
	gdk_window_clear(area->window);
}

GtkWidget *
kt_stars_new(GtkWidget *area, GdkPixmap *pixmap)
{
	srand(time(NULL));
	/*  area = gtk_drawing_area_new();*/
	gtk_object_set_data(GTK_OBJECT(area), "main_pixmap", pixmap);
	return area;
}


void
kt_stars_set_logo_pixmp(GtkWidget *s, GdkPixmap *pmap, GdkPixmap *mask)
{
	gtk_object_set_data(GTK_OBJECT(s), "logo_pmap", pmap);
	gtk_object_set_data(GTK_OBJECT(s), "logo_mask", mask);
}

