/*
 * sine.c
 *
 * Generates a sine wave that steps gradually down from 22050 hertz down.
 * Useful for testing eXtace for accuracy and Window function performance
 *
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	short v;
	double a=0;
	float f = 64;
	int i;
	int count = 22050;	 /* number of discrete steps */
	int delay = 400000;
	int amplitude = 32768;

	while(count--)
	{
		f++;
		fprintf(stderr,"%5.5f\n",44100/(double)f);
		for (i=0;i<(float)delay/f;i++)
		{
			for(a=(M_PI/(double)f);a<(2*M_PI);a+=(2*M_PI/(double)f))
			{
				v=(short)(sin(a)*amplitude);
				fwrite(&v,sizeof(short),1,stdout);
				fwrite(&v,sizeof(short),1,stdout);
			}
		}
	}
	return 0;
}
