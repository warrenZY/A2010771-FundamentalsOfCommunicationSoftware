#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "string.h"
#include "BitMapPicture.h"

int ReadBmpFileHeader(char* filepath, BITMAPFILEHEADER* bmfh)
{
    FILE* fp;
    fp = fopen(filepath, "rb");
    if (!fp)
    {
        return -1;
    }
    if (fread(&bmfh->bfType, sizeof(WORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmfh->bfSize, sizeof(DWORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmfh->bfReserved1, sizeof(WORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmfh->bfReserved2, sizeof(WORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmfh->bfOffBits, sizeof(DWORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}


int ReadBmpInfoHeader(char* filepath, BITMAPINFOHEADER* bmih)
{
    FILE* fp;
    fp = fopen(filepath, "rb");
    if (!fp)
    {
        return -1;
    }
    fseek(fp, 14, SEEK_SET);
    if (fread(&bmih->biSize, sizeof(DWORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmih->biWidth, sizeof(LONG), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmih->biHeight, sizeof(LONG), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmih->biPlanes, sizeof(WORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmih->biBitCount, sizeof(WORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmih->biCompression, sizeof(DWORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmih->biSizeImage, sizeof(DWORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmih->biXPelsPerMeter, sizeof(LONG), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmih->biYPelsPerMeter, sizeof(LONG), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmih->biClrUsed, sizeof(DWORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    if (fread(&bmih->biClrImportant, sizeof(DWORD), 1, fp) != 1)
    {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}


void PrintBmpFileHeader(BITMAPFILEHEADER* bmfh)
{
    printf("The contents in the file header of the BMP file:\n");
    printf("bfOffBits: %ld\n", (long int)bmfh->bfOffBits);
    printf("bfReserved1: %ld\n", (long int)bmfh->bfReserved1);
    printf("bfReserved2: %ld\n", (long int)bmfh->bfReserved2);
    printf("bfSize: %ld\n", (long int)bmfh->bfSize);
    printf("bfType: %ld\n", (long int)bmfh->bfType);
}

void PrintBmpInfoHeader(BITMAPINFOHEADER* bmih)
{
    printf("The content in the info header of the BMP file:\n");
    printf("biBitCount: %ld\n", (long int)bmih->biBitCount);
    printf("biClrImportant: %ld\n", (long int)bmih->biClrImportant);
    printf("biClrUsed: %ld\n", (long int)bmih->biClrUsed);
    printf("biCompression: %ld\n", (long int)bmih->biCompression);
    printf("biHeight: %ld\n", (long int)bmih->biHeight);
    printf("biPlanes: %ld\n", (long int)bmih->biPlanes);
    printf("biSize: %ld\n", (long int)bmih->biSize);
    printf("biSizeImage: %ld\n", (long int)bmih->biSizeImage);
    printf("biWidth: %ld\n", (long int)bmih->biWidth);
    printf("biXPelsPerMeter: %ld\n", (long int)bmih->biXPelsPerMeter);
    printf("biYPelsPerMeter: %ld\n", (long int)bmih->biYPelsPerMeter);
}



BYTE* ReadBmp(BYTE* filepath, BITMAPFILEHEADER* bmf, BITMAPINFOHEADER* bmi)
{
    BYTE* imgData;
    BITMAPINFOHEADER bmih;
    BITMAPFILEHEADER bmfh;
    FILE* fp;
    u8 R, G, B;
    u16 pixel;
    int n, i;
    int width;
    int height;
    int bitCount;
    DWORD dwLineBytes;
    n = ReadBmpFileHeader(filepath, &bmfh);
    if (n == -1)
    {
        printf("Can not read the file header of the BMP file.\n");
        return NULL;
    }
    n = ReadBmpInfoHeader(filepath, &bmih);
    if (n == -1)
    {
        printf("Can not read the info header of the BMP file.\n");
        return NULL;
    }

    memcpy(bmi, &bmih, sizeof(BITMAPINFOHEADER));
    memcpy(bmf, &bmfh, sizeof(BITMAPFILEHEADER));
#ifdef DEBUG
    PrintBmpFileHeader(bmf);
    PrintBmpInfoHeader(bmi);
#endif // DEBUG

    width = bmih.biWidth;
    height = bmih.biHeight;
    bitCount = bmih.biBitCount;
    imgData = (BYTE*)malloc((bitCount / (8 * sizeof(BYTE))) * width * height * sizeof(BYTE));
    fp = fopen(filepath, "rb");
    if (!fp)
    {
        printf("Can not open the file: %s\n", filepath);
        return NULL;
    }
    fseek(fp, bmfh.bfOffBits, SEEK_SET);//move pointer to start of data

    if (fread(imgData, (bitCount / (8 * sizeof(BYTE))) * width * height * sizeof(BYTE), 1, fp) != 1)
    {

        free(imgData);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return imgData;
}



BYTE WriteBmp(BYTE* filepath, BYTE* imgData, BITMAPFILEHEADER* bmf, BITMAPINFOHEADER* bmi, PICTYPE pt)
{
    FILE* fp;
    int i;
    RGBQuad* IpRGBQuad;
    LONG height = bmi->biHeight;
    DWORD dwLineBytes = (bmi->biBitCount / (8 * sizeof(BYTE))) * bmi->biWidth;
    fp = fopen(filepath, "wb");
    if (!fp)
    {
        printf("Error: Can not open the file:%s\n", filepath);
    }
    switch (pt)
    {

    case LI_BMP_1://BW
        dwLineBytes = (bmi->biWidth + 8 - bmi->biWidth % 8) / 8;

        if (fwrite(&(bmf->bfType), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfSize), sizeof(DWORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfReserved1), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfReserved2), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfOffBits), sizeof(DWORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(bmi, 40, 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        /*图像的读取顺序是从下到上,从左到右*/
        IpRGBQuad = (RGBQuad*)malloc(2 * sizeof(RGBQuad));
        IpRGBQuad[0].rgbRed = 0;
        IpRGBQuad[0].rgbGreen = 0;
        IpRGBQuad[0].rgbBlue = 0;
        IpRGBQuad[0].rgbReserved = 0;

        IpRGBQuad[1].rgbRed = 255;
        IpRGBQuad[1].rgbGreen = 255;
        IpRGBQuad[1].rgbBlue = 255;
        IpRGBQuad[0].rgbReserved = 0;
        fwrite(IpRGBQuad, 8, 1, fp);
        if (fwrite(imgData, dwLineBytes * height, 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        break;

    case LI_BMP_8://GRAY
        PicLog("WRITE GRAY");
        if (fwrite(&(bmf->bfType), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfSize), sizeof(DWORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfReserved1), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfReserved2), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfOffBits), sizeof(DWORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(bmi, 40, 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        /*图像的读取顺序是从下到上,从左到右*/

        IpRGBQuad = (RGBQuad*)malloc(256 * sizeof(RGBQuad));//灰度图为8位的调色板数据为256个结构,1024个字节
        for (int i = 0; i < 256; i++) {
            IpRGBQuad[i].rgbRed = i;
            IpRGBQuad[i].rgbGreen = i;
            IpRGBQuad[i].rgbBlue = i;
            IpRGBQuad[i].rgbReserved = 0;
        }
        fwrite(IpRGBQuad, 1024, 1, fp);
        if (fwrite(imgData, height * dwLineBytes, 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        break;

    case LI_BMP_888://BMP888
        //因为存在字节对齐问题不能一次写入
        if (fwrite(&(bmf->bfType), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfSize), sizeof(DWORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfReserved1), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfReserved2), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfOffBits), sizeof(DWORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(bmi, 40, 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(imgData, height * dwLineBytes, 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        break;

    case LI_BMP_32://BMP32
        if (fwrite(&(bmf->bfType), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfSize), sizeof(DWORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfReserved1), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfReserved2), sizeof(WORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(&(bmf->bfOffBits), sizeof(DWORD), 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(bmi, 40, 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        if (fwrite(imgData, height * dwLineBytes, 1, fp) != 1){
            fclose(fp);
            return -1;
        }
        break;

    default:
        break;
    }

}


void Li_Arr_bgr_gray(LiArr* src, LiArr* dst, LONG width, LONG height, BYTE site)
{
    int x, y;
    switch (site)
    {

    case 0: //12bit convert to gray
        for (x = 0; x < height; x++)
            for (y = 0; y < width; y++)
            {
                *((BYTE*)dst + width * x + y) = (YMap[2][*((BYTE*)src + 3 * width * x + 3 * y)]) //B
                    + YMap[1][(*((BYTE*)src + 3 * width * x + 3 * y + 1))] //G
                    + YMap[0][(*((BYTE*)src + 3 * width * x + 3 * y + 2))]; //R
            }
        break;

    case 4:
        for (x = 0; x < height; x++)
            for (y = 0; y < width; y++)
            {
                *((BYTE*)dst + width * x + y) = (*((BYTE*)src + 3 * width * x + 3 * y) * 2) //B
                    + (*((BYTE*)src + 3 * width * x + 3 * y + 1) * 10) //G
                    + (*((BYTE*)src + 3 * width * x + 3 * y + 2) * 4) >> 4; //R
            }
        break;

    case 8:
        for (x = 0; x < height; x++)
            for (y = 0; y < width; y++)
            {
                *((BYTE*)dst + width * x + y) = (*((BYTE*)src + 3 * width * x + 3 * y) * 30) //B
                    + (*((BYTE*)src + 3 * width * x + 3 * y + 150) * 10) //G
                    + (*((BYTE*)src + 3 * width * x + 3 * y + 2) * 76) >> 8; //R
            }
        break;

    case 16:
        for (x = 0; x < height; x++)
            for (y = 0; y < width; y++)
            {
                *((BYTE*)dst + width * x + y) = (*((BYTE*)src + 3 * width * x + 3 * y) * 7472) //B
                    + (*((BYTE*)src + 3 * width * x + 3 * y + 150) * 38469) //G
                    + (*((BYTE*)src + 3 * width * x + 3 * y + 2) * 19595) >> 16; //R
            }
        break;

    default:
        break;
    }

}