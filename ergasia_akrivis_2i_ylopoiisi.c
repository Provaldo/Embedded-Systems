#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define total_time 7200
#define intervals 100000	// 100.000 microsecs = 0.1 sec

int counter=1;
long int arr[total_time*15][2];
long int drift;

void export_timestamps_csv(long int a[][2], int lines)
{
	char name[]="exported_timestamps2";

	char *filename = name;

	printf ("Creating %s.csv file. \n", filename);

	FILE *fp;
	int i,j;

	filename = strcat (filename, ".csv");

	fp = fopen (filename, "w+");
	fprintf (fp, "Timestamp ID, Seconds, Microseconds");
	
	for (i=0; i<lines; i++)
	{
		fprintf (fp, "\n%d, %ld, %ld", i+1, a[i][0], a[i][1]);
	}
	
	fclose(fp);
	printf("%s file created.\n", filename);
}

void catch_alarm (int sig)
{
	struct timeval timestamp;
	long double temp_drift;
	
	gettimeofday(&timestamp, NULL);
	arr[counter][0] = timestamp.tv_sec;
	arr[counter][1] = timestamp.tv_usec;
	
	temp_drift = arr[counter][0]-arr[counter-1][0];
	temp_drift = temp_drift*1000000 + (long double)((arr[counter][1]-arr[counter-1][1]));

	drift = (long int)(temp_drift);
	drift = 100000 - drift; //drift in microsecs
	//printf ("Drift is: %ld microsecs. \n", drift);
	
	drift *= 1000; //drift in nanosecs
	//printf ("Drift is: %ld nanosecs. \n", drift);
	
	printf ("Sleep cycle number: %d \n", counter+1);
	counter++;
}

int main (void)
{
	struct itimerval old, new;
	struct timeval initial_timestamp;

	struct timespec requested_time, remaining;

	int counter=1;
	unsigned int rem;
	
	new.it_interval.tv_usec = intervals;
  	new.it_interval.tv_sec = 0;
  	new.it_value.tv_usec = intervals;
  	new.it_value.tv_sec = 0;
	
	signal (14, catch_alarm);
	
	gettimeofday (&initial_timestamp, NULL);
	arr[0][0] = initial_timestamp.tv_sec;
	arr[0][1] = initial_timestamp.tv_usec;
	printf ("First timestamp is: %ld seconds, %ld microseconds \n", initial_timestamp.tv_sec, initial_timestamp.tv_usec);
	
	setitimer (ITIMER_REAL, &new, &old);
	printf ("Started sampling \n");

	requested_time.tv_sec = total_time;
	requested_time.tv_nsec = 0;
	nanosleep(&requested_time, &remaining);

	while (1)
	{
		if (rem == 0)
		{
			export_timestamps_csv(arr, total_time*15);
			printf("finished execution \n");
			exit(1);
		}
		else
		{
			requested_time = remaining;
			requested_time.tv_nsec += drift;
			rem = nanosleep(&requested_time, &remaining);
		}
	}
}

