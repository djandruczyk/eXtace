/*
 * reducer.c source file for extace
 * 
 /GDK/GNOME sound (esd) system output display program
 * 
 * Based on the original extace written by The Rasterman and Michael Fulbright
 *  
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


/* See globals.h for variable declarations and DEFINES */


/* pip = vertical line on screen
 * bin = datapoint from output of fft
 * axis_length is length of frequency axis on screen in PIXELS
 * bins_per_pip can go from above 0 up.  (depending on how large the fft is)
 * typically bins per pip is around 0.3-12 depending on FFT length and screen
 * size.
 * bins_per_pip is the number of frequency bins averaged into one pixel on 
 * screen.  This algorithm can handle bins_per_pip from just above 0 and up.
 * Hopefully it does it right.. :)
 *
 * disp_val[x] is the pixel representation of bins_per_pip of the fft.
 * i.e. The combination of multiple bins for display.
 *
 * i is the index for the pixels on the display
 *
 * j is the index for the fft bins
 *
 * This routine does a linear interpolation (well thats what I intended)
 * to smoothly fit the data on screen with minimal distortion.
 * Though it ain't perfect, as any form of interpolation will distort the
 * information somewhat from its original represenation.
 */

void reducer(int lowfreq, int hifreq ,int axis_length)
{
	gint i = 0;
	gint j = 0;
	gfloat pip = 0.0;
	gfloat running_total = 0.0;
	gfloat pip_total = 0.0;
	gfloat partial = 0.0;
	gint special_case = 0;
	gint count = 0;
	gint lowbin = 0;
	gint highbin = 0;
	gfloat hertz_per_bin = 0.0;
	gfloat bins_per_pip = 0.0;
	/* Ideally this should be rewritten to allow arbritrary frequency 
	 * spreads in the display. i.e low and high freq should be 
	 * selectable eventually so that you can zoom into the desired range 
	 */
	
	hertz_per_bin = (float)RATE/(float)nsamp;
	lowbin = lowfreq/hertz_per_bin;
	highbin = hifreq/hertz_per_bin;

//	bins_per_pip = ((float)nsamp/(RATE/bandwidth))/(fabs(axis_length));
	bins_per_pip = ((float)(highbin-lowbin))/fabs(axis_length);

//	printf("hertz/bin %f, lowbin %i, highbin %i bins_per_pip %f\n",hertz_per_bin,lowbin,highbin,bins_per_pip);

	if ((bins_per_pip <= 0.0) || (axis_length <= 0))
		printf("ERROR!!, bins_per_pip %f, axis_length %i\n",bins_per_pip,axis_length);
	pip = bins_per_pip;

	j = lowbin;
	
	while (i < axis_length)
	{
		if (j >= highbin*2)
			printf("reducer error, disp_val OVERFLOW!!\n");
		count ++;
		if (count > 10000)
		{
			g_print("ERROR in reducer!!!!\n");
			g_print("Main while loop counter = %i\n",i);
			g_print("Iterations = %i\n",count);
			g_print("pip value = %f\n",pip);
			g_print("Bins per pip value = %f\n",bins_per_pip);
			g_print("Running total = %f\n",running_total);
			g_print("Email the author with this information so he can fix it\n");
			exit(-4);
		}

		while (pip > 1.0)
		{
			pip_total += disp_val[j]*1.0;
			pip--;
			j++;
		}
		if ((pip <= 1.0)  && (bins_per_pip > 1.0))
		{
			pip_total += disp_val[j]*pip;
			pip_arr[i] = (gint)(pip_total/bins_per_pip);

			pip_total = disp_val[j]*(1.0-pip);
			pip = bins_per_pip - (1.0 -pip);
			j++;
			i++;
		}
		if (bins_per_pip == 1.0)
		{
			pip_arr[i] = (gint)disp_val[j]*1.0;
			i++;
			j++;
		}

		if (bins_per_pip < 1.0 )
		{
			if (running_total + pip >= 1.0)
			{
				partial = 1.0 - running_total;
				special_case = 1;
			}
			running_total += pip;
			if (special_case)
			{
				pip_total = disp_val[j]*partial;
				j++;
				pip_total += disp_val[j]*(running_total - 1.0);
				special_case = 0;
			}
			else
			{
				pip_total += disp_val[j]*pip;
			}
			if (running_total >= 1.0)
			{
				running_total -= 1.0;
			}

			pip_arr[i] = (gint)(pip_total/bins_per_pip);
			i++;
			pip_total = 0;

		}

	}
}
