/*
 * sine.c
 *
 * Generates a sine wave that steps gradually down from 22050 hertz down.
 * Useful for testing eXtace for accuracy and Window function performance
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Prototypes */
int main(int,char **);
void usage(char **);
void check_limits(char **);

/* Globals */
int low_freq;
int high_freq;

int main(int argc, char **argv)
{
//	short v;
//	double a=0;
//	float f = 2;
//	int i;
//	int count = 22050;	 /* number of discrete steps */
//	int delay = 40000;
//	int amplitude = 32768;
	
//	printf("argc is %i\n",argc);
	
//	while (--argc > 0)
//		printf("%s%s", *++argv, (argc > 1) ? " " : "");
//	printf("\n");
	if (argc != 3)
		usage(argv);
	else
		check_limits (argv);
	
	printf("This app isn't complete yet and of now does nothing except check the input arguments and their values for compliance\n");

	/* Sweep type for now will be a simple linear sweep between points
	 * "a" and "b" Sweep time will be fixed until I see the need to expose
	 * that to the user.  Waveform will be a sinewave at 50% amplitude to
	 * avoid blowing one's speakers out. 
	 */

//	while(count--)
//	{
//		f++;
//		fprintf(stderr,"%5.5f\n",44100/(double)f);
//		for (i=0;i<(float)delay/f;i++)
//		{
//			for(a=(M_PI/(double)f);a<(2*M_PI);a+=(2*M_PI/(double)f))
//			{
//				v=(short)(sin(a)*amplitude);
//				fwrite(&v,sizeof(short),1,stdout);
//				fwrite(&v,sizeof(short),1,stdout);
//			}
//		}
//	}
	return 0;
}

void usage(char **argv)
{
	printf("%s requires two arguments\n\n1. The low frequency in hertz \n2. The high frequency in hertz\n\n The maximum is 20,000 hertz, minimum is 1 hertz\n\n sweep will generate a sine wave sweep to verify eXtace is functioning normally\n\n",argv[0]);
	exit(-1);
}

void check_limits(char **arguments)
{
	int tmp;
	int error = 0;
	tmp = atoi(arguments[1]);
	if ((tmp < 1) || (tmp > 20000))
	{
		printf("Low frequency value %i is out of range (1<=X<=20000)\n",tmp);
		error = 1;
	}
	else
		low_freq = tmp;

	tmp = atoi(arguments[2]);
	if ((tmp < 1) || (tmp > 20000))
	{
		printf("High frequency value %i is out of range (1<=X<=20000)\n",tmp);
		error = 1;
	}
	else
		high_freq = tmp;

	if (error)
		exit(-2);

}
