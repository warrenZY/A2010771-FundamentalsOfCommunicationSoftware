
#ifndef BMP_H_INCLUDED
#define BMP_H_INCLUDED
#include "stdio.h"
#include "LiteCV.h"
#include "Windows.h"

 /*
  *
  *   黑白：文件头(14字节)+信息头(40字节)+2个调色板(共8字节)+Height(图像高度)*(Width+8-Width%8)/8

      16色：文件头(14字节)+信息头(40字节)+16个调色板(共64字节)+Height(图像高度)*(Width+4-Width%4)/2

      256色：文件头(14字节)+信息头(40字节)+256个调色板(共1024字节)+Height(图像高度)*(Width+4-Width%4)

      16位色：文件头(14字节)+信息头(40字节)+Height(图像高度)*(Width+4-Width%4)*2 (由于每个像素由两个字节表示)

      24位色：文件头(14字节)+信息头(40字节)+Height(图像高度)*(Width+4-Width%4)*3 (由于每个像素由三个字节表示)
  * */


/*
*bfType：位图文件类型，必须是0x424D，即字符串“BM”，也就是说，所有的“*.bmp”文件的头两个字节都是“BM”。
*bfSize：位图文件大小，包括这14个字节。
*bfReserved1, bfReserved2：Windows保留字，暂不用。
*bfOffBits：(bfsize+bisize)从文件头到实际的位图数据的偏移字节数
*/


//调色板


/**
 * @name: Read_bmp
 * @msg: 读取一个BMP图片
 * @param char *filepath        读取文件的路径
 *        BITMAPFILEHEADER *bmf 与文件有关的信息
 *        BITMAPINFOHEADER *bmi 与图片有关的信息
 * @return 数据指针
 */
BYTE* Read_bmp(BYTE* filepath, BITMAPFILEHEADER* bmf, BITMAPINFOHEADER* bmi);
/**
 * @name:
 * @msg: 写BMP图片，只负责写数据，没有图片的转换功能
 * @param char *filepath        读取文件的路径
 *        BYTE *imgData         读到的数据
 *        BITMAPFILEHEADER *bmf 与文件有关的信息
 *        BITMAPINFOHEADER *bmi 与图片有关的信息
 *        PICTYPE pt            图片类型
 * @return 0 (right) or -1(something wrong)
 */
BYTE Write_bmp(BYTE* filepath, BYTE* imgData, BITMAPFILEHEADER* bmf, BITMAPINFOHEADER* bmi, PICTYPE pt);

void Print_bmp_FileHeader(BITMAPFILEHEADER* bmfh);
void Print_bmp_InfoHeader(BITMAPINFOHEADER* bmih);

#endif 
