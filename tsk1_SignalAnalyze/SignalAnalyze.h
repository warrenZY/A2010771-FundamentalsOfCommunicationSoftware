#ifndef __SignalAnalyze_H__
#define __SignalAnalyze_H__
#define MaxNumOfDSigFiles 12
#define MaxNumOfSSigFiles 3
#define DSigFileNameOffset 40
#define SSigFileNameOffset 0
#define MaxLenOfDSig 30000
#define MaxLenOfSSig 4096
#define MaxNumOfSort 6
#include <stdio.h>
extern const char* DeterministicSignalFileName;
extern const char* SampleSignalFileName;
extern const char* FileExtension;
extern const char* WorkLoadEnvDir;


extern const unsigned int StartOfDsigFiles;
extern const unsigned int StartOfSSigFiles;

char DSigFileNames[MaxNumOfDSigFiles];
char SSigFileNames[MaxNumOfSSigFiles];

float DSigData[MaxNumOfDSigFiles][MaxLenOfDSig / 2];
float DsigCorrelatedData[MaxNumOfDSigFiles][MaxLenOfDSig / 2];
float SSigData[MaxNumOfSSigFiles][MaxLenOfSSig];


//Storage complex number
typedef struct
{
	double Re;
	double Im;
}ComplexNum;

//Storage deterministic signal information
typedef struct
{
	unsigned int FileCanBeOpen[MaxNumOfDSigFiles];
	double PowerValue[MaxNumOfDSigFiles];
	double PeakValue[MaxNumOfDSigFiles][MaxNumOfSSigFiles];
	int PeakValueIndex[MaxNumOfDSigFiles][MaxNumOfSSigFiles];

}DSigInfo;

typedef struct
{
	unsigned int FileCanBeOpen[MaxNumOfDSigFiles];
	double PowerValue[MaxNumOfDSigFiles];

}SSigInfo;


//get file path with customize work environment
char* GetFileFullPathSetDir(char* workLoadEnvDir, char* fileClassName, unsigned int index, char* extension);

//Generate full path using file name, index and file extension
char* GetFileFullPath(char* fileClassName, unsigned int index, char* extension);

//Only works when File opned, get lenth of FileStream
unsigned int GetFileLenth(FILE* FileStream);

//Check deterministic signal files and calculate it's power
int ReadDSigFile(DSigInfo* _dSigInfo);
//Check sample signal files
int ReadSSigFile(SSigInfo* _sSigInfo);

//Find max power value, return index with no offset
int SelectSort(DSigInfo* _dSigInfo, SSigInfo* _sSigInfo, double(*sortResult)[MaxNumOfDSigFiles]);

//Detect correlation using slide window and output result
int CorrelationDetection(unsigned int DSigIndex, DSigInfo* _dSigInfo, unsigned int SSigIndex, SSigInfo* _sSigInfo);

//Calculate complex number multiplication
int CalculateComplexNumMultiplication(ComplexNum multiplicand, ComplexNum multiplier, ComplexNum* _product);

#endif // __SignalAnalyze_H__
