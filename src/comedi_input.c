/*
 *
 * comedi_input.c extace source file
 * This are routines needed for manipulating, saving, and
 * loading comedi_cmd structures.
 * 
 * Copyright (C) 1999 by Dave J. Andruczyk 
 * 
 * Based on the original extace written by The Rasterman and Michael Fulbright
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#include <comedi_input.h>
#include <stdio.h>
#include <input.h>
#include <stdlib.h>
#include <string.h>
#include <configfile.h>

#ifdef HAVE_COMEDI
char *cmdtest_messages[];
char *cmd_src(int src,char *buf);


/*
  Modify the scan rate for streaming input.  This function assumes
  that cmd is already a valid structure.
  This mimics, somewhat, the behavior of comedi_get_cmd_generic_timed(...)
  except that it does not reset cmd.  On success, it returns the
  modified rate.  On failure, it returns 0.

  This was based on comedilib/lib/cmd.c internal 
  routine __generic_timed(...)
*/

unsigned int comedi_command_timed(comedi_t *it, comedi_cmd *cmd, 
			       unsigned int ns)
{
	const int err=0;  /* error return */
	int ret;

	/* Potential bug: there is a possibility that the source mask may
	 * have * TRIG_TIMER set for both convert_src and scan_begin_src,
	 * but they may not be supported together. */
	if(cmd->convert_src&TRIG_TIMER){
		if(cmd->scan_begin_src&TRIG_FOLLOW){
			cmd->convert_src = TRIG_TIMER;
			cmd->convert_arg = ns;
			cmd->scan_begin_src = TRIG_FOLLOW;
			cmd->scan_begin_arg = 0;
		}else{
			cmd->convert_src = TRIG_TIMER;
			cmd->convert_arg = ns;
			cmd->scan_begin_src = TRIG_TIMER;
			cmd->scan_begin_arg = ns;
		}
	}else if(cmd->convert_src & TRIG_NOW &&
		cmd->scan_begin_src & TRIG_TIMER)
	{
		cmd->convert_src = TRIG_NOW;
		cmd->convert_arg = 0;
		cmd->scan_begin_src = TRIG_TIMER;
		cmd->scan_begin_arg = ns;
	}else{
		fprintf(stderr,__FILE__":   can't do timed?\n");
		return err;
	}

	ret=comedi_command_test(it,cmd);
	if(ret==3)   		/* OK, try again */
		ret=comedi_command_test(it,cmd);
	if(ret!=4 && ret!=0)   	/* return with error */
	{  
#ifdef DEBUG
		printf(__FILE__ ":  comedi_command_test %i, returned %i\n"
		       "     See kernel system messages (dmesg)\n",__LINE__,ret);
#endif
		return err;
	}
	
	/* return with new rate */
	if(cmd->convert_src&TRIG_TIMER)
		return cmd->convert_arg;
	else 
		return cmd->scan_begin_arg;
}

#endif


/***************************************************************************

  Routines borrowed from Comedi demo programs

*/

#ifdef HAVE_COMEDI

void probe_max_1chan(comedi_t *it,int s)
{
	comedi_cmd cmd;
	char buf[100];

	printf("  command fast 1chan:\n");
	if(comedi_get_cmd_generic_timed(it,s,&cmd,1)<0){
		printf("    not supported\n");
	}else{
		printf("    start: %s %d\n",
			cmd_src(cmd.start_src,buf),cmd.start_arg);
		printf("    scan_begin: %s %d\n",
			cmd_src(cmd.scan_begin_src,buf),cmd.scan_begin_arg);
		printf("    convert: %s %d\n",
			cmd_src(cmd.convert_src,buf),cmd.convert_arg);
		printf("    scan_end: %s %d\n",
			cmd_src(cmd.scan_end_src,buf),cmd.scan_end_arg);
		printf("    stop: %s %d\n",
			cmd_src(cmd.stop_src,buf),cmd.stop_arg);
	}
}

void get_command_stuff(comedi_t *it,int s)
{
	comedi_cmd cmd;
	char buf[100];

	if(comedi_get_cmd_src_mask(it,s,&cmd)<0){
		printf("    not supported\n");
	}else{
		printf("    start: %s\n",cmd_src(cmd.start_src,buf));
		printf("    scan_begin: %s\n",cmd_src(cmd.scan_begin_src,buf));
		printf("    convert: %s\n",cmd_src(cmd.convert_src,buf));
		printf("    scan_end: %s\n",cmd_src(cmd.scan_end_src,buf));
		printf("    stop: %s\n",cmd_src(cmd.stop_src,buf));
	
		probe_max_1chan(it,s);
	}
}


char *cmd_src(int src,char *buf)
{
        buf[0]=0;

        if(src&TRIG_NONE)strcat(buf,"none|");
        if(src&TRIG_NOW)strcat(buf,"now|");
        if(src&TRIG_FOLLOW)strcat(buf, "follow|");
        if(src&TRIG_TIME)strcat(buf, "time|");
        if(src&TRIG_TIMER)strcat(buf, "timer|");
        if(src&TRIG_COUNT)strcat(buf, "count|");
        if(src&TRIG_EXT)strcat(buf, "ext|");
        if(src&TRIG_INT)strcat(buf, "int|");
#ifdef TRIG_OTHER
        if(src&TRIG_OTHER)strcat(buf, "other|");
#endif

        if(strlen(buf)==0){
                sprintf(buf,"unknown(0x%08x)",src);
        }else{
                buf[strlen(buf)-1]=0;
        }

        return buf;
}

/*
  The following are from the comedilib demo directory.
  They should be part of the comedilib API ...
*/

char *cmdtest_messages[]={
        "success",
        "invalid source",
        "source conflict",
        "invalid argument",
        "argument conflict",
        "invalid chanlist",
};

char *subdevice_types[N_SUBDEVICE_TYPES]={
	"unused",
	"analog input",
	"analog output",
	"digital input",
	"digital output",
	"digital I/O",
	"counter",
	"timer",
	"memory",
	"calibration",
	"processor"
};

/*
 default values for comedi_cmd structure
 rate is samples per second for each channel.

 returns the result of a comedi_command_test.
 This is based on comedilib/demo/cmd.c
*/

int default_comedi_cmd(comedi_t *dev, comedi_cmd *cmd, float *rate)
{
	int ret;
	unsigned int *channels;
	unsigned int n_channels;
	unsigned int period;
	unsigned int subdevice = 0; /* choose comedi subdevice */
	unsigned int aref= AREF_GROUND;
	unsigned int range=3;  // which voltage range to use

	if(dev == NULL) /* don't do anything if there is no device */
	{
		fprintf(stderr,__FILE__":  dev is null, can't initialize\n");
		return -1;
	}

	/* set default channels, but don't put into comedi_cmd */
	channels=malloc(16*sizeof(*cmd->chanlist));
	n_channels=0;
	channels[n_channels++]=CR_PACK(8,range,aref);
#if 0
    	channels[n_channels++]=CR_PACK(9,range,aref);
#endif
	if(n_channels > comedi_get_n_channels(dev,subdevice))
	{
		fprintf(stderr,__FILE__":  Too many channels defined\n");
		return -1;
	}

	/* comedi rate is samples per nanosecond for 
	   all channels being read.  
	   Assume rate in samples per second for each channel. */
	*rate=22222;

	period=1e9/(*rate*(float) n_channels);
#ifdef DEBUG
	printf(__FILE__",%i:  period %i ns\n",__LINE__,period);
#endif
	ret = comedi_get_cmd_generic_timed(dev,subdevice,cmd,period);
	if(ret<0){
		comedi_perror("comedi_get_cmd_generic_timed\n");
		return ret;
	} 

	/* For some reason, comedi_get_cmd_generic_timed stomps
           on any allocation of cmd->chanlist */
	cmd->chanlist_len=n_channels;
	cmd->chanlist=channels;
        cmd->scan_end_arg = n_channels;

	/* These are needed to get a signal */
	cmd->stop_src = TRIG_NONE;
        cmd->stop_arg = 0;
	
	/* Test it once */
        ret=comedi_command_test(dev,cmd);
#ifdef DEBUG
	if(ret)
		printf(__FILE__",%i:  default comedi_command_test(...) returned %i\n",
		       __LINE__,ret);
#endif
	return ret;
}

/*
  routines to save and load a comedi_cmd structure and
  the rate to and from a config file.

  note that this mallocs two arrays in cmd.
*/

#define COMEDI_CONFIG_FILE "/.eXtace/comedi"

int read_comedi_cmd(comedi_cmd *cmd, float *rate)
{
	int i;
	int temp;
        ConfigFile *cfgfile;
        gchar *filename;
	char str[32];

	filename = g_strconcat(g_get_home_dir(), COMEDI_CONFIG_FILE, NULL);
	cfgfile = cfg_open_file(filename);
	if (cfgfile)
        {		
		cfg_read_float(cfgfile, "other", "rate", rate);
		cfg_read_int(cfgfile, "cmd", "subdev", &cmd->subdev);
		cfg_read_int(cfgfile, "cmd", "flags", &cmd->flags);

		cfg_read_int(cfgfile, "cmd", "start_src", &cmd->start_src);
		cfg_read_int(cfgfile, "cmd", "start_arg", &cmd->start_arg);

		cfg_read_int(cfgfile, "cmd", "begin_src", &cmd->scan_begin_src);
		cfg_read_int(cfgfile, "cmd", "begin_arg", &cmd->scan_begin_arg);

		cfg_read_int(cfgfile, "cmd", "convert_src", &cmd->convert_src);
		cfg_read_int(cfgfile, "cmd", "convert_arg", &cmd->convert_arg);

		cfg_read_int(cfgfile, "cmd", "scan_end_src", &cmd->scan_end_src);
		cfg_read_int(cfgfile, "cmd", "scan_end_arg", &cmd->scan_end_arg);

		cfg_read_int(cfgfile, "cmd", "stop_src", &cmd->stop_src);
		cfg_read_int(cfgfile, "cmd", "stop_arg", &cmd->stop_arg);

		cfg_read_int(cfgfile, "cmd", "chanlist_len", &cmd->chanlist_len);
		cmd->chanlist=malloc(cmd->chanlist_len*sizeof(*cmd->chanlist));
		for(i=0; i<cmd->chanlist_len; i++)
		{
			sprintf(str,"chanlist_%i",i);
			cfg_read_int(cfgfile, "cmd", str, cmd->chanlist+i);
#if 0
			printf("reading channel[%i]=%x\n",i,cmd->chanlist[i]);
#endif
		}

		cfg_read_int(cfgfile, "cmd", "data_len", &cmd->data_len);
		cmd->data=malloc(cmd->data_len*sizeof(*cmd->data));
		for(i=0; i<cmd->data_len; i++)
		{
			sprintf(str,"data_%i",i);
			cfg_read_int(cfgfile, "cmd", str, &temp);
			cmd->data[i]=temp;
		}

                cfg_free(cfgfile);
	}
        else
                printf("Config file not found, using defaults\n");
        g_free(filename);

	return cfgfile?0:-1; /* if successful, return 0 */
}

/*  write the comedi_cmd structure and the rate to the config file */

int write_comedi_cmd(comedi_cmd *cmd, float rate)
{
	int i;
	char str[32];
        ConfigFile *cfgfile;
        gchar *filename;

	filename = g_strconcat(g_get_home_dir(), COMEDI_CONFIG_FILE, NULL);
	cfgfile = cfg_open_file(filename);
	if (!cfgfile)
                cfgfile = cfg_new();

	cfg_write_float(cfgfile, "other", "rate", rate);
	cfg_write_int(cfgfile, "cmd", "subdev", cmd->subdev);
	cfg_write_int(cfgfile, "cmd", "flags", cmd->flags);

	cfg_write_int(cfgfile, "cmd", "start_src", cmd->start_src);
	cfg_write_int(cfgfile, "cmd", "start_arg", cmd->start_arg);
	
	cfg_write_int(cfgfile, "cmd", "begin_src", cmd->scan_begin_src);
	cfg_write_int(cfgfile, "cmd", "begin_arg", cmd->scan_begin_arg);
	
	cfg_write_int(cfgfile, "cmd", "convert_src", cmd->convert_src);
	cfg_write_int(cfgfile, "cmd", "convert_arg", cmd->convert_arg);
	
	cfg_write_int(cfgfile, "cmd", "scan_end_src", cmd->scan_end_src);
	cfg_write_int(cfgfile, "cmd", "scan_end_arg", cmd->scan_end_arg);
	
	cfg_write_int(cfgfile, "cmd", "stop_src", cmd->stop_src);
	cfg_write_int(cfgfile, "cmd", "stop_arg", cmd->stop_arg);
	
	cfg_write_int(cfgfile, "cmd", "chanlist_len", cmd->chanlist_len);
	for(i=0; i<cmd->chanlist_len; i++)
	{
		sprintf(str,"chanlist_%i",i);
		cfg_write_int(cfgfile, "cmd", str, cmd->chanlist[i]);
#if 0
		printf("saving channel[%i]=%x\n",i,cmd->chanlist[i]);
#endif
	}
	
	cfg_write_int(cfgfile, "cmd", "data_len", cmd->data_len);
	for(i=0; i<cmd->data_len; i++)
	{
		sprintf(str,"data_%i",i);
		cfg_write_int(cfgfile, "cmd", str, cmd->data[i]);
	}
	
        cfg_write_file(cfgfile, filename);
	cfg_free(cfgfile);
        g_free(filename);

	return 0;  /* if successful, return 0 */
}

/* free memory used by comedi_cmd structure */

int free_comedi_cmd(comedi_cmd *cmd)
{
	cmd->data_len=0;
	cmd->chanlist_len=0;
	free(cmd->data);
	free(cmd->chanlist);

	return 0;
}

#endif




