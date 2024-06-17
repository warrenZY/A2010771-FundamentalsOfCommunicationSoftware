#ifndef __BitmapAlterService_H__
#define __BitmapAlterService_H__
#define _CRT_SECURE_NO_WARNINGS 1

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Message.h"


#define PI 3.1415926

extern const char* SrcBmpFilePath;
extern const char* DstAddBorderFilePath;
extern const char* DstGrayFilePath;
extern const char* DstHistogramFilePath;
extern const char* DstPseudocolorFilePath;
extern const char* DstScaleFilePath;
extern const char* DstRotatedFilePath;
extern const char* SrcEdgeDetectionFilePath;
extern const char* DstEdgeDetectionFilePath;


#define PicLog(msg)  printf("%s%s: %s (%s,%d,%s)\n",ErrorMsg,LOG,msg, __FILE__, __LINE__ ,__FUNCTION__)
#define LOG   "BitMapAlterService"

#define myNew(className,Num) (className*)malloc(Num*sizeof(className));

#define snap4byte(input) (input + 31) / 32 * 4;	


/*
* @brief convert Color BMP 2 Gray Scale
* @param 24bit color BMP
* @return  8 bit gray BMP
*/
int BMPRGB2Gray(char* srcBmpFilePath, char* dstGrayFilePath);

/*
* @brief Scale BMP file
* @param 24bit color BMP
* @return  Scaled BMP file
*/
int BMPScale();

/*
* @brief Rotate the BMP file using the input angle
* (24 bit color)
* @param 24bit color BMP/8bit color BMP
* @return Same type of BMP as input
*/
int BMPRotation();

/*
* @brief Add color border to the BMP file
* (24 bit color)
* @param 24bit color BMP
* @return Same type of BMP as input
*/
int BMPAddColorBorder(char* srcBmpFilePath, char* dstBmpFilePath);

/*
* @brief Detect edge using Sobel
* (24 bit color)
* @param 24 bit color BMP
* @return 8 bit gray BMP
* @author –§‘Û”Ó
*/
int BMPEdgeDetection(char* srcBmpFilePath, char* dstBmpFilePath);

#endif // !__BitmapAlterService_H__
