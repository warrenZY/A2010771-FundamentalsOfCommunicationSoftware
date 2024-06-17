#include <stdio.h>
#include <threads.h>
#include <string.h>
#include "ProgressBar.h"

#define ProgressBarLenth 60
#define DefalutflushTimeSec 1

//[=============              23.4%                          ]
//[==========================100.0%==========================]

//keep checking value of float *progressPercentage
int ProgressBar(float* progressPercentage) {
	char charBuffer[ProgressBarLenth+1] = { 0 };
	char runningIndicator[4+1] = { "-/|\\" };
	int index = 0;
	int i = 0;
	while(*progressPercentage<100){
		if (index > 5)
			index = 0;
		//generate progress bar
		for (int i = 0; i < ProgressBarLenth; i++) {
			if (i < (int)(*progressPercentage * 0.6))
				charBuffer[i] = '=';
			else
				charBuffer[i] = ' ';

		}
		//display progress bar and percentage
		printf("\r[%25.25s%04.1f%%%25.25s][%c]",
			charBuffer, *progressPercentage, charBuffer + (strlen(charBuffer) - 25), runningIndicator[index % 4]);
		fflush(stdout);
		thrd_sleep(&(struct timespec) { .tv_nsec = 550000000 }, NULL);
		index++;
	}
	//progress reach 100%
	if ((int)*progressPercentage >= 100) {
		for (int i = 0; i < ProgressBarLenth; i++) {
				charBuffer[i] = '=';
		}
		printf("\r[%25.25s%2d%%%25.25s][%s]\n\n",
			charBuffer, 100, charBuffer + (strlen(charBuffer) - 25), "Done!");
		thrd_sleep(&(struct timespec) { .tv_sec = 1 }, NULL);
	}

	return 0;
	
}

//display title of progress bar
int ProgressBarTitle(char* title) {
	printf("\n%8s\n", title);
}
