#include "BitmapAlterService.h"

extern const char* SrcBmpFilePath = "./Source.bmp";
extern const char* DstAddBorderFilePath = "./tsk3_AddedBorder.bmp";
extern const char* DstGrayFilePath = "./tsk3_Gray.bmp";
extern const char* DstHistogramFilePath = "./tsk3_HistogramImage.csv";
extern const char* DstPseudocolorFilePath = "./tsk3_PseudocolorImage.bmp";
extern const char* DstScaleFilePath = "./tsk3_Scaled.bmp";
extern const char* DstRotatedFilePath = "./tsk3_Rotated.bmp";
extern const char* SrcEdgeDetectionFilePath = "./Coin.bmp";
extern const char* DstEdgeDetectionFilePath = "./tsk3_EdgeImage.bmp";

/*
* @brief convert Color BMP 2 Gray Scale
* @param 24bit color BMP
* @return  8 bit gray BMP
* @author –§‘Û”Ó
*/
int BMPRGB2Gray(char* srcBmpFilePath, char* dstGrayFilePath) {
	printf("\r%sConverting RGB 2 Gray Scale...", InformationMsg);
	fflush(stdout);
	unsigned char ImgRGBData[3000][3];	// line of RGB data
	unsigned char ImgGrayData[3000];	// line of Gray scale data
	BITMAPFILEHEADER* fileHeader;
	BITMAPINFOHEADER* infoHeader;
	RGBQUAD* rgbQuad;

	FILE* srcBmpFileStream = fopen(srcBmpFilePath, "rb");
	if (!srcBmpFileStream) {
		printf("%sFile %s does not exist.\n", ErrorMsg, srcBmpFilePath);
		printf("%sEncountering critical error, program exit", InformationMsg);
		return -1;
	}
	FILE* dstBmpFileStream = fopen(dstGrayFilePath, "wb");
	if (!dstBmpFileStream) {
		printf("%sCreate file %s Failed.\n", ErrorMsg, dstGrayFilePath);
		printf("%sEncountering critical error, program exit", InformationMsg);
		return -1;
	}

	fileHeader = myNew(BITMAPFILEHEADER, 1);
	infoHeader = myNew(BITMAPINFOHEADER, 1);
	rgbQuad = myNew(RGBQUAD, 256);
	if (!fileHeader || !infoHeader || !rgbQuad) {
		PicLog("Failed allocate mem.");
		return -1;
	}
	fread(fileHeader, sizeof(BITMAPFILEHEADER), 1, srcBmpFileStream);
	fread(infoHeader, sizeof(BITMAPINFOHEADER), 1, srcBmpFileStream);
	infoHeader->biBitCount = 8;
	infoHeader->biSizeImage = ((infoHeader->biWidth + 3) / 4) * 4 * infoHeader->biHeight;
	//biClrUsed=256
	fileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	fileHeader->bfSize = fileHeader->bfOffBits + infoHeader->biSizeImage;

	//Create color table,R=G=B
	for (int index = 0; index < 256; index++) {
		rgbQuad[index].rgbBlue = rgbQuad[index].rgbGreen = rgbQuad[index].rgbRed = index;
		//rgbQuad->rgbReserved = 0;
	}

	//Write info header to dst
	fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, dstBmpFileStream);
	fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, dstBmpFileStream);
	fwrite(rgbQuad, sizeof(RGBQUAD), 256, dstBmpFileStream);

	int heightIndex, widthIndex, colorIndex;
	for (heightIndex = 0; heightIndex < infoHeader->biHeight; heightIndex++) {
		for (widthIndex = 0; widthIndex < (infoHeader->biWidth + 3) / 4 * 4; widthIndex++) {
			for (colorIndex = 0; colorIndex < 3; colorIndex++) {
				fread(&ImgRGBData[widthIndex][colorIndex], 1, 1, srcBmpFileStream);
			}
		}
		for (widthIndex = 0; widthIndex < (infoHeader->biWidth + 3) / 4 * 4; widthIndex++) {
			ImgGrayData[widthIndex] = (int)((float)ImgRGBData[widthIndex][0] * 0.114 +
				(float)ImgRGBData[widthIndex][1] * 0.587 +
				(float)ImgRGBData[widthIndex][2] * 0.299);
		}
		//Write gray scale data per line

		fwrite(ImgGrayData, widthIndex, 1, dstBmpFileStream);
	}

	//Clean up
	free(fileHeader);
	free(infoHeader);
	free(rgbQuad);
	fclose(srcBmpFileStream);
	fclose(dstBmpFileStream);

	printf("\r%sConverting RGB 2 Gray Scale...Done!\n", InformationMsg);
	return 0;
}



/*
* @brief Scale BMP file
* @param 24bit color BMP
* @return  Scaled BMP file
* @author –§‘Û”Ó
*/
int BMPScale() {
	FILE* srcBmpFileStream = fopen(SrcBmpFilePath, "rb");
	if (srcBmpFileStream == 0) {
		printf("%sFile %s does not exist.\n", ErrorMsg, SrcBmpFilePath);
		printf("%sEncountering critical error, program exit", InformationMsg);
		return -1;
	}
	FILE* dstBmpFileStream = fopen(DstScaleFilePath, "wb");
	if (!dstBmpFileStream) {
		printf("%sCreate file %s Failed.\n", ErrorMsg, DstScaleFilePath);
		printf("%sEncountering critical error, program exit", InformationMsg);
		return -1;
	}

	BITMAPFILEHEADER fileHead;
	BITMAPINFOHEADER infoHead;
	BITMAPFILEHEADER dstFileHead;
	BITMAPINFOHEADER dstInfoHead;

	double ratio;
	printf("Please input scale ratio :\n");
	scanf("%lf", &ratio);

	printf("\r%sScaling BMP file...", InformationMsg);
	fflush(stdout);

	fread(&fileHead, sizeof(BITMAPFILEHEADER), 1, srcBmpFileStream);
	fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, srcBmpFileStream);

	dstFileHead = fileHead;
	dstInfoHead = infoHead;

	int width = infoHead.biWidth;
	int height = infoHead.biHeight;

	int newWidth = (int)(width * ratio + 0.5);
	int newHeight = (int)(height * ratio + 0.5);
	int srcLineWidth = snap4byte(width * dstInfoHead.biBitCount);
	int dstLineWidth = snap4byte(newWidth * dstInfoHead.biBitCount);
	dstInfoHead.biWidth = newWidth;
	dstInfoHead.biHeight = newHeight;
	dstInfoHead.biSizeImage = dstLineWidth * dstInfoHead.biHeight;
	dstFileHead.bfSize =
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dstInfoHead.biSizeImage;

	BYTE* pBmpBufSrc = (BYTE*)malloc(srcLineWidth * height);
	memset(pBmpBufSrc, 0, srcLineWidth * height);

	BYTE* pBmpBufDst = (BYTE*)malloc(dstLineWidth * newHeight);
	memset(pBmpBufDst, 0, dstLineWidth * newHeight);


	if (infoHead.biBitCount == 24) {

		fwrite(&dstFileHead, sizeof(BITMAPFILEHEADER), 1, dstBmpFileStream);
		fwrite(&dstInfoHead, sizeof(BITMAPINFOHEADER), 1, dstBmpFileStream);

		int srcImg = srcLineWidth * height;
		int dstImg = dstLineWidth * newHeight;

		fread(pBmpBufSrc, 1, srcImg, srcBmpFileStream);

		int iDstImgWIndex, iDstImgHIndex;
		for (int hIndex = 0; hIndex < newHeight; hIndex++) {
			for (int wIndex = 0; wIndex < newWidth; wIndex++) {
				int index = hIndex * dstLineWidth + wIndex * 3;
				iDstImgWIndex = (int)(wIndex / ratio);
				iDstImgHIndex = (int)(hIndex / ratio);
				if (iDstImgWIndex >= 0 && iDstImgWIndex <= width
					&& iDstImgHIndex >= 0 && iDstImgHIndex <= height) {
					pBmpBufDst[index] = pBmpBufSrc[srcLineWidth * iDstImgHIndex + iDstImgWIndex * 3];
					pBmpBufDst[index + 1] = pBmpBufSrc[srcLineWidth * iDstImgHIndex + iDstImgWIndex * 3 + 1];
					pBmpBufDst[index + 2] = pBmpBufSrc[srcLineWidth * iDstImgHIndex + iDstImgWIndex * 3 + 2];

				}
			}
		}


		fwrite(pBmpBufDst, 1, dstImg, dstBmpFileStream);
		free(pBmpBufSrc);
		free(pBmpBufDst);
	}

	else if (infoHead.biBitCount == 8 && !pBmpBufDst) {
		RGBQUAD strPla[256];//copy color table
		for (unsigned int nCounti = 0; nCounti < 256; nCounti++) {
			fread((char*)&(strPla[nCounti].rgbBlue), 1, sizeof(BYTE), srcBmpFileStream);
			fread((char*)&(strPla[nCounti].rgbGreen), 1, sizeof(BYTE), srcBmpFileStream);
			fread((char*)&(strPla[nCounti].rgbRed), 1, sizeof(BYTE), srcBmpFileStream);
			fread((char*)&(strPla[nCounti].rgbReserved), 1, sizeof(BYTE), srcBmpFileStream);
		}
		//write color table
		for (int nCounti = 0; nCounti < 256; nCounti++) {
			fwrite((char*)&(strPla[nCounti].rgbBlue), 1, sizeof(BYTE), dstBmpFileStream);
			fwrite((char*)&(strPla[nCounti].rgbGreen), 1, sizeof(BYTE), dstBmpFileStream);
			fwrite((char*)&(strPla[nCounti].rgbRed), 1, sizeof(BYTE), dstBmpFileStream);
			fwrite((char*)&(strPla[nCounti].rgbReserved), 1, sizeof(BYTE), dstBmpFileStream);
		}

		fread(pBmpBufSrc, 256 * sizeof(RGBQUAD) * width, height, srcBmpFileStream);
		//Init pixel in new image
		for (int i = 0; i < newHeight; ++i) {
			for (int j = 0; j < newWidth; ++j) {
				*(pBmpBufDst + i * newWidth + j) = 0;
			}
		}

		int srcImg = srcLineWidth * height;
		int dstImg = dstLineWidth * newHeight;

		fread(pBmpBufSrc, 1, srcImg, srcBmpFileStream);

		int iDstImgWIndex, iDstImgHIndex;
		for (int hIndex = 0; hIndex < newHeight; hIndex++) {
			for (int wIndex = 0; wIndex < newWidth; wIndex++) {
				int index = hIndex * dstLineWidth + wIndex * 3;
				iDstImgWIndex = (int)(wIndex / ratio);
				iDstImgHIndex = (int)(hIndex / ratio);
				if (iDstImgWIndex > 0 && iDstImgWIndex < width
					&& iDstImgHIndex> 0 && iDstImgHIndex < height) {
					pBmpBufDst[index] = pBmpBufSrc[srcLineWidth * iDstImgHIndex + iDstImgWIndex * 3];
					pBmpBufDst[index + 1] = pBmpBufSrc[srcLineWidth * iDstImgHIndex + iDstImgWIndex * 3 + 1];
					pBmpBufDst[index + 2] = pBmpBufSrc[srcLineWidth * iDstImgHIndex + iDstImgWIndex * 3 + 2];

				}
			}
		}
		if (!pBmpBufDst) {
			fwrite(pBmpBufDst, 1, dstImg, dstBmpFileStream);
		}
		free(pBmpBufSrc);
		free(pBmpBufDst);
	}

	fclose(srcBmpFileStream);
	fclose(dstBmpFileStream);
	printf("\r%sScaling BMP file...Done!\n", InformationMsg);
	printf("%sComplete saving image at %s.\n", InformationMsg, DstRotatedFilePath);
	return 0;
}



/*
* @brief Rotate the BMP file using the input angle
* (24 bit color)
* @param 24bit color BMP/8bit color BMP
* @return Same type of BMP as input
* @author –§‘Û”Ó
*/
int BMPRotation() {

	FILE* srcBmpFileStream = fopen(SrcBmpFilePath, "rb");
	if (srcBmpFileStream == 0) {
		printf("%sFile %s does not exist.\n", ErrorMsg, SrcBmpFilePath);
		printf("%sEncountering critical error, program exit", InformationMsg);
		return -1;
	}
	FILE* dstbmpfileStream = fopen(DstRotatedFilePath, "wb");
	if (!dstbmpfileStream) {
		printf("%sCreate file %s Failed.\n", ErrorMsg, DstRotatedFilePath);
		printf("%sEncountering critical error, program exit", InformationMsg);
		return -1;
	}

	BITMAPFILEHEADER fileHead;
	BITMAPINFOHEADER infoHead;
	BITMAPFILEHEADER dstFileHead;
	BITMAPINFOHEADER dstInfoHead;

	double angle;
	printf("Please input rotation angle (clockwise): \n");
	scanf("%lf", &angle);

	printf("\r%sRotating BMP file...", InformationMsg);
	fflush(stdout);

	fread(&fileHead, sizeof(BITMAPFILEHEADER), 1, srcBmpFileStream);
	fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, srcBmpFileStream);

	dstFileHead = fileHead;
	dstInfoHead = infoHead;

	int width = infoHead.biWidth;
	int height = infoHead.biHeight;

	double theta = angle * PI / 180;

	int newWidth = abs(width * cos(theta) + height * sin(theta));
	int newHeight = abs(width * sin(theta) + height * cos(theta));


	dstInfoHead.biWidth = newWidth;
	dstInfoHead.biHeight = newHeight;
	//4 byte snap
	int writeWidth = snap4byte(newWidth * dstInfoHead.biBitCount);
	dstInfoHead.biSizeImage = writeWidth * dstInfoHead.biHeight;
	dstFileHead.bfSize =
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dstInfoHead.biSizeImage;

	fwrite(&dstFileHead, sizeof(BITMAPFILEHEADER), 1, dstbmpfileStream);
	fwrite(&dstInfoHead, sizeof(BITMAPINFOHEADER), 1, dstbmpfileStream);
	if (infoHead.biBitCount == 24) {
		int srcLineWidth = snap4byte(width * infoHead.biBitCount);
		int dstLineWidth = snap4byte(dstInfoHead.biWidth * dstInfoHead.biBitCount);
		int srcRotateX = width / 2;
		int srcRotateY = height / 2;
		int dstRotateX = newWidth / 2;
		int dstRotateY = newHeight / 2;

		BYTE* pBmpBufSrc = (BYTE*)malloc(srcLineWidth * height);
		memset(pBmpBufSrc, 0, srcLineWidth * height);

		BYTE* pBmpBufDst = (BYTE*)malloc(dstLineWidth * newHeight);
		memset(pBmpBufDst, 0, dstLineWidth * newHeight);

		int srcImg = srcLineWidth * height;
		int dstImg = dstLineWidth * newHeight;

		fread(pBmpBufSrc, 1, srcImg, srcBmpFileStream);

		for (int hIndex = 0; hIndex < newHeight; hIndex++) {
			for (int wIndex = 0; wIndex < newWidth; wIndex++) {
				int index = hIndex * writeWidth + wIndex * 3;
				double dSrcImgWIndex = (wIndex - dstRotateX) * cos(theta) - (hIndex - dstRotateY) * sin(theta) + srcRotateX;
				double dSrcImgHIndex = (wIndex - dstRotateX) * sin(theta) + (hIndex - dstRotateY) * cos(theta) + srcRotateY;

				//set white for blank area
				if (dSrcImgHIndex<0 || dSrcImgWIndex<0 || dSrcImgHIndex>height || dSrcImgWIndex>width) {
					pBmpBufDst[index] = 255;//B
					pBmpBufDst[index + 1] = 255;//G
					pBmpBufDst[index + 2] = 255;//R
					continue;
				}
				else {
					//bilinear interpolation
					int iSrcImgWIndex = dSrcImgWIndex;
					int iSrcImgHIndex = dSrcImgHIndex;
					double distance2aX = dSrcImgWIndex - iSrcImgWIndex;
					double distance2aY = dSrcImgHIndex - iSrcImgHIndex;

					int srcPA = iSrcImgHIndex * srcLineWidth + iSrcImgWIndex * 3;
					int srcPB = iSrcImgHIndex * srcLineWidth + (iSrcImgWIndex + 1) * 3;
					int srcPC = (iSrcImgHIndex + 1) * srcLineWidth + iSrcImgWIndex * 3;
					int srcPD = (iSrcImgHIndex + 1) * srcLineWidth + (iSrcImgWIndex + 1) * 3;

					if (iSrcImgWIndex == width - 1) {
						srcPA = srcPB;
						srcPC = srcPD;
					}
					if (iSrcImgHIndex == height - 1) {
						srcPC = srcPA;
						srcPD = srcPB;
					}
					//blue
					pBmpBufDst[index] =
						(1 - distance2aX) * (1 - distance2aY) * pBmpBufSrc[srcPA]
						+ (1 - distance2aX) * distance2aY * pBmpBufSrc[srcPB]
						+ distance2aX * (1 - distance2aY) * pBmpBufSrc[srcPC]
						+ distance2aX * distance2aY * pBmpBufSrc[srcPD];
					//green
					pBmpBufDst[index + 1] =
						(1 - distance2aX) * (1 - distance2aY) * pBmpBufSrc[srcPA + 1]
						+ (1 - distance2aX) * distance2aY * pBmpBufSrc[srcPB + 1]
						+ distance2aX * (1 - distance2aY) * pBmpBufSrc[srcPC + 1]
						+ distance2aX * distance2aY * pBmpBufSrc[srcPD + 1];
					//red
					pBmpBufDst[index + 2] =
						(1 - distance2aX) * (1 - distance2aY) * pBmpBufSrc[srcPA + 2]
						+ (1 - distance2aX) * distance2aY * pBmpBufSrc[srcPB + 2]
						+ distance2aX * (1 - distance2aY) * pBmpBufSrc[srcPC + 2]
						+ distance2aX * distance2aY * pBmpBufSrc[srcPD + 2];

				}
			}
		}
		fwrite(pBmpBufDst, 1, dstImg, dstbmpfileStream);
		free(pBmpBufSrc);
		free(pBmpBufDst);
	}
	else if (infoHead.biBitCount == 8)// 8bit bmp
	{
		RGBQUAD strPla[256];//copy color table
		for (unsigned int nCounti = 0; nCounti < 256; nCounti++) {
			fread((char*)&(strPla[nCounti].rgbBlue), 1, sizeof(BYTE), srcBmpFileStream);
			fread((char*)&(strPla[nCounti].rgbGreen), 1, sizeof(BYTE), srcBmpFileStream);
			fread((char*)&(strPla[nCounti].rgbRed), 1, sizeof(BYTE), srcBmpFileStream);
			fread((char*)&(strPla[nCounti].rgbReserved), 1, sizeof(BYTE), srcBmpFileStream);
		}
		//write color table
		for (int nCounti = 0; nCounti < 256; nCounti++) {
			fwrite((char*)&(strPla[nCounti].rgbBlue), 1, sizeof(BYTE), dstbmpfileStream);
			fwrite((char*)&(strPla[nCounti].rgbGreen), 1, sizeof(BYTE), dstbmpfileStream);
			fwrite((char*)&(strPla[nCounti].rgbRed), 1, sizeof(BYTE), dstbmpfileStream);
			fwrite((char*)&(strPla[nCounti].rgbReserved), 1, sizeof(BYTE), dstbmpfileStream);
		}
		int srcLineWidth = snap4byte(width * infoHead.biBitCount);
		BYTE* pBmpBufSrc = (BYTE*)malloc(height * srcLineWidth);
		memset(pBmpBufSrc, 0, height * srcLineWidth);
		BYTE* pBmpBufDst = (BYTE*)malloc(newHeight * writeWidth);
		memset(pBmpBufDst, 0, newHeight * writeWidth);

		fread(pBmpBufSrc, 256 * sizeof(RGBQUAD) * width, height, srcBmpFileStream);
		//Init pixel in new image
		for (int i = 0; i < newHeight; ++i) {
			for (int j = 0; j < newWidth; ++j) {
				*(pBmpBufDst + i * newWidth + j) = 0;
			}
		}
		int rotateX = width / 2;
		int rotateY = height / 2;
		//new center
		int write_rotateX = newWidth / 2;
		int write_rotateY = newHeight / 2;
		int OriginalImg = srcLineWidth * height;
		int LaterImg = writeWidth * newHeight;
		fread(pBmpBufSrc, 1, OriginalImg, srcBmpFileStream);
		for (int i = 0; i < newHeight; ++i) {
			for (int j = 0; j < newWidth; ++j) {
				int index = i * writeWidth + j;
				double dSrcImgHIndex =
					(j - write_rotateX) * sin(theta) + (i - write_rotateY) * cos(theta) + rotateY;
				double dSrcImgWIndex =
					(j - write_rotateX) * cos(theta) - (i - write_rotateY) * sin(theta) + rotateX;
				if ((dSrcImgWIndex >= 0) && (dSrcImgWIndex < width) && (dSrcImgHIndex >= 0) && (dSrcImgHIndex <= height))
				{
					//bilinear interpolation
					int dSrcImgHIndex_True = dSrcImgHIndex;
					int dSrcImgWIndex_True = dSrcImgWIndex;
					double distance2aX = dSrcImgWIndex - dSrcImgWIndex_True;
					double distance2aY = dSrcImgHIndex - dSrcImgHIndex_True;

					int srcPA = dSrcImgHIndex_True * srcLineWidth + dSrcImgWIndex_True;
					int srcPB = dSrcImgHIndex_True * srcLineWidth + (dSrcImgWIndex_True + 1);
					int srcPC = (dSrcImgHIndex_True + 1) * srcLineWidth + dSrcImgWIndex_True;
					int srcPD = (dSrcImgHIndex_True + 1) * srcLineWidth + (dSrcImgWIndex_True + 1);

					if (dSrcImgWIndex_True == width - 1) {
						srcPA = srcPB;
						srcPC = srcPD;
					}
					if (dSrcImgHIndex_True == height - 1) {
						srcPC = srcPA;
						srcPD = srcPB;
					}
					//blue 
					pBmpBufDst[index] = (1 - distance2aX) * (1 - distance2aY) * pBmpBufSrc[srcPA]
						+ (1 - distance2aX) * distance2aY * pBmpBufSrc[srcPB]
						+ distance2aX * (1 - distance2aY) * pBmpBufSrc[srcPC]
						+ distance2aX * distance2aY * pBmpBufSrc[srcPD];
					//green
					pBmpBufDst[index + 1] =
						(1 - distance2aX) * (1 - distance2aY) * pBmpBufSrc[srcPA + 1]
						+ (1 - distance2aX) * distance2aY * pBmpBufSrc[srcPB + 1]
						+ distance2aX * (1 - distance2aY) * pBmpBufSrc[srcPC + 1]
						+ distance2aX * distance2aY * pBmpBufSrc[srcPD + 1];
					//red
					pBmpBufDst[index + 2] =
						(1 - distance2aX) * (1 - distance2aY) * pBmpBufSrc[srcPA + 2]
						+ (1 - distance2aX) * distance2aY * pBmpBufSrc[srcPB + 2]
						+ distance2aX * (1 - distance2aY) * pBmpBufSrc[srcPC + 2]
						+ distance2aX * distance2aY * pBmpBufSrc[srcPD + 2];
				}
			}
		}
		fwrite(pBmpBufDst, 1, LaterImg, dstbmpfileStream);
		free(pBmpBufSrc);
		free(pBmpBufDst);
	}

	fclose(srcBmpFileStream);
	fclose(dstbmpfileStream);
	printf("\r%sRotating BMP file...Done!\n", InformationMsg);
	printf("%sComplete saving image at %s.\n", InformationMsg, DstRotatedFilePath);
	return 0;
}




/*
* @brief Add RGB border to select BMP file
* (24 bit color)
* @param 24bit color BMP
* @return Same type of BMP as input
* @author –§‘Û”Ó
*/
int BMPAddColorBorder(char* srcBmpFilePath, char* dstBmpFilePath)//–ﬁ∏ƒÕºœÒµƒ¥Ê¥¢ ˝æ›:‘ˆº”≤ …´±ﬂøÚ
{

	FILE* srcBmpFileStream = fopen(srcBmpFilePath, "rb");
	if (srcBmpFileStream == 0) {
		printf("%sFile %s does not exist.\n", ErrorMsg, srcBmpFilePath);
		printf("%sEncountering critical error, program exit", InformationMsg);
		return -1;
	}
	FILE* dstBmpFileStream = fopen(dstBmpFilePath, "wb");
	if (!dstBmpFileStream) {
		printf("%sCreate file %s Failed.\n", ErrorMsg, dstBmpFilePath);
		printf("%sEncountering critical error, program exit", InformationMsg);
		return -1;
	}

	BITMAPFILEHEADER fileHead;
	BITMAPINFOHEADER infoHead;
	BITMAPFILEHEADER dstFileHead;
	BITMAPINFOHEADER dstInfoHead;

	double thickness;
	printf("Please input thickness of border (int):\n");
	scanf("%lf", &thickness);


	printf("\r%sAdding border to BMP file...", InformationMsg);
	fflush(stdout);

	fread(&fileHead, sizeof(BITMAPFILEHEADER), 1, srcBmpFileStream);
	fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, srcBmpFileStream);

	dstFileHead = fileHead;
	dstInfoHead = infoHead;

	dstInfoHead.biWidth = infoHead.biWidth * (1 + 0.03 * thickness);
	dstInfoHead.biHeight = infoHead.biHeight * (1 + 0.03 * thickness);

	int srcLineWidth = snap4byte(infoHead.biWidth * infoHead.biBitCount);
	int dstLineWidth = snap4byte(dstInfoHead.biWidth * dstInfoHead.biBitCount);

	dstInfoHead.biSizeImage = dstLineWidth * dstInfoHead.biHeight;
	dstFileHead.bfSize =
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dstInfoHead.biSizeImage;

	fwrite(&dstFileHead, sizeof(BITMAPFILEHEADER), 1, dstBmpFileStream);
	fwrite(&dstInfoHead, sizeof(BITMAPINFOHEADER), 1, dstBmpFileStream);

	BYTE* pBmpBufSrc = (BYTE*)malloc(srcLineWidth * infoHead.biHeight);
	memset(pBmpBufSrc, 0, srcLineWidth * infoHead.biHeight);
	BYTE* pBmpBufDst = (BYTE*)malloc(dstLineWidth * dstInfoHead.biHeight);
	memset(pBmpBufDst, 0, dstLineWidth * dstInfoHead.biHeight);

	int srcImg = srcLineWidth * infoHead.biHeight;
	int dstImg = dstLineWidth * dstInfoHead.biHeight;

	fread(pBmpBufSrc, 1, srcImg, srcBmpFileStream);

	//new center		
	int srcX = infoHead.biWidth / 2;
	int srcY = infoHead.biHeight / 2;
	int dstX = dstInfoHead.biWidth / 2;
	int dstY = dstInfoHead.biHeight / 2;
	int redEdgePosX = srcX * (1 + 0.02 * thickness);
	int redEdgePosY = srcY * (1 + 0.02 * thickness);
	int greenEdgePosX = srcX * (1 + 0.01 * thickness);
	int greenEdgePosY = srcY * (1 + 0.01 * thickness);
	int blueEdgePosX = srcX * 1.00;
	int blueEdgePosY = srcY * 1.00;

	for (int hIndex = -dstY; hIndex < dstY; hIndex++) {
		for (int wIndex = -dstX; wIndex < dstX; wIndex++) {
			//map the index to same position
			int srcIndex = (hIndex + dstY - (int)(srcY * 0.03 * thickness)) * srcLineWidth + (wIndex + dstX - (int)(srcX * 0.03 * thickness)) * 3;
			int dstIndex = (hIndex + dstY) * dstLineWidth + (wIndex + dstX) * 3;
			if (abs(hIndex) > redEdgePosY || abs(wIndex) > redEdgePosX) {
				//red
				pBmpBufDst[dstIndex] = 0;
				pBmpBufDst[dstIndex + 1] = 0;
				pBmpBufDst[dstIndex + 2] = 255;

			}
			else if (abs(hIndex) > greenEdgePosY || abs(wIndex) > greenEdgePosX) {
				//green
				pBmpBufDst[dstIndex] = 0;
				pBmpBufDst[dstIndex + 1] = 255;
				pBmpBufDst[dstIndex + 2] = 0;

			}
			else if (abs(hIndex) > blueEdgePosY || abs(wIndex) > blueEdgePosX) {
				//blue
				pBmpBufDst[dstIndex] = 255;
				pBmpBufDst[dstIndex + 1] = 0;
				pBmpBufDst[dstIndex + 2] = 0;

			}
			else if (pBmpBufSrc[srcIndex + 2]) {

				pBmpBufDst[dstIndex] = pBmpBufSrc[srcIndex];
				pBmpBufDst[dstIndex + 1] = pBmpBufSrc[srcIndex + 1];
				pBmpBufDst[dstIndex + 2] = pBmpBufSrc[srcIndex + 2];
			}
		}
	}

	fwrite(pBmpBufDst, 1, dstImg, dstBmpFileStream);
	fclose(dstBmpFileStream);
	free(pBmpBufSrc);
	free(pBmpBufDst);
	printf("\r%sAdding border to BMP file...Done\n", InformationMsg);

	return 1;
}

// Sobel
int sobelX[3][3] = {
	{-1, 0, 1},
	{-2, 0, 2},
	{-1, 0, 1}
};

int sobelY[3][3] = {
	{-1, -2, -1},
	{0, 0, 0},
	{1, 2, 1}
};


unsigned char rgbToGray(unsigned char r, unsigned char g, unsigned char b) {
	return (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
}


void sobelEdgeDetection(unsigned char* src, int width, int height) {
	int threshold; char flag_gradient;
	printf("\nplease input threshold(0-255)£∫");
	scanf("%d", &threshold);
	printf("Please select grads (x(x),y(y),z(xy))£∫");
	getchar();
	scanf("%c", &flag_gradient, 1);

	int gx, gy, g;

	unsigned char* new_data = (unsigned char*)malloc(width * height * sizeof(unsigned char));
	if (new_data == NULL) {
		PicLog("Failed allocate mem.");
		return;
	}

	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			gx = gy = 0;

			for (int i = -1; i < 2; i++) {
				for (int j = -1; j < 2; j++) {
					gx += sobelX[i + 1][j + 1] * src[(y + i) * width + x + j];
					gy += sobelY[i + 1][j + 1] * src[(y + i) * width + x + j];
				}
			}

			if (flag_gradient == 'z')
				g = (int)sqrt(gx * gx + gy * gy);
			else if (flag_gradient == 'y')
				g = abs(gy);
			else if (flag_gradient == 'x')
				g = abs(gx);


			if (g > threshold) g = threshold; // ∑¿÷π“Á≥ˆ

			// store data
			new_data[(y) * (width)+(x)] = (unsigned char)g;
		}
	}
	// fill no data area cause by edge detect with ZERO
	for (int y = 0; y < height; y++) {
		if (y == 0 || y == height - 1) {
			for (int x = 0; x < width; x++) {
				new_data[y * (width)+x] = 0;
			}
		}
	}
	for (int x = 0; x < width; x++) {
		if (x == 0 || x == width - 1) {
			for (int y = 0; y < height; y++) {
				new_data[y * (width)+x] = 0;
			}
		}
	}

	//copy data back to src
	memcpy(src, new_data, width * height);

	free(new_data);
}

/*
* @brief Detect edge using Sobel
* (24 bit color)
* @param 24 bit color BMP
* @return 8 bit gray BMP
* @author –§‘Û”Ó
*/
int BMPEdgeDetection(char* srcBmpFilePath, char* dstBmpFilePath) {

	printf("\r%sConverting RGB 2 Gray Scale...", InformationMsg);
	fflush(stdout);

	BITMAPFILEHEADER* fileHeader;
	BITMAPINFOHEADER* infoHeader;
	RGBQUAD* rgbQuad;

	FILE* srcBmpFileStream = fopen(srcBmpFilePath, "rb");
	if (!srcBmpFileStream) {
		printf("%sFile %s does not exist.\n", ErrorMsg, srcBmpFilePath);
		printf("%sEncountering critical error, program exit", InformationMsg);
		return -1;
	}
	FILE* dstBmpFileStream = fopen(dstBmpFilePath, "wb");
	if (!dstBmpFileStream) {
		printf("%sCreate file %s Failed.\n", ErrorMsg, dstBmpFilePath);
		printf("%sEncountering critical error, program exit", InformationMsg);
		return -1;
	}

	fileHeader = myNew(BITMAPFILEHEADER, 1);
	infoHeader = myNew(BITMAPINFOHEADER, 1);
	rgbQuad = myNew(RGBQUAD, 256);
	if (!fileHeader || !infoHeader || !rgbQuad) {
		PicLog("Failed allocate mem.");
		return -1;
	}
	fread(fileHeader, sizeof(BITMAPFILEHEADER), 1, srcBmpFileStream);
	fread(infoHeader, sizeof(BITMAPINFOHEADER), 1, srcBmpFileStream);
	int width = infoHeader->biWidth;
	int height = infoHeader->biHeight;
	int newWidth = (infoHeader->biWidth + 3) / 4 * 4;

	infoHeader->biBitCount = 8;
	infoHeader->biSizeImage = ((infoHeader->biWidth + 3) / 4) * 4 * infoHeader->biHeight;
	//biClrUsed=256
	fileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	fileHeader->bfSize = fileHeader->bfOffBits + infoHeader->biSizeImage;

	//Create color table,R=G=B
	for (int index = 0; index < 256; index++) {
		rgbQuad[index].rgbBlue = rgbQuad[index].rgbGreen = rgbQuad[index].rgbRed = index;
		//rgbQuad->rgbReserved = 0;
	}

	//Write info header to dst
	fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, dstBmpFileStream);
	fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, dstBmpFileStream);
	fwrite(rgbQuad, sizeof(RGBQUAD), 256, dstBmpFileStream);


	size_t imageSize = newWidth * height * 3;

	unsigned char* imageData = (unsigned char*)malloc(imageSize);
	fread(imageData, 1, imageSize, srcBmpFileStream);

	// Convert to gray
	unsigned char* grayData = (unsigned char*)malloc(newWidth * height);
	for (int i = 0; i < newWidth * height; i++) {
		grayData[i] = rgbToGray(imageData[i * 3], imageData[i * 3 + 1], imageData[i * 3 + 2]);
	}

	// Detect using Sobel
	sobelEdgeDetection(grayData, newWidth, height);
	fwrite(grayData, 1, newWidth * height, dstBmpFileStream);

	// free mem.
	free(imageData);
	free(grayData);
	fclose(srcBmpFileStream);
	fclose(dstBmpFileStream);
	return 0;
}
