/*  XMMS - Cross-platform multimedia player
 *  Copyright (C) 1998-1999  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef EXTACE_CONFIGFILE_H
#define EXTACE_CONFIGFILE_H 1

#include <glib.h>

typedef struct
{
	gchar *key;
	gchar *value;
}
ConfigLine;

typedef struct
{
	gchar *name;
	GList *lines;
}
ConfigSection;

typedef struct
{
	GList *sections;
}
ConfigFile;

ConfigFile *cfg_new(void);
ConfigFile *cfg_open_file(gchar * filename);
gboolean cfg_write_file(ConfigFile * cfg, gchar * filename);
void cfg_free(ConfigFile * cfg);

gboolean cfg_read_string(ConfigFile * cfg, gchar * section, gchar * key, gchar ** value);
gboolean cfg_read_int(ConfigFile * cfg, gchar * section, gchar * key, gint * value);
gboolean cfg_read_boolean(ConfigFile * cfg, gchar * section, gchar * key, gboolean * value);
gboolean cfg_read_float(ConfigFile * cfg, gchar * section, gchar * key, gfloat * value);
gboolean cfg_read_double(ConfigFile * cfg, gchar * section, gchar * key, gdouble * value);

void cfg_write_string(ConfigFile * cfg, gchar * section, gchar * key, gchar * value);
void cfg_write_int(ConfigFile * cfg, gchar * section, gchar * key, gint value);
void cfg_write_boolean(ConfigFile * cfg, gchar * section, gchar * key, gboolean value);
void cfg_write_float(ConfigFile * cfg, gchar * section, gchar * key, gfloat value);
void cfg_write_double(ConfigFile * cfg, gchar * section, gchar * key, gdouble value);

void cfg_remove_key(ConfigFile * cfg, gchar * section, gchar * key);

#endif
