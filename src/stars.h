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

#ifndef __STARS_H__
#define __STARS_H__

#include <gtk/gtk.h>

GtkWidget *kt_stars_new(GtkWidget *area, GdkPixmap *pixmap);
void kt_stars_start(GtkWidget *s, gint width, gint height);
void kt_stars_set_logo_pixmp(GtkWidget *s, GdkPixmap *pmap, GdkPixmap *mask);
void kt_stars_stop(GtkWidget *s);

#endif
