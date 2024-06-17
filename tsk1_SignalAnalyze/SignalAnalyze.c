#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <threads.h>
#include "SignalAnalyze.h"
#include "Message.h"
extern const char* DeterministicSignalFileName = "data";
extern const char* SampleSignalFileName = "PSS";
extern const char* FileExtension = ".txt";
extern const char* WorkLoadEnvDir = "D:/data/";



extern const unsigned int StartOfDsigFiles = 40;
extern const unsigned int StartOfSSigFiles = 0;

extern DSigInfo dSigInfo;
extern SSigInfo sSigInfo;

extern double SortResult[2][MaxNumOfSort] = {0};

//Get file path with customize work environment
char* GetFileFullPathSetDir(char* workLoadEnvDir, char* fileClassName, unsigned int index, char* extension) {
	static char fileFullPath[100];
	sprintf(fileFullPath, "%s%s%d%s", workLoadEnvDir, fileClassName, index, extension);
	return fileFullPath;
}

//Get file path in working dir
char* GetFileFullPath(char* fileClassName, unsigned int index, char* extension) {
	static char fileFullPath[100];
	sprintf(fileFullPath, "%s%s%d%s", WorkLoadEnvDir, fileClassName, index, extension);
	return fileFullPath;
}

//Only works when File opned, get lenth of FileStream
unsigned int GetFileLenth(FILE* fileStream) {
	unsigned int fileLength = 0;
	int _currentChar;
	if (fileStream == NULL)
		return fileLength;
	while ((_currentChar = fgetc(fileStream)) != EOF) {
		if (_currentChar == '\n' || _currentChar == '\r')
			fileLength++;
	}
	rewind(fileStream);
	return fileLength;
}


//Check deterministic signal files and calculate it's power
int ReadDSigFile(DSigInfo* _dSigInfo){
	ComplexNum _currentComplexNum;
	FILE* currentFile;
	unsigned int currentFileLenth;
	printf("%sStart reading deterministic signal file.\n", InformationMsg);
	for (int i = 0 ; i < MaxNumOfDSigFiles; i++) {
		_dSigInfo->FileCanBeOpen[i] = 0;
		
		currentFile = fopen(GetFileFullPath(DeterministicSignalFileName, i + DSigFileNameOffset, FileExtension), "r");
		if (currentFile == NULL)
		{
			printf("%sFile %s does not exist.\n", WarningMsg, GetFileFullPath(DeterministicSignalFileName,
				i + DSigFileNameOffset, FileExtension));
			continue;
		}
		currentFileLenth = GetFileLenth(currentFile);
		if (currentFileLenth > MaxLenOfDSig)
		{
			printf("%sFile %s's lenth exceeds the %d line limit.\n", ErrorMsg, GetFileFullPath(DeterministicSignalFileName, i, FileExtension),MaxLenOfDSig);
			continue;
		}

		_dSigInfo->PowerValue[i] = 0;
		_currentComplexNum.Re = 0;
		_currentComplexNum.Im = 0;
		//calculate PowerValue of
		while (!feof(currentFile)) {
			_dSigInfo->PowerValue[i] += sqrt(_currentComplexNum.Re * _currentComplexNum.Re + _currentComplexNum.Im * _currentComplexNum.Im);
			fscanf(currentFile, "%lf", &_currentComplexNum.Re);
			fscanf(currentFile, "%lf", &_currentComplexNum.Im);
		}
		printf("%sThe power strength of %s is%18lf.\n", InformationMsg,
			GetFileFullPath(DeterministicSignalFileName, i + DSigFileNameOffset, FileExtension),_dSigInfo->PowerValue[i]);
		_dSigInfo->FileCanBeOpen[i] = 1;
		fclose(currentFile);
	}
	printf("%sRead deterministic signal file done!\n", InformationMsg);
	return 0;
}

//Check sample signal files
int ReadSSigFile(SSigInfo* _sSigInfo) {
	FILE* currentFile;
	unsigned int currentFileLenth;

	printf("%sStart reading sample signal file.\n", InformationMsg);
	for (int i = 0; i < MaxNumOfSSigFiles; i++) {
		_sSigInfo->FileCanBeOpen[i] = 0;
		currentFile = fopen(GetFileFullPath(SampleSignalFileName, i + SSigFileNameOffset, FileExtension), "r");
		if (currentFile == NULL)
		{
			printf("%sFile %s does not exist.\n", WarningMsg, GetFileFullPath(SampleSignalFileName, i + SSigFileNameOffset, FileExtension));
			continue;
		}
		currentFileLenth = GetFileLenth(currentFile);
		if (currentFileLenth > MaxLenOfSSig)
		{
			printf("%sFile %s's lenth exceeds the %d line limit\n", ErrorMsg, GetFileFullPath(SampleSignalFileName, i, FileExtension),MaxLenOfSSig);
			continue;
		}
		
		_sSigInfo->FileCanBeOpen[i] = 1;
		fclose(currentFile);
	}
	printf("%sRead sample signal file done!\n", InformationMsg);
	return 0;
}

//Find max power value, return index with no offset
int SelectSort(DSigInfo* _dSigInfo, SSigInfo* _sSigInfo, double(*sortResult) [MaxNumOfDSigFiles]) {
	int maxIndex = 0;
	double	tempIndex, tempValue;
	maxIndex = 0;
	for (int index = 0; index < MaxNumOfDSigFiles; index++) {
		sortResult[0][index] = index;
		sortResult[1][index] = _dSigInfo->PowerValue[index];
	}
/*
	//check if current init
	for (int index = 0; index < MaxNumOfDSigFiles; index++) {
		printf("%s%-2d|File %d\t%lf.\n", InformationMsg, index + 1,(int) sortResult[0][index], sortResult[1][index]);
	}
*/
	for (int index = 0; index < MaxNumOfDSigFiles-1; index++) {
		maxIndex = index;
		for (int j = index+1; j < MaxNumOfDSigFiles; j++) {

			if (sortResult[1][j] > sortResult[1][maxIndex])
				//sortResult[1][maxIndex] equals *((double *)SortResult+1*MaxNumOfDSigFiles+minIndex)
				maxIndex = j;

		}
		tempIndex = sortResult[0][maxIndex];
		tempValue = sortResult[1][maxIndex];
		sortResult[0][maxIndex] = sortResult[0][index];
		sortResult[1][maxIndex] = sortResult[1][index];
		sortResult[0][index] = tempIndex;
		sortResult[1][index] = tempValue;
	}
	printf("%sTop 6 strength are as follows:\n",InformationMsg);
	for (int index = 0; index < MaxNumOfSort; index++) {
		printf("\t%-2d|File %d\t%lf\n", index+1, (int) sortResult[0][index] + DSigFileNameOffset, sortResult[1][index]);
	}
	return 0;
}

//Detect correlation using slide window and output result
int CorrelationDetection(unsigned int dSigIndex, DSigInfo* _dSigInfo, unsigned int sSigIndex, SSigInfo* _sSigInfo) {
	FILE* dSigFile;
	FILE* sSigFile;
	FILE* outputFile;
	char outputFileName[50];
	unsigned int dSigFileLenth = 0;
	unsigned int sSigFileLenth = 0;
	double peakValue = 0;
	ComplexNum productValue = { 0,0 };
	double productValueMod = 0;
	dSigIndex -= DSigFileNameOffset;
	sSigIndex -= SSigFileNameOffset;

	//check file state
	if (_dSigInfo->FileCanBeOpen[dSigIndex] == 0 || _sSigInfo->FileCanBeOpen[sSigIndex] == 0) {
		printf("%sDeterministic Signal File or Sample Signal File is inaccessible!\n", ErrorMsg);
		return -1;
	}

	dSigFile = fopen(GetFileFullPath(DeterministicSignalFileName, dSigIndex + DSigFileNameOffset, FileExtension), "r");
	sSigFile = fopen(GetFileFullPath(SampleSignalFileName, sSigIndex + SSigFileNameOffset, FileExtension), "r");
	dSigFileLenth = GetFileLenth(dSigFile);
	sSigFileLenth = GetFileLenth(sSigFile);

	sprintf(outputFileName, "CorrelationResult_Data%d_PSS%d.csv",
		dSigIndex+DSigFileNameOffset, sSigIndex+SSigFileNameOffset);
	if ((outputFile = fopen(outputFileName, "w")) == NULL) {
		printf("%sFailed creating %s",WarningMsg, outputFileName);
		return -1;
	}
	
	ComplexNum dSigData[MaxLenOfDSig / 2] = {0,0};
	ComplexNum sSigData[MaxLenOfSSig / 2] = {0,0};

	for (int index = 0; index < dSigFileLenth/2; index++) {
		fscanf(dSigFile, "%lf", &dSigData[index].Re);
		fscanf(dSigFile, "%lf", &dSigData[index].Im);
	}

	for (int index = 0; index < sSigFileLenth/2; index++) {
		fscanf(sSigFile, "%lf", &sSigData[index].Re);
		fscanf(sSigFile, "%lf", &sSigData[index].Im);
	}

	//Slide window
	for (int index = 0; index < (dSigFileLenth / 2 - sSigFileLenth / 2); index++) {
		productValue.Im = 0;
		productValue.Re = 0;
		productValueMod = 0;
		for (int position = 0; position < sSigFileLenth / 2; position++) {
			CalculateComplexNumMultiplication(dSigData[index + position], sSigData[position], &productValue);
		}
		productValueMod = sqrt(productValue.Re * productValue.Re + productValue.Im * productValue.Im);
		fprintf(outputFile, "%lf\n", productValueMod);
		if (peakValue < productValueMod) {
			peakValue = productValueMod;
			_dSigInfo->PeakValueIndex[dSigIndex][sSigIndex] = index;
			_dSigInfo->PeakValue[dSigIndex][sSigIndex] = productValueMod;
		}
	}
	//complete file out put
	fclose(outputFile);
	
	/*
	//check output
	printf("%s%s's correlation with ", InformationMsg,
		GetFileFullPathSetDir("", DeterministicSignalFileName, dSigIndex + DSigFileNameOffset, FileExtension));
	printf("%s: Peak\t%2lf \tat Location\t%d.\n",
		GetFileFullPathSetDir("", SampleSignalFileName, sSigIndex + SSigFileNameOffset, FileExtension),
		_dSigInfo->PeakValue[dSigIndex][sSigIndex], _dSigInfo->PeakValueIndex[dSigIndex][sSigIndex]);
	
	*/
	return 0;
}



//(a+bi)*(c+di)=(ac-bd)+(bc+ad)i
int CalculateComplexNumMultiplication(ComplexNum multiplicand,ComplexNum multiplier, ComplexNum* _product){

	_product->Re += (multiplicand.Re * multiplier.Re - multiplicand.Im * multiplier.Im);
	_product->Im += (multiplicand.Im * multiplier.Re + multiplicand.Re * multiplier.Im);
	return 0;
}

