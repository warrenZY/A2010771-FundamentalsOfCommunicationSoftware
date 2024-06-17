#define _CRT_SECURE_NO_WARNINGS
#define __STDCPP_THREADS__ 1
/*
Creator: 2022210226-肖泽宇
Function: 利用确定性信号分析实际采样信号相关性。


*/

#include <stdio.h>
#include <math.h>
//threads only available on C11 and above
#include <threads.h>
#include "SignalAnalyze.h"
#include "Message.h"
#include "ProgressBar.h"
#include "ImageGenerate.h"

DSigInfo dSigInfo = { 0 };
SSigInfo sSigInfo = { 0 };
//SortResult[0][*] store index, [1][*] store data
double SortResult[2][MaxNumOfSort];

int CorrelationDetectionPerDSig(unsigned int _dSigIndex);


static float percentage = 0;

int main(void){

	//Multi threads check file availability
	thrd_t LoadDSigThread;
	thrd_t LoadSSigThread;
	thrd_create(&LoadDSigThread, ReadDSigFile, &dSigInfo);
	thrd_create(&LoadSSigThread, ReadSSigFile, &sSigInfo);
	thrd_join(LoadDSigThread, 0);
	thrd_join(LoadSSigThread, 0);

	SelectSort(&dSigInfo, &sSigInfo, SortResult);

	//Multi threads check correlation
	static int thrdRc;
	thrd_t CorrelationDetectionPerDSigThread[MaxNumOfSort];
	for (unsigned int index = 0; index < MaxNumOfSort; index++) {
		unsigned int dSigIndex =(unsigned int) SortResult[0][index] + DSigFileNameOffset;
		thrdRc = thrd_create(
			&CorrelationDetectionPerDSigThread[index], CorrelationDetectionPerDSig, dSigIndex);
		if (thrdRc == thrd_error) {
			printf("%s thrd_create() call failed.\n", ErrorMsg);
		}
		thrd_sleep(&(struct timespec) { .tv_nsec = 5 }, NULL);//wait for 5 ns
	}

	
	//create progress bar and monitor correlation threads
	float* percentagePointer = &percentage;
	thrd_t ProgressBarThread;
	ProgressBarTitle("Compute:Correlation");
	thrdRc = thrd_create(&ProgressBarThread, ProgressBar, percentagePointer);
	if (thrdRc == thrd_error) {
		printf("%s thrd_create() call failed\n.", ErrorMsg);
	}
	thrd_sleep(&(struct timespec) { .tv_nsec = 5 }, NULL);//wait for 5 ns
	
	//Check if all correlation threads exits
	for (unsigned int index = 0; index < MaxNumOfSort; index++) {
		thrd_join(CorrelationDetectionPerDSigThread[index], NULL);
		thrd_sleep(&(struct timespec) { .tv_nsec = 5 }, NULL);
	}
	//Set percentage to 100%, mark operation complete
	if (percentage < 100)
		percentage = 100;

	
	thrd_join(ProgressBarThread, NULL);

	printf("\n\033[47;30m%9s%7s%16s%20s%13s\033[0m\n","No.","Data", "Sample", "Correlation","Position");
	for (unsigned int index = 0; index < MaxNumOfSort; index++) {
		unsigned int dSigIndex = SortResult[0][index];
		for (unsigned int sSigIndex = 0; sSigIndex < MaxNumOfSSigFiles; sSigIndex++) {
			printf("%8d%14s", index * 3 + sSigIndex + 1,
				GetFileFullPathSetDir("", DeterministicSignalFileName, dSigIndex+DSigFileNameOffset, FileExtension));
			printf("%12s \%14.2lf\t%8d\n",
				GetFileFullPathSetDir("", SampleSignalFileName, sSigIndex + SSigFileNameOffset, FileExtension),
				dSigInfo.PeakValue[dSigIndex][sSigIndex], dSigInfo.PeakValueIndex[dSigIndex][sSigIndex]);
		}

		thrd_sleep(&(struct timespec) { .tv_nsec = 5 }, NULL);//wait for 5 ns
	}

	double maxCorrelation[3][MaxNumOfSSigFiles] = {-1};
	for (unsigned int index = 0; index < MaxNumOfSort; index++) {
		unsigned int dSigIndex = SortResult[0][index];
		for (unsigned int sSigIndex = 0; sSigIndex < MaxNumOfSSigFiles; sSigIndex++) {
			if (dSigInfo.PeakValue[dSigIndex][sSigIndex] > maxCorrelation[1][sSigIndex]) {
				maxCorrelation[1][sSigIndex] = dSigInfo.PeakValue[dSigIndex][sSigIndex];
				maxCorrelation[0][sSigIndex] = dSigIndex + DSigFileNameOffset;
				maxCorrelation[2][sSigIndex] = sSigIndex + SSigFileNameOffset;
			}
		}
	}

	for(unsigned int sSigIndex = 0; sSigIndex < MaxNumOfSSigFiles; sSigIndex++){
		printf("%sThe max correlation exist between %s and ", InformationMsg,
			GetFileFullPathSetDir("", DeterministicSignalFileName, (unsigned int)maxCorrelation[0][sSigIndex], FileExtension));
		printf("%s.\n", GetFileFullPathSetDir("", SampleSignalFileName, (unsigned int)maxCorrelation[2][sSigIndex], FileExtension));
	}

	printf("%sSave Correlation output at%s\n", InformationMsg, "CorrelationResult_Data*_PSS*.csv");
	

	for (unsigned int index = 0;index < MaxNumOfSort; index++) {
		unsigned int dSigIndex = SortResult[0][index] + DSigFileNameOffset;
		for (unsigned int sSigIndex = 0+ SSigFileNameOffset;
			sSigIndex < MaxNumOfSSigFiles + SSigFileNameOffset; sSigIndex++) {
			char title[50];
			sprintf(title, "Correlation: data%d x PSS%d", dSigIndex, sSigIndex);
			DrawShape(title, dSigIndex, sSigIndex);
		}
	}

	printf("%sProgram exits.\n", InformationMsg);
	return 0;
}


int CorrelationDetectionPerDSig(unsigned int _dSigIndex) {
	for (unsigned int _sSigIndex = 0 + SSigFileNameOffset; _sSigIndex < MaxNumOfSSigFiles + SSigFileNameOffset; _sSigIndex++) {
		CorrelationDetection(_dSigIndex, &dSigInfo, _sSigIndex, &sSigInfo);
	}
	percentage += (float)100 / MaxNumOfSort;
}



/*
	//detect only top

	//detect all data correlation
	thrd_t CorrelationDetectionPerDSigThread[MaxNumOfDSigFiles];
	int thrdRc;
	for (unsigned int dSigIndex = 0 + DSigFileNameOffset; dSigIndex < MaxNumOfDSigFiles + DSigFileNameOffset; dSigIndex++) {
		thrdRc = thrd_create(
			&CorrelationDetectionPerDSigThread[dSigIndex - DSigFileNameOffset], CorrelationDetectionPerDSig, dSigIndex);
		if (thrdRc == thrd_error) {
			printf("%s thrd_create() call failed\n", ErrorMsg);
		}
		thrd_sleep(&(struct timespec) { .tv_nsec = 5 }, NULL);//wait for 5 ns
	}

	//display all correlation

	for (unsigned int index = 0; index < MaxNumOfDSigFiles; index++) {
		percentage += 100 / MaxNumOfDSigFiles;
		thrd_join(CorrelationDetectionPerDSigThread[index], NULL);
		thrd_sleep(&(struct timespec) { .tv_nsec = 3000000 }, NULL);
	}

	for (unsigned int dSigIndex = 0; dSigIndex < MaxNumOfDSigFiles; dSigIndex++) {
		for (unsigned int sSigIndex = 0; sSigIndex < MaxNumOfSSigFiles; sSigIndex++) {

			printf("%s%s's correlation with ", InformationMsg,
				GetFileFullPathSetDir("", DeterministicSignalFileName, dSigIndex + DSigFileNameOffset, FileExtension));
			printf("%s: Peak\t%2lf \tat Location\t%d\n",
				GetFileFullPathSetDir("", SampleSignalFileName, sSigIndex + SSigFileNameOffset, FileExtension),
				dSigInfo.PeakValue[dSigIndex][sSigIndex], dSigInfo.PeakValueIndex[dSigIndex][sSigIndex]);
		}
	}
	*/

	/*

		typedef struct percentageUpdateData {
		thrd_t targetThreads[MaxNumOfSort];
		unsigned int threadIndex;
		float* _percentage;
		float step;
	}PercentageUpdateData;
		int PercentageUpdate(PercentageUpdateData *data);

		thrd_t PercentageUpdateThread[MaxNumOfSort];
		PercentageUpdateData percentageUpdateData;
		for(unsigned int index = 0; index < MaxNumOfSort; index++)
			percentageUpdateData.targetThreads[index] = CorrelationDetectionPerDSigThread[index];
		percentageUpdateData.threadIndex = 0;
		percentageUpdateData._percentage = &percentage;
		percentageUpdateData.step = (float)100 / MaxNumOfSort;

		for (unsigned int index = 0; index < MaxNumOfSort; index++) {
			percentageUpdateData.threadIndex = index;
			thrd_create(&PercentageUpdateThread, PercentageUpdate,&percentageUpdateData);
			thrd_sleep(&(struct timespec) { .tv_nsec = 5 }, NULL);
		}

		for (unsigned int index = 0; index < MaxNumOfSort; index++) {
			thrd_join(PercentageUpdateThread[index], NULL);
		}

		int PercentageUpdate(PercentageUpdateData* data) {
		thrd_join(data->targetThreads[data->threadIndex], NULL);
		*(data->_percentage)+=data->step;
	}
	*/