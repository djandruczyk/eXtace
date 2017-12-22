/*
 * Copyright (C) 2003 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux eXtace Audio visualizer
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute, etc. this as long as all the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#ifndef __PA_UTILS_H__
#define __PA_UTILS_H__

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <pulse/pulseaudio.h>

typedef struct pa_devicelist {
        uint8_t initialized;
        char name[512];
        uint32_t index;
        char description[256];
} pa_devicelist_t;

/* Prototypes */
int populate_inputs_from_pulseaudio(GtkWidget *);
void pa_state_cb(pa_context *c, void *userdata);
void pa_sinklist_cb(pa_context *c, const pa_sink_info *l, int eol, void *userdata);
void pa_sourcelist_cb(pa_context *c, const pa_source_info *l, int eol, void *userdata);
int pa_get_devicelist(pa_devicelist_t *input, pa_devicelist_t *output);

#endif
