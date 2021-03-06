/*
 * defaults.h source file for extace
 * 
 * /GDK/GNOME sound (esd) system output display program
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

#ifndef _DEFAULTS_H_
#define _DEFAULTS_H_ 1

const float	lag_min = 1.00;		/* Minimum lag allowed */
const float	noise_floor_min = -160.0;
const float	noise_floor_max = 100.0;
const float	multiplier_min = 1.0;	/* Sensivity max allowed */
const float	multiplier_max = 100.0;	/* Sensivity max allowed */
const float	bar_decay_max = 35.0;
const float	peak_hold_max = 35.0;
const float	peak_decay_max = 35.0;
const float	refresh_max = 86.0;
const float	tape_scroll_max = 12.0;

#endif
