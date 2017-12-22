/*
 * input_processing.c extace source file
 * 
 * Audio visualization
 * 
 * Copyright (C) 1999-2017 by Dave J. Andruczyk 
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

#include <errno.h>
#include <input_processing.h>
#include <config.h>
#include <convolve.h>
#include <enums.h>
#include <input.h>
#include <globals.h>
#include <gtk/gtk.h>
#include <math.h>
#ifdef USING_FFTW2
#include <fftw.h>
#endif 
#ifdef USING_FFTW3
#include <fftw3.h>
#endif 

#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif


static gint delay;
static gint last_start;
static gint last_end;
gint  scope_sync_source;

void run_fft(void)
{
	int nsamp_sqd = nsamp*nsamp;
	gdouble *fft_ptr=NULL;
	gdouble *real_fft_out=NULL;
	gdouble *imag_fft_out=NULL;
	gint index1=0;


#ifdef USING_FFTW2
	rfftw_one(plan, raw_fft_in, raw_fft_out);
#elif USING_FFTW3
	fftw_execute(plan);
#endif

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

#if 0
		/* Power Spectrum */
		*fft_ptr=multiplier*(noise_floor+(((*real_fft_out)*(*real_fft_out))+(*imag_fft_out)*(*imag_fft_out)));
#elif 0
		/* Amplitude Spectrum */
		*fft_ptr=sqrt(multiplier*(noise_floor+(((*real_fft_out)*(*real_fft_out))+(*imag_fft_out)*(*imag_fft_out))));
#elif 0
		/* Phase Spectrum */
		*fft_ptr=100*multiplier*atan2(*imag_fft_out,*real_fft_out);

#elif 0  /* 	Normalized???  Not sure... */
		*fft_ptr=multiplier*(noise_floor+log((((*real_fft_out * *real_fft_out)+(*imag_fft_out * *imag_fft_out)))/(nsamp_sqd)));
#elif 1	 /* Phase and Real Components combined */
		*fft_ptr=multiplier*(noise_floor +
				     10*log10(pow(*real_fft_out,2) 
					       + pow(*imag_fft_out,2)
					     ));
#elif 0  //	Real Components ONLY 
	 	*fft_ptr=multiplier*(noise_floor + 20*log10(fabs(*real_fft_out)));
#elif 0 		 /* Imaginary Components ONLY  */
	 	*fft_ptr=multiplier*(noise_floor + 20*log10(fabs(*real_fft_out)));
#endif
		if (*fft_ptr < 0) 
			*fft_ptr=0;
		fft_ptr++;
		real_fft_out++;
		imag_fft_out--;
	}
	
}

int input_chewer(void)
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
	gint start_offset=0;
	gint end_offset=0;
	gint count;
	gint j = 0;
	gint virtual_centerpoint;
	gint i,k;
//	gfloat cur_time=0;
	gfloat input_offset_lag=0;
	gint input_offset_delay=0;
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

	/* 
	   Delay in number of samples.
	   Lag is in milliseconds from the options panel.
	   Need to add another nsamp/2 so that we don't read
	   past the newest points in the ring buffer.
	 */
	delay = nsamp/2+ring_rate*((float) lag)/1000.0;
//	printf("delay %i samples, ring_rate %f(hz.), lag %d(ms.)\n",delay,ring_rate,lag);

	draw_win_time_last = draw_win_time;
	gettimeofday(&draw_win_time, NULL); 

	/* cur_time is the absolute time NOW when this function runs.  Its 
	 * needed to calculate how much time has past since the last chunk
	 * of audio came in. (Most usefull with LARGE fft sizes (8192 points 
	 * or more..)
	 */
	//cur_time = draw_win_time.tv_sec
	//	+((double)draw_win_time.tv_usec/1000000.0);

	/* inpu_offset_lag is the time difference between when this 
	 * function runs since that last audio block was committed to 
	 * the ringbuffer. 
	 */
	input_offset_lag = ((draw_win_time.tv_sec 
				+(draw_win_time.tv_usec/1000000.0))
			-(input_arrival.tv_sec
				+(input_arrival.tv_usec/1000000.0)))*1000;

	/* Need this in samples not in milliseconds.... */
	input_offset_delay = ring_rate*((float) input_offset_lag)/1000.0;

	/* 
	   virtual_centerpoint represents the "center" of the fft 
	   that we want to run.

	   Set pointer to be offset from the beginning of the ring + the 
	   position of the audio reader thread + the buffer size - the 
	   time delay (lag compensation).  
	 */

	virtual_centerpoint = ring_pos-(delay-input_offset_delay)*ring_channels;
	/* If the pointer is out of bounds, i.e. below ringbuffer, or after
	 * ringbuffer+ring_end, shift by one buffer length.
	 */
	while (virtual_centerpoint < 0)
		virtual_centerpoint += ring_end;
	while (virtual_centerpoint > ring_end)
		virtual_centerpoint -= ring_end;

	/* this assumes ring_end is a multiple of ring_channels */
	virtual_centerpoint -= virtual_centerpoint%ring_channels;

#if 0  /* debug print */
	printf("delay=%i, input_offset_delay=%i,"
			" ring position %i centerpoint=%i\n",
			delay,input_offset_delay,ring_pos,
			virtual_centerpoint);  
#endif

	if (decimation_factor < 1)
		decimation_factor = 1;
	if ((decimation_factor > 0) && (decimation_factor <= 16))
	{
		/* shift must be multiple of number of channels */
		start_offset = virtual_centerpoint - 
			ring_channels*(nsamp*decimation_factor/2);
		end_offset = virtual_centerpoint + 
			ring_channels*(nsamp*decimation_factor/2);
	} 
	else
		fprintf(stderr,__FILE__":  invalid decimation_factor=%i\n",
				decimation_factor);

	/* Handle the condition of reverse loop around */
	while (start_offset < 0)	
		start_offset += ring_end;

	/* handle condtion of endpoint being past end of buffer, loop around */
	while (end_offset > ring_end)	
		end_offset -= ring_end;

//	printf("start_offset is %i, end_offset is %i, len= %i\n",start_offset, end_offset, end_offset-start_offset);
	/* copy to buffers section 
	 * we find our position in the ring to copy from above, and 
	 * copy the data to the audio_left and right buffers as requested
	 * eventualy this code will go away and we'll just pass the necessary
	 * offsets to the scope code and drop the buffer altogether for 
	 * pure speed, and allows us to use a convolution on any size
	 * of datablocks, unless latency is set extremely low
	 * we increment our index by "ring_channels*decimation_factor",  
	 * because the data is interleaved, the ring_channels makes sure we 
	 * don't swap channels by accident and the decimation_factor acts 
	 * to essentially change the scope's effective sweeep rate, even 
	 * though it updates at a constant speed. (decimation acts like 
	 * resampling)
	 */

	switch(mode){
		case SCOPE:
			for(i=start_offset, count=0; count<nsamp; count++)
			{
				for(k=0; k<ring_channels; k++)
				{
					/* This will change if we generalize 
					   number of scope channels */
					/* for scope left channel */
					if(k==0)
						audio_left[count] = INPUT_RING(i+k);

					/* for scope right channel */
					if(k==1)
						audio_right[count] = INPUT_RING(i+k);
				}
				i += ring_channels*decimation_factor;
				while(i >= ring_end)
					i -= ring_end;
			}
			break;
		case STARS:
			break;
		default:
			switch ((FftDataPacking)fft_signal_source)
			{
				case LEFT_MINUS_RIGHT:
					if(ring_channels < 2)break;  /* Fix! error condition */
					for(count=0, i=start_offset; count<nsamp; count++)
					{
						raw_fft_in[count] = datawindow[count]*
							(INPUT_RING(i) - INPUT_RING(i+1))/2.0;
						i += ring_channels*decimation_factor; 
						while(i >= ring_end) i -= ring_end;
					}
					break;
				case LEFT_PLUS_RIGHT:
					if(ring_channels<2)break;  /* Fix! error condition */
					for(count=0, i=start_offset; count<nsamp; count++)
					{
						/* no need for (double) type cast */
						raw_fft_in[count] = datawindow[count]*
						  (INPUT_RING(i) + INPUT_RING(i+1))/2.0;
						i += ring_channels*decimation_factor; 
						while(i >= ring_end) 
							i -= ring_end;
					}
					break;
				case LEFT:
					if(ring_channels<1)break;  /* Fix! error condition */
					for(count=0, i=start_offset; count<nsamp; count++)
					{
						raw_fft_in[count] = datawindow[count]*
							INPUT_RING(i);
						i += ring_channels*decimation_factor; 
						while(i >= ring_end)
							i -= ring_end;
					}
					break;
				case RIGHT:
					if(ring_channels < 2)break;  /* Fix! error condition */
					for(count=0, i=start_offset; count<nsamp; count++)
					{
						raw_fft_in[count] = datawindow[count]*
							INPUT_RING(i+1);
						i += ring_channels*decimation_factor; 
						while(i >= ring_end)
							i -= ring_end;
					}
					break;
				default:
					fprintf(stderr,__FILE__":  This shouldn't happen!!!,"
							" fft_signal_source is NOT set, BUG DETECTED, contact author with this information\n");
					break;
			}

	}

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
		switch ((ScopeSyncSource)scope_sync_source)
		{
			case SYNC_LEFT:
				scope_begin_l = convolve_factor*convolve_match(last_buf_l, cur_buf_l, l_state);
				break;
			case SYNC_RIGHT:
				scope_begin_r = convolve_factor*convolve_match(last_buf_r, cur_buf_r, r_state);
				break;
			case SYNC_INDEP:
				scope_begin_l = convolve_factor*convolve_match(last_buf_l, cur_buf_l, l_state);
				scope_begin_r = convolve_factor*convolve_match(last_buf_r, cur_buf_r, r_state);
				break;
			default:
				break;
		}
#if 0
		printf("begin left\t%i,\tbegin_right\t%i\n",scope_begin_l,scope_begin_r);
#endif
	}

}
