#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define total_time 720
#define intervals 100000   // 100.000 microsecs = 0.1 sec

int counter=0;
long int arr[total_time*15][2];

void export_timestamps_csv(long int a[][2], int lines)
{
	char name[]="exported_timestamps";

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
	
	gettimeofday(&timestamp, NULL);
	arr[counter][0] = timestamp.tv_sec;
	arr[counter][1] = timestamp.tv_usec;
	printf ("Sleep cycle number: %d \n", counter+1);
	counter++;
}


int main (void)
{
	struct itimerval old, new;
	struct timeval initial_timestamp;
	int counter=1;
	unsigned int remaining_time;
	
	new.it_interval.tv_usec = intervals;
  	new.it_interval.tv_sec = 0;
  	new.it_value.tv_usec = intervals;
  	new.it_value.tv_sec = 0;
	
	signal (SIGALRM, catch_alarm);
	
	gettimeofday (&initial_timestamp, NULL);
	printf ("Timestamp is: %ld seconds, %ld microseconds \n", initial_timestamp.tv_sec, initial_timestamp.tv_usec);
	
	setitimer (ITIMER_REAL, &new, &old);
	printf ("Started sampling \n");
	remaining_time = sleep(total_time);
	while (1)
	{
		if (remaining_time == 0)
		{
			export_timestamps_csv(arr, total_time*15);
			printf("finished execution \n");
			exit(1);
		}
		else
		{
			remaining_time = sleep(remaining_time);
		}
	}
}

