/*
 * audio_processing.c extace source file
 * 
 * /GTK sound (esd) system audio monitoring display program
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

#include <asm/errno.h>
#include <audio_processing.h>
#include <config.h>
#include <convolve.h>
#include <enums.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <math.h>
#ifdef HAVE_LIBRFFTW
#include <rfftw.h>
#endif 
#ifdef HAVE_LIBDRFFTW
#include <drfftw.h>
#endif 
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif


static gint delay;
static gint last_start;
static gint last_end;

void run_fft(void)
{
	int nsamp_sqd = nsamp*nsamp;
	gdouble *real_fft_out=NULL;
	gdouble *imag_fft_out=NULL;
	gdouble *fft_ptr=NULL;
	gint index1=0;


	rfftw_one(plan, raw_fft_in, raw_fft_out);
	norm_fft[0] = (raw_fft_out[0]*raw_fft_out[0])/nsamp_sqd;
	norm_fft[nsamp/2] = (raw_fft_out[nsamp/2]*raw_fft_out[nsamp/2])/nsamp_sqd;
	 /*This pointer will be INCREMENTED below...  */
	real_fft_out=raw_fft_out; /* Beginning of buffer */

	/* This pointer will be DECREMENTED below... */
	imag_fft_out=raw_fft_out+nsamp-1;	/* End of buffer,
						 * the "mirrored side" 
						 */

	/* take the log */
	fft_ptr = norm_fft;
	index1 = nsamp;
	while (index1--)
	{
		//	*fft_ptr=multiplier*(noise_floor+log10((((*real_fft_out * *real_fft_out)+(*imag_fft_out * *imag_fft_out)))/(nsamp_sqd)));

		// Alternatives. 
		// 	Normalized???  Not sure...
//			*fft_ptr=multiplier*(noise_floor+log((((*real_fft_out * *real_fft_out)+(*imag_fft_out * *imag_fft_out)))/(nsamp_sqd)));
		//
		 	/* Phase and Real Components combined */
		*fft_ptr=multiplier*(noise_floor+(20*log10(sqrt((*real_fft_out * *real_fft_out)+(*imag_fft_out * *imag_fft_out)))));
		
		//	Real Components ONLY 
	 	//*fft_ptr=multiplier*(noise_floor+(20*log10(sqrt((*real_fft_out * *real_fft_out)))));
		 /* Phase Components ONLY  */

 	 	// *fft_ptr=multiplier*(noise_floor+(20*log10(sqrt((*imag_fft_out * *imag_fft_out)))));

		if (*fft_ptr < 0) 
			*fft_ptr=0;
		fft_ptr++;
		real_fft_out++;
		imag_fft_out--;
	}

}

int audio_chewer(void)
{
	split_and_decimate();
	if (mode != SCOPE)
		run_fft();
	
	return TRUE;
}

void split_and_decimate()
{
	/* De-interleaves data into seperate buffers and decimates 
	 * if requested 
	 */
	gint start_offset = 0;
	gint end_offset = 0;
	gint count = 0;
	gint j = 0;
	gint virtual_centerpoint = 0;
	gint index = 0;
	gint endpoint_1 = 0;
	gint endpoint_2 = 0;
	gint looparound = 0;
	gshort *audio_left_ptr = NULL;
	gshort *audio_right_ptr = NULL;
	gdouble *data_win_ptr = NULL;
	gdouble *raw_fft_in_ptr = NULL;
	gfloat cur_time=0;
	gfloat audio_offset_lag=0;
	gint audio_offset_delay=0;
	gint index2 = 0;
	gint wrap = 0;
	gint length = 0;
	gint tmp = 0;


	/* REWRITE IN PROGRESS 04/14/03
	 *
	 * Conditions:
	 * 1. Normal FFT
	 *    - Center of buffer is the "center of the fft" data at the ends 
	 *    may not necessarily be used, adjust accordingly  designed to be 
	 *    a scalable solution.
	 * 3. Decimated time domain data->fft.
	 *    - Decimate this buffer, remembering middle of buf is mid of 
	 *    display.  Skip every other byte, to decimate by 2 and so on.
	 * 4. Scope.
	 *    - Center of buffer is NOT the center of the scope,  it is offset
	 *    by an amount that is dependant on the currently selected FFT
	 *    size. (THIS SHOULD BE CHANGED EVENTUALLY).
	 *    will need to handle this condition.  because of the huge buffer
	 *    scope semi-decimation can be used for the convolve to get sync
	 *    that is stable to sub 30 hertz ranges without having to use a 
	 *    large convolution function.
	 */

	/* determine how much to offset our virtual centerpoint from the 
	 * where the input routine last put data into the buffer
	 */

	/* Lag is in milliseconds from the options panel.
	 * fft_lag is a delay based on fft size, as the fft seems to be
	 * in best visual sync when delaying by 1/2 it's length in 
	 * "sample time". It's computed as follows:
	 *      init.c: fft_lag = 1000*((nsamp/2)/(float)RATE);
	 * so for a 4096 point fft, this adds  another 46.44 milliseconds.
	 */
	delay = (int)(((float)(fft_lag+lag)/1000.0)*(float)RATE);

	/* Set pointer position to be offset from the reader pointer by
	 * amount specified by "fft_lag" (user adjustable). fft_lag is in 
	 * milliseconds, and is converted to samples above (see delay)
	 */
	draw_win_time_last = draw_win_time;
	gettimeofday(&draw_win_time, NULL); 

	/* cur_time is the absolute time NOW when this function runs.  Its 
	 * needed to calculate how much time has past since the last chunk
	 * of audio came in. (Most usefull with LARGE ft sizes (8192 points 
	 * or more..)
	 */
	cur_time = draw_win_time.tv_sec\
		+((double)draw_win_time.tv_usec/1000000.0);

	/* audio_offset_lag is the time difference between when this 
	 * function runs since that last audio block was committed to 
	 * the ringbuffer. 
	 */
	audio_offset_lag = ((draw_win_time.tv_sec \
				+(draw_win_time.tv_usec/1000000.0))\
			-(audio_arrival.tv_sec\
				+(audio_arrival.tv_usec/1000000.0)))*1000;


	/* Need this in sample elements not in milliseconds.... */
	audio_offset_delay = (int)(((float)(audio_offset_lag)/1000.0)\
			*(float)RATE);

	
	/* Set pointer to be offset from the beginning of the ring + the 
	 * position of the audio reader thread + the buffer size - the 
	 * time delay (lag compensation).
	 */

	/* printf("Total Delay factor is %i\n",delay+audio_offset_delay);*/

	/* Must add "BUFFER" to the ring value to make sure that raw_ptr
	 * OVERFLOWS, otherwise it never gets to the end and wraps. 
	 * Function below takes care of overflow and moves to the right spot.
	 */

	raw_ptr = audio_ring+ring_pos-(delay-audio_offset_delay)*2;
	/* raw_ptr now represents the "center" of the fft that we want to 
	 * run.  We will now copy this buffer int oanother shifting things
	 * as necessary to  get raw_ptr to be exactly at the midpoint of th
	 * buffer,  makes decimation calcs and scope stuff far easier.
	 */

	/* If hte pointer is out of bounds, i.e. below audio_ring, or after
	 * audio_ring+ring_end, shift by one buffer length.
	 */
	while (raw_ptr < audio_ring)
	{
		raw_ptr += BUFFER;
	}
	while (raw_ptr > (audio_ring+ring_end))
	{
		raw_ptr -=BUFFER;
	}
	/* convert to real number offset from "0" (beginning of buffer)
	 * instead of a pointer address, (easier to deal with)
	 */
	virtual_centerpoint = raw_ptr-audio_ring; 

	if (decimation_factor < 1)
		decimation_factor = 1;
	if ((decimation_factor > 0) && (decimation_factor <= 16))
	{
		start_offset = virtual_centerpoint - (nsamp*decimation_factor);
		end_offset = virtual_centerpoint + (nsamp*decimation_factor);
	}
	/* Handle the condition of reverse loop around */
	while (start_offset < 0)	
	{
		start_offset += BUFFER;
	}
	/* handle condtion of endpoint being past end of buffer, loop around */
	while (end_offset > ring_end)	
	{
		end_offset -= BUFFER;
	}
	data_win_ptr = datawindow;
	raw_fft_in_ptr = raw_fft_in;
	audio_left_ptr = audio_left;
	audio_right_ptr = audio_right;

	/* EASIEST case, no loop handling necessary */
	if (start_offset < end_offset)
	{
		index = start_offset;	/* Start at the right location */
		endpoint_1 = end_offset;
		looparound = 0;		/* don't need to loop around */
	}
	else
	{	/* we need to loop around, handle it properly */
		index = start_offset;
		endpoint_1 = ring_end;
		/* since we may end short of the end of the buffer, we
		 * need to offset properly after the wraparound, otherwaise
		 * we'll get one too many samples, and segfault. 
		 */
		if ((endpoint_1-start_offset)%(2*decimation_factor))
		{
			index2 = (2*decimation_factor)\
				- (endpoint_1-start_offset)%(2*decimation_factor);
		}
		endpoint_2 = end_offset;
		looparound = 1;
	}
	/* copy to buffers section 
	 * we find our position in the ring to copy from above, and 
	 * copy the data to the audio_left and right buffers as requested
	 * eventualy this code will go away and we'll just pass the necessary
	 * offsets to the scope code and drop the buffer altogether for 
	 * pure speed, and allows us to use a convolution on any size
	 * of datablocks, unless latency is set extremely low
	 * we increment our index by "2*decimation_factor",  because the 
	 * data is interleaved, the 2 makes sure we don't swap channels 
	 * by accident and the decimation_factor acts to essentially change 
	 * the scope's effective sweeep rate, even though it updates at a 
	 * constant speed. (decimation acts like resampling)
	 */

	if (mode == SCOPE) /* copy data to scope buffers */
	{
		while (index < endpoint_1)
		{
			/* for scope left channel */
			*audio_left_ptr=((short)*(audio_ring\
						+index));
			audio_left_ptr++;

			/* for scope right channel */
			*audio_right_ptr=((short)*(audio_ring\
						+index+1));
			audio_right_ptr++;
			index += 2*decimation_factor; 
			count++;
		}
		if (looparound)
		{	
			while (index2 < endpoint_2)
			{
				/* for scope left channel */
				*audio_left_ptr=((short)*(audio_ring\
							+index2));
				audio_left_ptr++;

				/* for scope right channel */
				*audio_right_ptr=((short)*(audio_ring\
							+index2+1));
				audio_right_ptr++;
				index2 += 2*decimation_factor; 
				count++;
			}
		}
	}
	else if (mode != STARS)	/* All FFT modes */
	{
		switch ((FftDataPacking)fft_signal_source)
		{
			case LEFT_MINUS_RIGHT:
				while (index < endpoint_1)
				{
					*raw_fft_in_ptr=(double)(*data_win_ptr)*(((double)*(audio_ring+index) - (double)*(audio_ring+index + 1))/2.0);
					data_win_ptr++;
					raw_fft_in_ptr++;
					index += 2*decimation_factor; 
					count++;

				}
				if (looparound)
				{	
					while (index2 < endpoint_2)
					{
						*raw_fft_in_ptr=(double)(*data_win_ptr)*(((double)*(audio_ring+index2) - (double)*(audio_ring+index2 + 1))/2.0);
						data_win_ptr++;
						raw_fft_in_ptr++;
						index2 += 2*decimation_factor; 
						count++;

					}
				}

				break;
			case LEFT_PLUS_RIGHT:
				while (index < endpoint_1)
				{
					*raw_fft_in_ptr=(double)(*data_win_ptr)*(((double)*(audio_ring+index) + (double)*(audio_ring+index + 1))/2.0);
					data_win_ptr++;
					raw_fft_in_ptr++;
					index += 2*decimation_factor;
					count++;

				}
				if (looparound)
				{
					while (index2 < endpoint_2)
					{
						*raw_fft_in_ptr=(double)(*data_win_ptr)*(((double)*(audio_ring+index2) + (double)*(audio_ring+index2 + 1))/2.0);
						data_win_ptr++;
						raw_fft_in_ptr++;
						index2 += 2*decimation_factor;
						count++;
					}
				}
				break;
			case LEFT:
				while (index < endpoint_1)
				{
					*raw_fft_in_ptr=(*data_win_ptr)\
						* *(audio_ring+index);
					data_win_ptr++;
					raw_fft_in_ptr++;
					index += 2*decimation_factor;
					count++;
				}
				if (looparound)
				{
					while(index2 < endpoint_2)
					{
						*raw_fft_in_ptr=(*data_win_ptr)\
							* *(audio_ring+index2);
						data_win_ptr++;
						raw_fft_in_ptr++;
						index2 += 2*decimation_factor;
						count++;
					}
				}
				break;
			case RIGHT:
				while (index < endpoint_1)
				{
					*raw_fft_in_ptr=(*data_win_ptr)\
						* *(audio_ring+index+1);
					data_win_ptr++;
					raw_fft_in_ptr++;
					index += 2*decimation_factor;
					count++;
				}
				if (looparound)
				{
					while (index2 < endpoint_2)
					{
						*raw_fft_in_ptr=(*data_win_ptr)\
							* *(audio_ring+index2+1);
						data_win_ptr++;
						raw_fft_in_ptr++;
						index2 += 2*decimation_factor;
						count++;
					}
				}
				break;
			default:
				fprintf(stderr,__FILE__":  This shouldn't happen!!!,"
					" fft_signal_source is NOT set, BUG DETECTED, contact author with this information\n");
				break;
		}
	}
#ifdef DEBUG
	if (count != nsamp)
	{	
		printf("buffer overrun, original startpoint %i, endpoint_1: %i, endpoint_2:%i\n",start_offset,endpoint_1,endpoint_2);
		exit (-1);
	}
#endif
	/* Buffer Latency Monitor update */

	if (gdk_window_is_visible(buffer_area->window))
	{
		tmp = (float)buffer_area->allocation.width\
			*((float)start_offset/(float)ring_end);
		if (end_offset < start_offset)	// wrap condition
		{
			length = buffer_area->allocation.width-tmp;
			wrap = 1;
		}
		else
		{
			length = (float)buffer_area->allocation.width\
				*((float)end_offset/(float)ring_end)-tmp;
			wrap = 0;	
		}
		// Only draw it if its visible.  Why waste CPU time ??? 
		gdk_threads_enter();

		gdk_draw_rectangle(buffer_pixmap,buffer_area->style->black_gc,\
				TRUE,\
				0,\
				65,\
				buffer_area->allocation.width,\
				16);

		gdk_draw_rectangle(buffer_pixmap,latency_monitor_gc,\
				FALSE,\
				tmp,\
				65,\
				length,\
				15);
		if (wrap)
		{
			gdk_draw_rectangle(buffer_pixmap,latency_monitor_gc,\
					FALSE,\
					0,\
					65,\
					(float)buffer_area->allocation.width\
					*((float)end_offset/(float)ring_end),\
					15);
		}

		last_start = (float)buffer_area->allocation.width\
			*((float)start_offset/(float)ring_end);
		last_end = (float)buffer_area->allocation.width\
			*((float)end_offset/(float)ring_end);

		gdk_window_clear(buffer_area->window);
		gdk_threads_leave();
	}


	/* printf("Number of samples put into fft_in buffer:%i\n",count) */;

	/* Only run stabilizer code if we have 2048 samples or MORE,
	 * otherwise the display gets truncated, and its impossible to
	 * have a display thats GURRANTEED to ALWAYS contain nsamp/2 points
	 * in it due to the convolve, so we force it to be off in that case
	 * so the display isn't cut in half. You DON't get any stabilization
	 * in that case, but I traded off the greater data for less stability
	 * in the lower number of samples.
	 *
	 * We could get around this when the buffer is large and the lag 
	 * is sufficient enough to allow us to use some of the data that's
	 * "not ready" yet. (buffered before display) and to also use some of 
	 * the data from the last run to extend the display window. Maybe I'll
	 * add that code to see if its doable without causing too much 
	 * difficulty, thus allowing the convolve down to small window sizes.
	 * AND allowing larger convolve window size. The convolve window is 
	 * a PORTION of the sample set that is used to sync the display.
	 * the larger the convolve set, the lower a frequency the display will
	 * lock onto and stay stable. (check and see by changing the number of 
	 * samples in scope mode, when you're watching a very low freq tone
	 * 100 hz or lower)
	 */
	if ((mode == SCOPE) && (nsamp >=2048) && (stabilized)) 
	{
		/* last buf, is the last 256 point array sent thru the 
		 * convolve cur_buff is a 512 point segment of the current 
		 * buffer. (In MOST cases the current buffer should be 
		 * much larger than 512 bytes.) We just use 256 or them 
		 * to get a lockup from the last run to help stabilize 
		 * the display.  It works similar to "triggering" on a
		 * conventional scope, only better.. :) as it functions 
		 * much more like a good pattern matcher.  only with 
		 * a slight phase shift effect when near its limits. 
		 * (low frequencies)  feed a decreasing freq tone into 
		 * eXtace and when the convolve is nearing its limits the 
		 * display will slide about back and forth, until it is 
		 * unstable. A larger convolve factor helps it go lower 
		 * without using a larger number of samples.  It can 
		 * only go so big before you jump the bounds
		 * of the buffer and fuck something up.. :)
		 */

		for (j=0;j<CONVOLVE_BIG;j++)
		{
			cur_buf_l[j] = audio_left[convolve_factor*j]; 
			cur_buf_r[j] = audio_right[convolve_factor*j];
		}
		if (sync_to_left || sync_independant)
			scope_begin_l = convolve_factor*convolve_match(last_buf_l, cur_buf_l, l_state);
		if (sync_to_right || sync_independant)
			scope_begin_r = convolve_factor*convolve_match(last_buf_r, cur_buf_r, r_state);
		//	printf("begin left\t%i,\tbegin_right\t%i\n",scope_begin_l,scope_begin_r);
	}

}
