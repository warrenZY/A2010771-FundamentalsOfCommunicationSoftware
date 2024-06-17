#ifndef __BitMapPicture_H__
#define __BitMapPicture_H__
#include "stdio.h"

#define PicLog(msg)  printf("%s: %s (%s,%d,%s)\n",LOG,msg, __FILE__, __LINE__ ,__FUNCTION__)
#define LOG   "BitMapPicture_H"

typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef long            LONG;
typedef unsigned char   BYTE;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef uint32_t        u32;
typedef uint16_t        u16;
typedef uint8_t         u8;

typedef enum tagPICTYPE
{
    BMP_1,
    BMP_8,
    BMP_565,
    BMP_888,
    BMP_32,
    JPEG,
    PNG
}PICTYPE;

enum {
    LI_BMP_1,
    LI_BMP_8,
    LI_BMP_565,
    LI_BMP_888,
    LI_BMP_32,
    LI_JPEG,
    LI_PNG,
};

enum
{
    LI_FONT_12 = 12,
    LI_FONT_16 = 16,
    LI_FONT_24 = 24,
    LI_FONT_32 = 32
};

typedef void LiArr;

typedef struct tagLi_Mat
{
    LiArr* arr;//Image data pointer
    LONG width;
    LONG height;
    BYTE datadepth;
    LONG matsize;    //width*height
    LONG arrsize;    //数据的大小matsize*depth
    BYTE Bitcount;   //Bit per pixel = (DEP+1)*8
}Li_Mat;

/**
 *  Y = 0.299 * R + 0.587 * G + 0.114 * B;
    Y=D+E+F;
    D=0.299*R;
    E=0.587*G;
    F=0.114*B;
*/
static short int YMap[3][256] = {
{0,0,0,0,1,1,1,2,2,2,2,3,3,3,4,4,4,5,5,5,5,6,6,6,7,7,7,8,8,8,8,9,9,9,10,10,10,11,11,11,11,12,12,12,13,13,13,14,14,14,14,15,15,15,16,16,16,17,17,17,17,18,18,18,19,19,19,20,20,20,20,21,21,21,22,22,22,23,23,23,23,24,24,24,25,25,25,25,26,26,26,27,27,27,28,28,28,28,29,29,29,30,30,30,31,31,31,31,32,32,32,33,33,33,34,34,34,34,35,35,35,36,36,36,37,37,37,37,38,38,38,39,39,39,40,40,40,40,41,41,41,42,42,42,43,43,43,43,44,44,44,45,45,45,46,46,46,46,47,47,47,48,48,48,49,49,49,49,50,50,50,51,51,51,51,52,52,52,53,53,53,54,54,54,54,55,55,55,56,56,56,57,57,57,57,58,58,58,59,59,59,60,60,60,60,61,61,61,62,62,62,63,63,63,63,64,64,64,65,65,65,66,66,66,66,67,67,67,68,68,68,69,69,69,69,70,70,70,71,71,71,72,72,72,72,73,73,73,74,74,74,75,75,75,75,76},
{0,0,1,1,2,2,3,4,4,5,5,6,7,7,8,8,9,9,10,11,11,12,12,13,14,14,15,15,16,17,17,18,18,19,19,20,21,21,22,22,23,24,24,25,25,26,26,27,28,28,29,29,30,31,31,32,32,33,34,34,35,35,36,36,37,38,38,39,39,40,41,41,42,42,43,44,44,45,45,46,46,47,48,48,49,49,50,51,51,52,52,53,53,54,55,55,56,56,57,58,58,59,59,60,61,61,62,62,63,63,64,65,65,66,66,67,68,68,69,69,70,71,71,72,72,73,73,74,75,75,76,76,77,78,78,79,79,80,80,81,82,82,83,83,84,85,85,86,86,87,88,88,89,89,90,90,91,92,92,93,93,94,95,95,96,96,97,98,98,99,99,100,100,101,102,102,103,103,104,105,105,106,106,107,107,108,109,109,110,110,111,112,112,113,113,114,115,115,116,116,117,117,118,119,119,120,120,121,122,122,123,123,124,125,125,126,126,127,127,128,129,129,130,130,131,132,132,133,133,134,134,135,136,136,137,137,138,139,139,140,140,141,142,142,143,143,144,144,145,146,146,147,147,148,149,149},
{0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,6,6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15,16,16,16,16,16,16,16,16,16,17,17,17,17,17,17,17,17,18,18,18,18,18,18,18,18,18,19,19,19,19,19,19,19,19,19,20,20,20,20,20,20,20,20,20,21,21,21,21,21,21,21,21,22,22,22,22,22,22,22,22,22,23,23,23,23,23,23,23,23,23,24,24,24,24,24,24,24,24,24,25,25,25,25,25,25,25,25,25,26,26,26,26,26,26,26,26,27,27,27,27,27,27,27,27,27,28,28,28,28,28,28,28,28,28,29}
};

/*
 *
 *   黑白：文件头(14字节)+信息头(40字节)+2个调色板(共8字节)+Height(图像高度)*(Width+8-Width%8)/8

     16色：文件头(14字节)+信息头(40字节)+16个调色板(共64字节)+Height(图像高度)*(Width+4-Width%4)/2

     256色：文件头(14字节)+信息头(40字节)+256个调色板(共1024字节)+Height(图像高度)*(Width+4-Width%4)

     16位色：文件头(14字节)+信息头(40字节)+Height(图像高度)*(Width+4-Width%4)*2 (由于每个像素由两个字节表示)

     24位色：文件头(14字节)+信息头(40字节)+Height(图像高度)*(Width+4-Width%4)*3 (由于每个像素由三个字节表示)
 * */

typedef struct
{ // BitMapInfoHeader
    DWORD   biSize;   //Windows:28H Byte=40Word
    LONG    biWidth; //Width of Bitmap image
    LONG    biHeight;    //Height of Bitmap image
    WORD    biPlanes;    //1
    WORD    biBitCount;  //Depth of clor
    DWORD   biCompression;    //0-no compress，1-RLE8 compress，2-RLE4 compress，3表示每个像素值由指定的掩码决定
    DWORD   biSizeImage;
    LONG    biXPelsPerMeter;    // 3780
    LONG    biYPelsPerMeter;    // 3780
    DWORD   biClrUsed;
    DWORD   biClrImportant;
}BITMAPINFOHEADER;


typedef struct
{ // bmfh
    WORD    bfType; //"BM",0x424D
    DWORD   bfSize; //size of bmp file
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;  //(bfsize+bisize)
}BITMAPFILEHEADER;

//调色板
typedef struct
{
    unsigned char rgbBlue; //该颜色的蓝色分量  
    unsigned char rgbGreen; //该颜色的绿色分量  
    unsigned char rgbRed; //该颜色的红色分量  
    unsigned char rgbReserved; //保留值  
} RGBQuad;



/**
 * @name: ReadBmp
 * @msg: ReadBitMap
 * @param char *filepath        读取文件的路径
 *        BITMAPFILEHEADER *bmf 与文件有关的信息
 *        BITMAPINFOHEADER *bmi 与图片有关的信息
 * @return 数据指针
 */
BYTE* ReadBmp(BYTE* filepath, BITMAPFILEHEADER* bmf, BITMAPINFOHEADER* bmi);

/**
 * @name:WriteBmp
 * @msg: Wtite BMP file data
 * @param char *filepath        target file path
 *        BYTE *imgData         image data
 *        BITMAPFILEHEADER *bmf file header data
 *        BITMAPINFOHEADER *bmi image info data
 *        PICTYPE pt            图片类型
 * @return 0 (right) or -1(something wrong)
 */
BYTE WriteBmp(BYTE* filepath, BYTE* imgData, BITMAPFILEHEADER* bmf, BITMAPINFOHEADER* bmi, PICTYPE pt);

void PrintBmpFileHeader(BITMAPFILEHEADER* bmfh);
void PrintBmpInfoHeader(BITMAPINFOHEADER* bmih);


//默认采用四位精度表示
//Grey= (R*4 + G*10 + B*2) >> 4
//Grey = (R*1224 + G*2405 + B*467) >> 12
//site 精确度
void Li_Arr_bgr_gray(LiArr* src, LiArr* dst, LONG width, LONG height, BYTE site);

#endif // !__BitMapPicture_H__
