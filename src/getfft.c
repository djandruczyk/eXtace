/*
 * getfft.c extace source file
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



int GetFFT(void)
{
	gint j=0;
	gint index1=0;
	gint index2=0;
	gint end=0;
	guint overshoot=0;
	gshort *audio_left_ptr=NULL;
	gshort *audio_right_ptr=NULL;

	gdouble *data_win_ptr=NULL;
	gdouble *audio_data_ptr=NULL;
	gdouble *real_fft_out=NULL;
	gdouble *imag_fft_out=NULL;
	gdouble *fft_ptr=NULL;
	gint nsamp_sqd=0;
	gfloat cur_time=0;
	gfloat audio_offset_lag=0;
	gint audio_offset_delay=0;
	gint delay = (int)(((float)(fft_lag+lag)/1000.0)*(float)RATE);
	static gint last;

	/* Actually the lag is the same for all displays which is NON-optimal
	 * as the FFT's give best visual/audio sync when the audio delay points
	 * to the MIDDLE of the fft window. (point 2048 in a 4096 pt fft).
	 * On large FFT sizes, this lag is considerable (over 50-100 msecs)
	 * whic makes the scope look out of sync.
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
	cur_time = draw_win_time.tv_sec + ((double)draw_win_time.tv_usec/1000000);
	/* audio_offset_lag is hte time difference between when this function runs 
	 * since that last audio block was committed to the ringbuffer.
	 */
	audio_offset_lag = ((draw_win_time.tv_sec + (double)draw_win_time.tv_usec/1000000)-(audio_arrival.tv_sec + (double)audio_arrival.tv_usec/1000000))*1000;

	/* Need this in sample elements not in milliseconds.... */
	audio_offset_delay = (int)(((float)(audio_offset_lag)/1000.0)*(float)RATE);

	//    printf("++Window DRAWER: current at %f, diff from LATEST audio %.2fms\n",cur_time,audio_offset_lag);

	/* Set pointer to be offset from the beginning of the ring + the position 
	 * of the audio reader thread + the buffer size - the time delay (lag
	 * compensation)  This really should use gettimeofday as GTK's timeout 
	 * functions are not guarranteed to go off at exactly when wanted
	 * especially on a loaded system.
	 */

	/* Must add "BUFFER" to the ring value to make sure that raw_ptr
	 * OVERFLOWS, otherwise it never gets to the end and wraps. Function below
	 * takes care of overflow and moves to the right spot.
	 */
	raw_ptr = audio_ring+ring_pos+BUFFER-(delay-audio_offset_delay)*2;

	data_win_ptr=datawindow;
	audio_data_ptr=audio_data;
	audio_left_ptr=audio_left;
	audio_right_ptr=audio_right;


	while (raw_ptr >= (audio_ring+ring_end))
	{
		//	printf("overshoot!!\n");
		overshoot = (raw_ptr - (audio_ring+ring_end));
		raw_ptr = audio_ring + overshoot;
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

	//    printf("distance from end in sample elements %i\n",(ring_end-raw_ptr)/2);
	index1 = (((audio_ring+ring_end - raw_ptr)/2) < nsamp) ? ((audio_ring+ring_end - raw_ptr)/2): nsamp ;
	if (index1 < nsamp)
	{
		/* buffer loop condition */
		index2 = nsamp - index1;
		end = 1;
		/*	printf("Buffer looping,  index1=%i, index2=%i\n",index1,index2); */
	}
	else
		end=0;
	/*    printf("begining of ring %p, raw_ptr, %p, end %p, length %i\n",audio_ring,raw_ptr,ring_end,ring_end-audio_ring);
	 *    printf("begining of ring %i, raw_ptr, %i, end %i, length %i\n",audio_ring-audio_ring,raw_ptr-audio_ring,ring_end-audio_ring,ring_end-audio_ring);
	 *    printf("index1 %i, index2 %i raw_ptr %p, endpt %p\n",index1,index2,raw_ptr,ring_end);
	 */

	switch (fft_signal_source)
	{
		case COMPOSITE:
			while (index1--)
			{
				*audio_data_ptr=((*data_win_ptr)*(double)((*raw_ptr + *(raw_ptr + 1))/2.0));
				data_win_ptr++;

				/* for scope left channel */
				*audio_left_ptr=((short)(*raw_ptr));
				audio_left_ptr++;
				raw_ptr++;

				/* for scope right channel */
				*audio_right_ptr=((short)(*raw_ptr));
				audio_right_ptr++;
				raw_ptr++;


				/* Counter to make sure loop ends (eventually... ) */
				audio_data_ptr++;

			}

			if (index2 > 0)
			{
				raw_ptr = audio_ring;
				//		printf("reset raw_ptr to beginning of ring %p\n",audio_ring);
			}
			while (index2--)
			{
				*audio_data_ptr=((*data_win_ptr)*(double)((*raw_ptr + *(raw_ptr+1))/2.0));
				data_win_ptr++;

				/* for scope left channel */
				*audio_left_ptr=((short)(*raw_ptr));
				audio_left_ptr++;
				raw_ptr++;

				/* for scope right channel */
				*audio_right_ptr=((short)(*raw_ptr));
				audio_right_ptr++;
				raw_ptr++;

				/* Counter to make sure loop ends (eventually... ) */
				audio_data_ptr++;
			}
			break;
		case LEFT:
			while (index1--)
			{
				*audio_data_ptr=((*data_win_ptr)*(double)*raw_ptr);
				data_win_ptr++;

				/* for scope left channel */
				*audio_left_ptr=((short)(*raw_ptr));
				audio_left_ptr++;
				raw_ptr++;

				/* for scope right channel */
				*audio_right_ptr=((short)(*raw_ptr));
				audio_right_ptr++;
				raw_ptr++;


				/* Counter to make sure loop ends (eventually... ) */
				audio_data_ptr++;

			}
			if (index2 > 0)
			{
				raw_ptr = audio_ring;
				//		printf("reset raw_ptr to beginning of ring %p\n",audio_ring);
			}
			while (index2--)
			{
				*audio_data_ptr=((*data_win_ptr)*(double)*raw_ptr);
				data_win_ptr++;

				/* for scope left channel */
				*audio_left_ptr=((short)(*raw_ptr));
				audio_left_ptr++;
				raw_ptr++;

				/* for scope right channel */
				*audio_right_ptr=((short)(*raw_ptr));
				audio_right_ptr++;
				raw_ptr++;

				/* Counter to make sure loop ends (eventually... ) */
				audio_data_ptr++;
			}
			break;
		case RIGHT:
			while (index1--)
			{
				*audio_data_ptr=((*data_win_ptr)*(double)*(raw_ptr+1));
				data_win_ptr++;

				/* for scope left channel */
				*audio_left_ptr=((short)(*raw_ptr));
				audio_left_ptr++;
				raw_ptr++;

				/* for scope right channel */
				*audio_right_ptr=((short)(*raw_ptr));
				audio_right_ptr++;
				raw_ptr++;


				/* Counter to make sure loop ends (eventually... ) */
				audio_data_ptr++;

			}
			if (index2 > 0)
			{
				raw_ptr = audio_ring;
				//		printf("reset raw_ptr to beginning of ring %p\n",audio_ring);
			}
			while (index2--)
			{
				*audio_data_ptr=((*data_win_ptr)*(double)*(raw_ptr+1));
				data_win_ptr++;

				/* for scope left channel */
				*audio_left_ptr=((short)(*raw_ptr));
				audio_left_ptr++;
				raw_ptr++;

				/* for scope right channel */
				*audio_right_ptr=((short)(*raw_ptr));
				audio_right_ptr++;
				raw_ptr++;

				/* Counter to make sure loop ends (eventually... ) */
				audio_data_ptr++;
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
		/* last buf, is the last 256 point array sent thru the convolve
		 * cur_buff is a 512 point segment of the current buffer. (In MOST
		 * cases the current buffer should be much larger than 512 bytes.)
		 * We just use 256 or them to get a lockup from the last run to help 
		 * stabilize the display.  It works similar to "triggering" on a 
		 * conventional scope, only better.. :) as it functions much more 
		 * like a good pattern matcher.  only with a phase shift effect
		 * when near its limits. (low frequencies)  feed a decreasing freq
		 * tone into eXtace and when the convolve is nearing its limits the 
		 * display will slide about back and forth, until it is unstable.
		 * A larger convolve factor helps it go lower without using a larger
		 * number of samples.  It can only go so big before you jump the bounds
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
		return TRUE;
	}

	nsamp_sqd = nsamp*nsamp;
	rfftw_one(plan, audio_data, raw_fft_out);
	norm_fft[0] = (raw_fft_out[0]*raw_fft_out[0])/nsamp_sqd;
	norm_fft[nsamp/2] = (raw_fft_out[nsamp/2]*raw_fft_out[nsamp/2])/nsamp_sqd;

	/* This pointer will be INCREMENTED below... */
	real_fft_out=raw_fft_out; /* Beginning of buffer*/

	/* This pointer will be DECREMENTED below... */
	imag_fft_out=raw_fft_out+nsamp-1; /* End of buffer, the "mirrored side" */

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
		// 	Phase and Real Components combined
		*fft_ptr=multiplier*(noise_floor+(20*log10(sqrt((*real_fft_out * *real_fft_out)+(*imag_fft_out * *imag_fft_out)))));
		//
		// 	 Real Components ONLY
		// 	fft_ptr=multiplier*(noise_floor+(20*log10(sqrt((*real_fft_out * *real_fft_out)))));
		/* Phase Components ONLY */

		// 	*fft_ptr=multiplier*(noise_floor+(20*log10(sqrt((*imag_fft_out * *imag_fft_out)))));
		if (*fft_ptr < 0) 
			*fft_ptr=0;
		fft_ptr++;
		real_fft_out++;
		imag_fft_out--;
	}

	return TRUE;
}

