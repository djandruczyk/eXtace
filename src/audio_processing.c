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

#include <config.h>
#include <globals.h>
#include <protos.h>
#include <math.h>
#include <gtk/gtk.h>
#include <asm/errno.h>
#include "convolve.h"
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
static gint last;

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
		//	*fft_ptr=multiplier*(noise_floor+log((((*real_fft_out * *real_fft_out)+(*imag_fft_out * *imag_fft_out)))/(nsamp_sqd)));
		//
		 	/* Phase and Real Components combined */
		*fft_ptr=multiplier*(noise_floor+(20*log10(sqrt((*real_fft_out * *real_fft_out)+(*imag_fft_out * *imag_fft_out)))));
		
		//	Real Components ONLY 
	 	//	fft_ptr=multiplier*(noise_floor+(20*log10(sqrt((*real_fft_out * *real_fft_out)))));
		 /* Phase Components ONLY  */

 	 	/*fft_ptr=multiplier*(noise_floor+(20*log10(sqrt((*imag_fft_out * *imag_fft_out)))));*/

		if (*fft_ptr < 0) 
			*fft_ptr=0;
		fft_ptr++;
		real_fft_out++;
		imag_fft_out--;
	}

}

int audio_chewer(void)
{
	copy_to_centered_buffer();
	split_and_decimate();
	if (mode != SCOPE)
		run_fft();
	
	return TRUE;
}

void split_and_decimate()
{
	/* De-interleaves data into seperate buffers and decimates 
	 * if requested for high-res fft's
	 */
	gint centerpoint = centered_buffer_end/2;
	gint start_offset = 0;
	gint end_offset = 0;
	gint count = 0;
	gint j = 0;
	gint increment = 0;
	gint index = 0;
	gshort *audio_left_ptr = NULL;
	gshort *audio_right_ptr = NULL;

	gdouble *data_win_ptr = NULL;
	gdouble *raw_fft_in_ptr = NULL;

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
	 *    - Center of buffer is NOT the cneter of the scope,  it is offset
	 *    by an amount that is dependant on the currently selected FFT
	 *    size. (THIS SHOULD BE CHANGED EVENTUALLY).
	 *    will need to handle this condition.  because of the huge buffer
	 *    scope semi-decimation can be used for the convolve to get sync
	 *    that is stable to sub 30 hertz ranges without having to use a 
	 *    large convolution function.
	 */


	switch (decimation_factor)
	{
		case NO_DECIMATION:
			/* No decimation at all */
//			printf("Decimate by 1\n");
			start_offset = centerpoint - nsamp;
			end_offset = centerpoint + nsamp;
			increment = 1;
			break;
		case DECIMATE_BY_2:
			/* decimate by factor of 2 */
//			printf("Decimate by 2\n");
			start_offset = centerpoint - (nsamp*2);
			end_offset = centerpoint + (nsamp*2);
			increment = 2;
			break;
		case DECIMATE_BY_3:
			/* decimate by factor of 3 */
//			printf("Decimate by 3\n");
			start_offset = centerpoint - (nsamp*3);
			end_offset = centerpoint + (nsamp*3);
			increment = 3;
			break;
		case DECIMATE_BY_4:
			/* decimate by factor of 4 */
//			printf("Decimate by 4\n");
			start_offset = centerpoint - (nsamp*4);
			end_offset = centerpoint + (nsamp*4);
			increment = 4;
			break;
		case DECIMATE_BY_5:
			/* decimate by factor of 5 */
//			printf("Decimate by 5\n");
			start_offset = centerpoint - (nsamp*5);
			end_offset = centerpoint + (nsamp*5);
			increment = 5;
			break;
		default:
			/*no decimate if code error */
//			printf("decimation_factor not set defaulting to 1\n");
			start_offset = centerpoint - nsamp;
			end_offset = centerpoint + nsamp;
			increment = 1;
			break;

	}
//	printf("Start_offset: %i, End offset: %i\n",start_offset,end_offset);

	data_win_ptr = datawindow;
	raw_fft_in_ptr = raw_fft_in;
	audio_left_ptr = audio_left;
	audio_right_ptr = audio_right;
	index = start_offset;  /* Start at the right location */

//#error THIS DOES NOT FUNCTION YET FIXME

	switch (fft_signal_source)
	{
		case DIFFERENCE:
			while (index < end_offset)
			{
				*raw_fft_in_ptr=(double)(*data_win_ptr)*(((double)*(centered_buffer+index) - (double)*(centered_buffer+index + 1))/2.0);
				data_win_ptr++;
				raw_fft_in_ptr++;

				/* for scope left channel */
				*audio_left_ptr=((short)*(centered_buffer\
						+index));
				audio_left_ptr++;

				/* for scope right channel */
				*audio_right_ptr=((short)*(centered_buffer\
						+index+1));
				audio_right_ptr++;
				index += 2*increment; 
				count++;

			}

			break;
		case COMPOSITE:
			while (index < end_offset)
			{
				*raw_fft_in_ptr=(double)(*data_win_ptr)*(((double)*(centered_buffer+index) + (double)*(centered_buffer+index + 1))/2.0);
				data_win_ptr++;
				raw_fft_in_ptr++;

				/* for scope left channel */
				*audio_left_ptr=((short)*(centered_buffer\
						+index));
				audio_left_ptr++;

				/* for scope right channel */
				*audio_right_ptr=((short)*(centered_buffer\
						+index+1));
				audio_right_ptr++;
				index += 2*increment;
				count++;

			}
			break;
		case LEFT:
			while (index < end_offset)
			{
				*raw_fft_in_ptr=(*data_win_ptr)\
						* *(centered_buffer+index);
				data_win_ptr++;
				raw_fft_in_ptr++;

				/* for scope left channel */
				*audio_left_ptr=((short)*(centered_buffer\
						+index));
				audio_left_ptr++;

				/* for scope right channel */
				*audio_right_ptr=((short)*(centered_buffer\
						+index+1));
				audio_right_ptr++;
				index += 2*increment;
				count++;
			}
			break;
		case RIGHT:
			while (index < end_offset)
			{
				*raw_fft_in_ptr=(*data_win_ptr)\
						* *(centered_buffer+index+1);
				data_win_ptr++;
				raw_fft_in_ptr++;

				/* for scope left channel */
				*audio_left_ptr=((short)*(centered_buffer\
						+index));
				audio_left_ptr++;

				/* for scope right channel */
				*audio_right_ptr=((short)*(centered_buffer\
						+index+1));
				audio_right_ptr++;
				index += 2*increment;
				count++;
			}
			break;
		default:
			printf("This shouldn't happen!!!, fft_signal_source is NOT set, BUG DETECTED, contact author with this information\n");
			break;
	}

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

void copy_to_centered_buffer()
{
	gfloat cur_time=0;
	gfloat audio_offset_lag=0;
	gint audio_offset_delay=0;
	/* Lag is in milliseconds from the options panel.
	 * fft_lag is a delay based on fft size, as the fft seems to be
	 * in best visual sync when delaying by 1/2 it's length in 
	 * "sample time". It's computed as follows:
	 * 	init.c: fft_lag = 1000*((nsamp/2)/(float)RATE);
	 * so for a 4096 point fft, this adds  another 46.43 milliseconds.
	 */
	delay = (int)(((float)(fft_lag+lag)/1000.0)*(float)RATE);

	/* Actually the lag is the same for all displays which is NON-optimal
	 * as the FFT's give best visual/audio sync when the audio delay points
	 * to the MIDDLE of the fft window. (point 2048 in a 4096 pt fft).
	 * On large FFT sizes, this lag is considerable (over 50-100 msecs)
	 * which makes the scope look out of sync.
	 * Sooner or later I'll rewrite this section to get around that..
	 */
	//    printf("Current FFT lag is %i ms\n",fft_lag+lag);
	//    printf("Current SCOPE lag is %i ms\n",lag);

	//

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
	cur_time = draw_win_time.tv_sec + ((double)draw_win_time.tv_usec/1000000.0);
	/* audio_offset_lag is the time difference between when this 
	 * function runs since that last audio block was committed to 
	 * the ringbuffer. 
	 */
	audio_offset_lag = ((draw_win_time.tv_sec + (draw_win_time.tv_usec/1000000.0)) - (audio_arrival.tv_sec + (audio_arrival.tv_usec/1000000.0)))*1000;
	//printf("Audio offset lag in milliseconds %f\n",audio_offset_lag);

	/* Need this in sample elements not in milliseconds.... */
	audio_offset_delay = (int)(((float)(audio_offset_lag)/1000.0)\
			*(float)RATE);

	/* Set pointer to be offset from the beginning of the ring + the 
	 * position of the audio reader thread + the buffer size - the 
	 * time delay (lag compensation).
	 */

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

	while (raw_ptr < audio_ring)
	{
//		printf("Warning raw_pointer pointing outside (before) buffer\n");
		raw_ptr += BUFFER;
	}
	while (raw_ptr > (audio_ring+ring_end))
	{
		raw_ptr -=BUFFER;
	}

	if (raw_ptr > (audio_ring + (BUFFER/2)))
	{
		/* raw_ptr is in the second half of the buffer */
		/* third arg is bytes thus the lack of a "/2" */
//		printf("raw_ptr past halfway point \n");
//		printf("RAW ptr's position relative to audio_ring %i, endpt %li\n",raw_ptr-audio_ring, ring_end);
//		printf("Copying %i bytes from audio_ring+raw_ptr-%i to 0, part 1\n",BUFFER,BUFFER/2);
		memcpy(centered_buffer,raw_ptr-(BUFFER/2),BUFFER);
//		printf("Copying %li bytes from raw_ptr (%p) to %i, part 2\n",(ring_end-(raw_ptr-audio_ring))*2, raw_ptr, BUFFER/2);
		memcpy(centered_buffer+BUFFER/2,raw_ptr,(ring_end-(raw_ptr-audio_ring))*2);
//		printf("Copying %i bytes, part 3 to %i\n",(raw_ptr-audio_ring-(BUFFER/2))*2, BUFFER/2+(ring_end-(raw_ptr-audio_ring)));
		memcpy(centered_buffer+(BUFFER/2)+(ring_end-(raw_ptr-audio_ring)),audio_ring,(raw_ptr-audio_ring-(BUFFER/2))*2);
	}	
	else
	{
//		printf("raw_ptr before halfway point\n");
//		printf("Copying %i bytes to %i, part 1\n",BUFFER,BUFFER/2);
		memcpy(centered_buffer+(BUFFER/2),raw_ptr,BUFFER);
//		printf("Copying %li bytes to 0, part 2\n",(ring_end-(raw_ptr-audio_ring)-(BUFFER/2))*2);
		memcpy(centered_buffer,raw_ptr+(BUFFER/2),(ring_end-(raw_ptr-audio_ring)-(BUFFER/2))*2);
//		printf("Copying %i bytes to %li, part 3\n",(raw_ptr-audio_ring)*2,(ring_end-(raw_ptr-audio_ring)-(BUFFER/2)));
		memcpy(centered_buffer+(ring_end-(raw_ptr-audio_ring)-(BUFFER/2)),audio_ring,(raw_ptr-audio_ring)*2);
	}

	if (gdk_window_is_visible(buffer_area->window))
	{
		// Only draw it if its visible.  Why waste CPU time ??? 
		gdk_threads_enter();

		gdk_draw_rectangle(buffer_pixmap,buffer_area->style->black_gc,
				TRUE,
				last, 65,
				2,15);

		gdk_draw_rectangle(buffer_pixmap,latency_monitor_gc,
				TRUE,
				(float)buffer_area->allocation.width\
				*((float)(raw_ptr-audio_ring)/(float)ring_end), 65,
				2,15);

		last = (float)buffer_area->allocation.width\
			*((float)(raw_ptr-audio_ring)/(float)ring_end);

		gdk_window_clear(buffer_area->window);
		gdk_threads_leave();
	}

	return ;
}

