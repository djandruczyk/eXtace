/*
 * win_func.c extace source file
 * 
 * /GDK/GNOME sound (esd) system output display program
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
#include <globals.h>
#include <protos.h>
#include <math.h>
#include <gtk/gtk.h>
#include <esd.h>

void setup_datawindow(GtkWidget *widget, WindowFunction function)
{
    gint i=0;
    gdouble sumw=0.0;
#ifdef HAVE_LIBRFFTW
    gdouble *ptr;
#endif
#ifdef HAVE_LIBDRFFTW
    gdouble *ptr;
#endif
#ifdef WINDOW_DEBUG
    FILE *f;
#endif
    gint winlen;
    gint alpha = 1;
    gint padding = 0;
    gint padsize = 0;

#ifdef WINDOW_DEBUG
    f = fopen("/tmp/window.in", "w");
    fprintf(f, "I\twin_val\t\twin_sum\n");
#endif

    ptr = datawindow;

    switch (winstyle)
    {
	case FULL:
	    padding = 0;
	    winlen = nsamp;
	    break;
	case HALF:
	    padding = 1;
	    padsize = nsamp/4;
	    winlen = nsamp/2;
	    break;
	case QUARTER:
	    padding = 1;
	    padsize = nsamp*3/8;
	    winlen = nsamp/4;
	    break;
	case EIGHTH:
	    padding = 1;
	    padsize = nsamp*7/16;
	    winlen = nsamp/8;
	    break;
	default:
	    padding = 0;
	    winlen = nsamp;
	    break;
    }
    if (padding)
    {
	for (i=0;i < padsize;i++)
	{
	    *ptr = 0;
#ifdef WINDOW_DEBUG
	    fprintf(f, "%ip\t%f\t%f\n", i, *ptr, sumw);
#endif
	    ptr++;
	}
    }
    for(i=0;i<winlen;i++)
    {
	switch(function)
	{
	    case HAMMING:   /* Hamming */
		*ptr = (0.54-0.46*cos(2*M_PI*i/(winlen-1)));
		break;
	    case HANNING:    /* Hanning */
		*ptr = (0.5 - 0.5*cos(2*M_PI*i/(winlen-1)));
		break;

	    case BLACKMAN:    /* Blackman */
		*ptr = (0.42 - 0.5*cos((2*M_PI*i)/(winlen-1))+0.08*cos((2*2*M_PI*i)/(winlen-1)));
		break;

	    case BLACKMAN_HARRIS:    /* Blackman */
		*ptr = (0.35875 - 0.48829*cos((2*M_PI*i)/(winlen-1))+0.14128*cos((2*2*M_PI*i)/(winlen-1))-0.01168*cos((3*2*M_PI*i)/(winlen-1)));
		break;

	    case GAUSSIAN:    /* Gaussian */
		*ptr = (exp(-alpha / ((double)winlen*(double)winlen)
			    * ((double)2*i-winlen)*((double)2*i-winlen)));
		break;
	    case WELCH:    /* Welch */
		*ptr = (1 -  ((double)(2*i-winlen)/(double)(winlen+1))
			    * ((double)(2*i-winlen)/(double)(winlen+1)));
		break;
		
	    case PARZEN:    /* Parzen */
		*ptr = (1 - fabs((double)(2*i-winlen)/(double)(winlen+1)));
		break;

	    case RECTANGULAR:    /* Rectangular */
		*ptr = 1.0;
		break;

	    default:   /* Hamming */
		*ptr = (0.54-0.46*cos(2*M_PI*i/(winlen-1)));
		break;
	}
	sumw += *ptr;

#ifdef WINDOW_DEBUG
	fprintf(f, "%iw\t%f\t%f\n", i, *ptr, sumw);
#endif
	ptr++;
	
    }
    if (padding)
    {
	for (i=0;i < padsize;i++)
	{
	    *ptr = 0;
#ifdef WINDOW_DEBUG
	    fprintf(f, "%ip\t%f\t%f\n", i, *ptr, sumw);
#endif
	    ptr++;
	}
    }
#ifdef WINDOW_DEBUG
    fclose(f);
#endif

#ifdef WINDOW_DEBUG
    fprintf(stderr, "window sum is %f\n",sumw);
#endif
    window_func = function;
}

