/*
 * draw.c source file for extace
 * 
 /GDK/GNOME sound (esd) system output display program
 * 
 * Based on the original extace written by The Rasterman and Michael Fulbright
 *   
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You may use this program at your own risk.
 */

#include <config.h>
#include <globals.h>
#include <protos.h>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif


/* See globals.h for variable declarations and DEFINES */
/* FPS debugging variables
 * 
 * gfloat elapsed_time_sec;
 * gfloat elapsed_time_usec;
 * gfloat elapsed;
*/
 
void draw_start()
{
    if (!paused)
    {
	/* Check to make sure its not already running */
	if (display_id)
	{
	    printf("Bug detected, display already running NON-fatal\n");
	    draw_stop();
	}

	if (mode == STARS)
	{
	    display_id = gtk_timeout_add((int)((1.0/(float)refresh_rate)*1000.0),(GtkFunction)kt_stars_update_func, stars);
	}
	else
	{

	    display_id = gtk_timeout_add((int)((1.0/(float)refresh_rate)*1000.0), (GtkFunction)draw, NULL);
	}
    }
    else
	printf("Display in paused state,  clik on \"Resume Display\" in Options Panel\n");

}
void draw_stop()
{
    if (display_id)    
	gtk_timeout_remove(display_id);
    display_id = 0;
}
int draw(void)
{
    gint i,j,k,x;
    /*	Used for frame per sec debugging 
	gint elapsed_time_sec=0;
	gint elapsed_time_usec=0;
	gfloat elapsed=0.0;
     */

    gfloat bands_per_block = 0;
    gfloat orig_bands_per_block = 0;
    gint fragcount = 0;
    gfloat fractional_bands = 0.0;
    static gint lin_x_axis[MAXBANDS];
    static gint log_x_axis[MAXBANDS];
    gdouble val=0;
    gint sum=0;
    gint count=0;
    gfloat resolution=0.0;

    if (!GetFFT()) return (TRUE);

    if (!main_display->window) return (TRUE);

    /* Next set of routines ONLY ONLY needs to be done for 3D Landform
     * and 2D EQ. Scope, spikes, and spectrogram don't need these, so
     * why waste the cpu time..
     */

    if (mode == LAND_3D)
    {
	for (i=1; i<nsamp/2; i++)
	{
	    disp_val[i-1] = (norm_fft[i]/20)*landflip;
	}
	disp_val[(nsamp/2)-1]=0;
    }
    else if (mode == SPIKE_3D)
    {
	for (i=1; i<nsamp/2; i++)
	{
	    disp_val[i-1] = (norm_fft[i]/20)*spikeflip;
	}
	disp_val[(nsamp/2)-1]=0;
    }
    else
    {
	for (i=1; i<nsamp/2; i++)
	{			
	    disp_val[i-1] = (norm_fft[i]/20);
	}
	disp_val[(nsamp/2)-1]=0;
    }

    if ((mode == LAND_3D) || (mode == EQ_2D))
    {
	/* Gives a "log-like" x axis (for 2D (EQ like analyzer))
	 * hacking to make it work better with the various displays
	 * the bin[i] deteremines how much data is summed together for the 
	 * display since the display can't fit a 1024 point fft nicely.
	 */ 
	if (axis_type == LINEAR)
	{
	    if (recalc_scale)
	    {
		for(i = 0;i < bands;i++)
		{
		    /* Linear x axis */
		    lin_x_axis[i]=(nsamp/2)/bands; 
		}
	    }
	}
	if (axis_type == LOG)
	{
	    if (recalc_scale)
	    {
		scalefactor = 7.0;

		count = 0;
recalc:
		count++;
		scale = bands/scalefactor;
		sum = 0;
		for (i=0; i < bands ; i++)
		{
		    log_x_axis[i] = exp((i/scale));
		    sum = sum + log_x_axis[i];
		}
		/* reduction algorithm, to get scalefactor so that ALL datapoints
		 *in the FFT get onto the display */
		if (sum > nsamp/2)
		{
		    if (sum > (2.0*(nsamp/2)))
			scalefactor = scalefactor - .500;
		    if (sum > (1.25*(nsamp/2)))
			scalefactor = scalefactor - .150;
		    if (sum > (1.15*(nsamp/2)))
			scalefactor = scalefactor - .025;
		    if (sum > (1.05*(nsamp/2)))
			scalefactor = scalefactor - .010;
		    if (sum >= (1.02*(nsamp/2)))
			scalefactor = scalefactor - .005;
		    if (sum >= (1.01*(nsamp/2)))
			scalefactor = scalefactor - .002;
		    if (sum < (1.01*(nsamp/2)))
			scalefactor = scalefactor - .001;
		    goto recalc;
		}
		if (sum < (nsamp/2)*.99)
		{
		    if (sum < (nsamp/2)*.99)
			scalefactor = scalefactor + .001;
		    if (sum < (nsamp/2)*.98)
			scalefactor = scalefactor + .002;
		    if (sum < (nsamp/2)*.95)
			scalefactor = scalefactor + .005;
		    if (sum < (nsamp/2)*.85)
			scalefactor = scalefactor + .025;
		    if (sum < (nsamp/2)*.75)
			scalefactor = scalefactor + .155;
		    goto recalc;
		}
#ifdef SCALEDEBUG
		for (i=0; i<bands; i++)
		    g_print("Log_axis[%i] is %i\n",i,log_x_axis[i]);
		g_print("It took %i iterations to get the scalefactor\n",count);
		g_print("Scalefactor %f\n",scalefactor);
		g_print("Number of points in display is %i\n",sum);
#endif
		recalc_scale = 0;
	    }
	}




	/*
	 * this function groups together the data from the FFT so that it can
	 * fit into the smaller division displayed on screen, since a 1024 point
	 * fft doesn't fit wwell into 32 or 64 divisions.  the lin/log_x_axis
	 * arrays determine how the fft data is broken up for a linear or log 
	 * based frequency axis
	 */

	j=0;
	k=0;
	x=0;
	bands_per_block = (int)(floor((nsamp/2)/bands));
	orig_bands_per_block = bands_per_block;
	fractional_bands = (((float)nsamp/2.0)/(float)bands)-bands_per_block;

	if(recalc_markers)
	{
	    if (axis_type == LINEAR)
		resolution = ((float)RATE/2.0)/(float)bands;
	    else if (axis_type == LOG)
		resolution = ((float)RATE/2.0)/((float)nsamp/2.0);
	    switch (axis_type)
	    {
		case LINEAR:
		    freqmark[0]=resolution;
		    for (x=1;x<bands;x++)
			freqmark[x]= freqmark[x-1]+resolution;
		    break;
		case LOG:
		    freqmark[0]=resolution*log_x_axis[0];
		    for (x=1;x<bands;x++)
			freqmark[x]= freqmark[x-1]+(resolution*log_x_axis[x]);
		    break;
	    }
	    recalc_markers = 0;
	}

	for(i=0;i<bands;i++)
	{
	    val=0;
	    /* sum it, then average it so the scaling doesn't go haywire */
	    switch (axis_type)
	    {
		case LINEAR:
		    bands_per_block = bands_per_block + fractional_bands;
		    if (bands_per_block > orig_bands_per_block+1.0)
		    {
			x=floor(bands_per_block);
			for(j=0;j<x;j++)
			    val+=disp_val[k++];
			val/=(double)bands_per_block;
			bands_per_block -= 1.0;
			fragcount++;
		    }
		    else
		    {
			x=floor(bands_per_block);
			for(j=0;j<x;j++)
			    val+=disp_val[k++];
			val/=(double)bands_per_block;
		    }
		    break;
		case LOG:
		    for(j=0;j<(log_x_axis[i]);j++)
			val+=disp_val[k++];
		    val/=(double)log_x_axis[i];
		    break;
	    }

	    if (bar_decay)
	    {
		if (mode == LAND_3D)
		{
		    if (landflip == 1)
		    {
			if (val<=(levels[i]-bar_decay_speed))
			{
			    levels[i]-=bar_decay_speed;
			    if (levels[i]<0)
				levels[i]=0;
			}
			else 
			    levels[i]=val;
		    }
		    else
		    {
			if (val>=(levels[i]-bar_decay_speed))
			{
			    levels[i]+=bar_decay_speed;
			    if (levels[i]>0)
				levels[i]=0;
			}
			else 
			    levels[i]=val;
		    }
		}
		else // 2D EQ
		{
		    if (val<=(levels[i]-bar_decay_speed))
		    {
			levels[i]-=bar_decay_speed;
			if (levels[i]<0)
			    levels[i]=0;
		    }
		    else
			levels[i]=val;
		}
	    }
	    else
		levels[i]=val;
	    if (peak_decay)
	    {
		trail_counter[i]--;
		if (val<=(trailers[i]))
		{
		    if (trail_counter[i]<=0)
		    {
			trailers[i]-=peak_decay_speed;
			if (trailers[i] < 0)
			    trailers[i] = 0;
		    }
		}
		else 
		{
		    trailers[i]=val;
		    trail_counter[i]=peak_hold_time;
		}
	    }
	    else
		trailers[i]=val;
	}

    }
    switch (mode)
    {
	case LAND_3D:
	    draw_land3d_fft();
	    break;
	case EQ_2D:
	    draw_2d_eq();
	    break;
	case SCOPE:
	    draw_scope();
	    break;
	case SPIKE_3D:
	    draw_spike_3d();
	    break;
	case VERT_SPECGRAM:
	    draw_vert_specgram();
	    break;
	case HORIZ_SPECGRAM:
	    draw_horiz_specgram();
	    break;

	default:
	    break;
    }
    /* shift current levels into the previous levels array, */
    for( i=0; i < bands; i++ ) 
    {
	plevels[i]=levels[i];
	ptrailers[i]=trailers[i];
    }
    /* Frame per second counter for debugging purposes... 
       frame_cnt++;
       if (frame_cnt == 10)
       {
       if (gettimeofday(&cur_time,NULL))
       g_print("Gettimeofday failed!\n");
       elapsed_time_sec = cur_time.tv_sec - last_time.tv_sec;
       elapsed_time_usec = cur_time.tv_usec - last_time.tv_usec;
       elapsed = elapsed_time_sec + (float)elapsed_time_usec/1000000;
       g_print("%f FPS\n", 10.0/elapsed);
       last_time.tv_sec=cur_time.tv_sec;
       last_time.tv_usec=cur_time.tv_usec;
       frame_cnt = 0;
       }
     */


    return (TRUE);

}
