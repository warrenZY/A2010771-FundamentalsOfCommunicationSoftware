#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wingdi.h>
#include "BitmapAlterService.h"
#define PATH_READ SrcBmpFilePath
#define PATH_WRITE_GREY "./temp.bmp"  //生成图像路径：灰度图
#define PATH_EXCEL DstHistogramFilePath //生成灰度统计表格
#define PATH_WRITE_GREYTOCOLOR DstPseudocolorFilePath //生成伪彩色图

unsigned char* pData;//存储图像数据
RGBQUAD* pRGBQUAD;//8位调色板

/*
    FILE* fp:BMP图像文件指针
    BITMAPFILEHEADER* bfheader:缓存文件头信息
    BITMAPINFOHEADER* biheader：缓存信息头信息
    unsigned char* pData:存储图像数据
    int flag:打印允许标志位
    返回值：图像数据数组地址
    使用fread函数直接读取一个结构体大小的数据，因为其大小固定，故数据可以直接对应上结构体；并用Printf函数直接输出结果
*/
unsigned char* readBMP(FILE* fp, BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, int flag);//读取BMP文件的文件头和信息头并输出
/*
    将十进制整形转为十六进制后按照ASCII码输出两个字符
    返回值：返回转换后的字符串地址
*/
char* hex2str(int a, char* s);//双字节转换
/*
    FILE* fp:BMP图像文件指针
    BITMAPFILEHEADER* bfheader:原文件头信息
    BITMAPINFOHEADER* biheader：原信息头信息
    unsigned char* pData:存储图像数据
    BITMAPINFOHEADER* biheader_write:存储修改后的文件头信息
    BITMAPFILEHEADER* bfheader_write:存储修改后信息头信息
    返回值：成功返回1，失败返回0
    通用创建BMP文件，写入文件头，信息头，图像数据等信息
*/
int writeBMP(FILE* fp, BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData);//写入文件头，信息头，图像
/*
    FILE* fp:BMP图像文件指针
    BITMAPFILEHEADER* bfheader:原文件头信息
    BITMAPINFOHEADER* biheader：原信息头信息
    unsigned char* pData:存储图像数据
    BITMAPINFOHEADER* biheader_write:存储修改后的文件头信息
    BITMAPFILEHEADER* bfheader_write:存储修改后信息头信息
    返回值：成功返回1，失败返回0
    通用创建BMP文件，写入文件头，信息头，图像数据等信息
*/

int getArray(unsigned char*** p, int row, int col);//根据长宽，生成二维数组
/*
    BITMAPFILEHEADER* bfheader:文件头信息
    BITMAPINFOHEADER* biheader：信息头信息
    unsigned char* pData:存储图像数据
    char*** p:指向创建的二维数组的地址
    返回值：成功返回1，失败返回0
    根据公式加权计算灰度值并存入二维数组
*/
int readGrey(BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData, unsigned char*** p);//获取灰度值
/*
    BITMAPFILEHEADER* bfheader:文件头信息
    BITMAPINFOHEADER* biheader：信息头信息
    unsigned char* pData:存储图像数据
    char*** p:指向创建的二维数组的地址
    返回值：成功返回1，失败返回0
    根据二维数组灰度值修改图像数据
*/
int BMP_Grey(BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData, unsigned char*** p);//修改图像数据：灰度图
/*
    BITMAPFILEHEADER* bfheader:文件头信息
    BITMAPINFOHEADER* biheader：信息头信息
    char*** p:指向创建的二维数组的地址
    int a[256]:存储灰度统计量
    返回值：成功返回1，失败返回0
    统计灰度值对应像素个数
*/
int greyExcel(BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char*** p, int a[256]);//统计灰度值数量
/*
    int a[256]:存储灰度统计量
    返回值：成功返回1，失败返回0
    写入统计的灰度值像素个数于EXCEL中
*/
int printExcel(int a[256]);//输出数据到excel中
/*
    BITMAPFILEHEADER* bfheader:文件头信息
    BITMAPINFOHEADER* biheader：信息头信息
    unsigned char* pData:存储图像数据
    char*** p:指向创建的二维数组的地址
    返回值：成功返回1，失败返回0
    根据分量叠加公式由灰度图转为伪彩色图
*/
int greyToColor(BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData, unsigned char*** p);//由灰度图转为伪彩色图
// 定义用于计算梯度的函数
int gradient_x(int x, int y, unsigned char*** grey, int width);
int gradient_y(int x, int y, unsigned char** grey, int height);
int gradient_z(int x, int y, unsigned char*** grey, int height, int width);
int writeBMP2(FILE* fp, BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData);
int mapGrayToIndex(unsigned char gray);
int writeBMP3(FILE* fp, BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData);
int readGrey_2(BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData_EDGE, unsigned char*** p);//获取灰度值



unsigned char* readBMP(FILE* fp, BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, int flag)//读取BMP文件的文件头和信息头并输出
{
    char s[10];
    if (!fp)
        exit(-1);
    fseek(fp, 0, SEEK_SET);//定位到文件开始处
    fread(bfheader, sizeof(BITMAPFILEHEADER), 1, fp);//读取文件头14字节进bfheader结构体中
    if (bfheader->bfType != 0x4D42)
    {
        printf("\n此文件不是BMP文件，读取失败！\n");
        exit(-1);
    }
    fseek(fp, 14, SEEK_SET);
    fread(biheader, sizeof(BITMAPINFOHEADER), 1, fp);//读取信息头进biheader结构体中
    hex2str(19778, s);//整形按双字节以ASCII码转为字符串
    if (flag)//标志为1时允许打印
    {
        /*打印相关文件头信息*/
        printf("\n以下为位图文件头信息：\n");
        printf("位图文件类型:%c%c\n", s[0], s[1]);
        printf("位图文件大小:%d字节\n", bfheader->bfSize);
        printf("位图文件数据偏移:%d字节\n", bfheader->bfOffBits);
        /*打印相关信息头信息*/
        printf("\n位图信息头信息如下:\n");
        printf("信息头占用大小:%d字节\n", biheader->biSize);
        printf("BMP图像宽度:%d像素\n", biheader->biWidth);
        printf("BMP图像高度:%d像素\n", biheader->biHeight);
        printf("BMP图像颜色位数:%d位\n", biheader->biBitCount);
        printf("BMP图像压缩类型:%d\n", biheader->biCompression);
        printf("BMP图像实际数据大小:%d字节\n", bfheader->bfSize);
    }

    /*数据读取*/
    if (biheader->biBitCount == 8)//对8位深图像单独处理
    {
        RGBQUAD* pRGBQUAD = (RGBQUAD*)malloc(sizeof(RGBQUAD) * 256);//为调色板申请空间
        fread(pRGBQUAD, sizeof(RGBQUAD), 256, fp);//读取256个调色板数据
    }
    int lineByte = (biheader->biWidth * biheader->biBitCount / 8 + 3) / 4 * 4;//计算实际位宽；+3后除4乘4是按4字节对齐计算位宽，如位宽7，则计算后得到实际位宽是8
    unsigned char* pData = (unsigned char*)malloc(sizeof(unsigned char) * biheader->biHeight * lineByte);//申请大小为实际图像数据大小的空间
    fseek(fp, bfheader->bfOffBits, SEEK_SET);//定位到数据区
    fread(pData, sizeof(unsigned char), biheader->biHeight * lineByte, fp);
    fclose(fp);
    return pData;
}
char* hex2str(int a, char* s)//双字节转换
{
    int a1 = a / 256;
    int a2 = a % 256;
    s[0] = (char)a2;
    s[1] = (char)a1;
    return s;
}

int mapGrayToIndex(unsigned char gray) {
    if (gray < 128)
        return 0;
    else
        return 255;
}
int writeBMP(FILE* fp, BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData)//写入文件头，信息头，图像数据生成BMP文件
{
    if (!fp)
        exit(-1);
    int colorTableSize = 0;//初始化调色板大小
    if (biheader->biBitCount == 8)
    {
        colorTableSize = 1024;//8位位深调色板大小1024字节
    }
    BITMAPFILEHEADER bfheader_write;
    BITMAPINFOHEADER biheader_write;

    int lineByte = (biheader->biWidth * biheader->biBitCount / 8 + 3) / 4 * 4;//计算实际位宽；+3后除4乘4是按4字节对齐计算位宽，如位宽7，则计算后得到实际位宽是8
    /*构建文件头信息*/
    bfheader_write.bfType = 0x4D42;//BM
    bfheader_write.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize + lineByte * biheader->biHeight;//设置文件大小：文件头+信息头+调色板+图像大小
    bfheader_write.bfReserved1 = bfheader_write.bfReserved2 = 0;
    bfheader_write.bfOffBits = 54 + colorTableSize;//设置偏移量或bfheader_write.bfOffBits=bfheader.bfOffBits
    /*构建信息头信息*/
    biheader_write.biBitCount = biheader->biBitCount;//设置位深
    biheader_write.biClrImportant = biheader_write.biClrUsed = biheader_write.biCompression = 0;
    biheader_write.biHeight = biheader->biHeight;//设置位高
    biheader_write.biWidth = biheader->biWidth;//设置位宽
    biheader_write.biPlanes = 1;
    biheader_write.biSize = 40;//信息头长度
    biheader_write.biSizeImage = bfheader_write.bfSize;
    biheader_write.biXPelsPerMeter = 0;//设置X分辨率
    biheader_write.biYPelsPerMeter = 0;//设置Y分辨率

    fwrite(&bfheader_write, sizeof(BITMAPFILEHEADER), 1, fp);//将文件头数据写入文件，写入一次大小为BITMAPFILEHEADER的数据
    fwrite(&biheader_write, sizeof(BITMAPINFOHEADER), 1, fp);//信息头数据写入文件，写入一次大小为BITMAPINFOHEADER的数据

    if (biheader->biBitCount == 8)//若8位色深，则还有调色板需要写入
        fwrite(pRGBQUAD, sizeof(RGBQUAD), 256, fp);//8位调色板有256个颜色



    fwrite(pData, sizeof(unsigned char), biheader->biHeight * lineByte, fp);//写入图像数据

    fclose(fp);
    return 1;
}
int writeBMP2(FILE* fp, BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData)//写入文件头，信息头，图像数据生成BMP文件
{
    if (!fp)
        exit(-1);
    int colorTableSize = 0;//初始化调色板大小
    if (biheader->biBitCount == 8)
    {
        colorTableSize = 1024;//8位位深调色板大小1024字节
    }
    BITMAPFILEHEADER bfheader_write;
    BITMAPINFOHEADER biheader_write;

    int lineByte = (biheader->biWidth * biheader->biBitCount / 8 + 3) / 4 * 4;//计算实际位宽；+3后除4乘4是按4字节对齐计算位宽，如位宽7，则计算后得到实际位宽是8
    /*构建文件头信息*/
    bfheader_write.bfType = 0x4D42;//BM
    bfheader_write.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize + lineByte * biheader->biHeight;//设置文件大小：文件头+信息头+调色板+图像大小
    bfheader_write.bfReserved1 = bfheader_write.bfReserved2 = 0;
    bfheader_write.bfOffBits = 54 + colorTableSize;//设置偏移量或bfheader_write.bfOffBits=bfheader.bfOffBits
    /*构建信息头信息*/
    biheader_write.biBitCount = biheader->biBitCount;//设置位深
    biheader_write.biClrImportant = biheader_write.biClrUsed = biheader_write.biCompression = 0;
    biheader_write.biHeight = biheader->biHeight;//设置位高
    biheader_write.biWidth = biheader->biWidth;//设置位宽
    biheader_write.biPlanes = 1;
    biheader_write.biSize = 40;//信息头长度
    biheader_write.biSizeImage = bfheader_write.bfSize;//设置图像大小
    biheader_write.biXPelsPerMeter = 0;//设置X分辨率
    biheader_write.biYPelsPerMeter = 0;//设置Y分辨率

    fwrite(&bfheader_write, sizeof(BITMAPFILEHEADER), 1, fp);//将文件头数据写入文件，写入一次大小为BITMAPFILEHEADER的数据
    fwrite(&biheader_write, sizeof(BITMAPINFOHEADER), 1, fp);//信息头数据写入文件，写入一次大小为BITMAPINFOHEADER的数据

    if (biheader->biBitCount == 8)//若8位色深，则还有调色板需要写入
        fwrite(pRGBQUAD, sizeof(RGBQUAD), 256, fp);//8位调色板有256个颜色



    fwrite(pData, sizeof(unsigned char), biheader->biHeight * lineByte, fp);//写入图像数据

    fclose(fp);
    return 1;
}


int writeBMP3(FILE* fp, BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData) {
    if (!fp) {
        exit(-1);
    }

    int colorTableSize = 0; // 调色板大小
    if (biheader->biBitCount == 8) {
        colorTableSize = 1024; // 8位位深调色板大小
    }
    else if (biheader->biBitCount == 1) {
        colorTableSize = sizeof(RGBQUAD) * 2; // 1位位深调色板大小
    }

    int lineByte = (biheader->biWidth * biheader->biBitCount / 8 + 3) / 4 * 4; // 计算实际位宽

    /* 构建文件头信息 */
    BITMAPFILEHEADER bfheader_write = {
        .bfType = 0x4D42, // 'BM'
        .bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize + lineByte * abs(biheader->biHeight),
        .bfReserved1 = 0,
        .bfReserved2 = 0,
        .bfOffBits = 54 + colorTableSize
    };

    /* 构建信息头信息 */
    BITMAPINFOHEADER biheader_write = {
        .biSize = 40, // 信息头长度
        .biWidth = biheader->biWidth,
        .biHeight = biheader->biHeight,
        .biPlanes = 1,
        .biBitCount = biheader->biBitCount,
        .biCompression = 0,
        .biSizeImage = lineByte * abs(biheader->biHeight),
        .biXPelsPerMeter = 0,
        .biYPelsPerMeter = 0,
        .biClrUsed = 0,
        .biClrImportant = 0
    };

    fwrite(&bfheader_write, sizeof(BITMAPFILEHEADER), 1, fp); // 写入文件头
    fwrite(&biheader_write, sizeof(BITMAPINFOHEADER), 1, fp); // 写入信息头

    // 如果是1位位深，则需要写入调色板
    if (biheader->biBitCount == 1) {
        RGBQUAD colorTable[2] = {
            { 0x00, 0x00, 0x00, 0x00 }, // 黑色
            { 0xFF, 0xFF, 0xFF, 0x00 }  // 白色
        };
        fwrite(colorTable, sizeof(RGBQUAD), 2, fp); // 写入调色板
    }

    fwrite(pData, sizeof(unsigned char), biheader->biHeight * lineByte, fp); // 写入图像数据

    fclose(fp);
    return 1;
}



int getArray(unsigned char*** p, int row, int col)//根据长宽，生成二维数组
{
    int i;
    *p = (unsigned char**)malloc(sizeof(unsigned char*) * row);//申请row行空间，*p存储所有行的首地址
    memset(*p, 0, sizeof(unsigned char*) * row);//每行首地址清零
    if (p == NULL)
        exit(-1);
    for (i = 0; i < row; i++)
    {
        (*p)[i] = (unsigned char*)malloc(sizeof(unsigned char) * col);//当前行申请col列空间，*p[i]存储该行的首地址
        memset((*p)[i], 0, sizeof(unsigned char) * col);//当前行所有列数据清零
        if ((*p)[i] == NULL)
            exit(-1);
    }
    return 1;
}


int readGrey(BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData, unsigned char*** p)//获取灰度值
{
    int lineByte = (biheader->biWidth * biheader->biBitCount / 8 + 3) / 4 * 4;//计算实际位宽
    float grey;
    for (int i = 0; i < biheader->biHeight; i++)//循环行
    {
        int col = 0;//声明图像列数
        for (int j = 0; j < 3 * biheader->biWidth; j++)//循环列
        {
            unsigned char B = *(pData + i * lineByte + j);//获取B值
            j++;
            unsigned char G = *(pData + i * lineByte + j);//获取G值
            j++;
            unsigned char R = *(pData + i * lineByte + j);//获取R值
            grey = 0.114 * B + 0.587 * G + 0.299 * R;
            (*p)[i][col] = grey;//加权计算灰度值
            col++;
        }
    }
    return 1;
}



int BMP_Grey(BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData, unsigned char*** p)//修改图像数据：灰度图
{
    int lineByte = (biheader->biWidth * biheader->biBitCount / 8 + 3) / 4 * 4;//计算实际位宽
    for (int i = 0; i < biheader->biHeight; i++)//循环行
    {
        int col = 0;//声明图像列数
        for (int j = 0; j < 3 * biheader->biWidth; j++)//循环列
        {
            *(pData + i * lineByte + j) = (*p)[i][col];//获取B值
            j++;
            *(pData + i * lineByte + j) = (*p)[i][col];//获取G值
            j++;
            *(pData + i * lineByte + j) = (*p)[i][col];//获取R值
            col++;
        }
    }
    return 1;
}

int greyExcel(BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char*** p, int a[256])//统计灰度值数量
{
    for (int i = 0; i < 256; i++)//灰度值从0到255
    {
        int count = 0;//声明计数器
        for (int j = 0; j < biheader->biHeight; j++)//遍历数组行
        {
            for (int k = 0; k < biheader->biWidth; k++)//遍历数组列
            {
                if ((*p)[j][k] == i)//遍历数组查询当前灰度值是否相同
                    count++;
            }
        }
        a[i] = count;//存入该灰度值的像素数量
    }
    return 1;
}

int printExcel(int a[256])//输出数据到excel中
{
    FILE* fp = NULL;
    fp = fopen(PATH_EXCEL, "w+");//创建指定路径excel文件
    fprintf(fp, "%s,%s\n", "灰度值", "像素数量");//写入标题position和power
    if (!fp)
        return 0;
    for (int i = 0; i < 256; i++)//写入灰度值对应像素个数
    {
        fprintf(fp, "%d,%d\n", i, a[i]);
    }

    fclose(fp);
    return 1;
}

int greyToColor(BITMAPFILEHEADER* bfheader, BITMAPINFOHEADER* biheader, unsigned char* pData, unsigned char*** p)//由灰度图转为伪彩色图
{
    int lineByte = (biheader->biWidth * biheader->biBitCount / 8 + 3) / 4 * 4;//计算实际位宽
    for (int i = 0; i < biheader->biHeight; i++)//循环行
    {
        int col = 0;//声明灰度数组列数
        for (int j = 0; j < biheader->biWidth * 3; j++)//循环列
        {
            /*仿OpenCV中RainBow映射*/
            if ((*p)[i][col] <= 30)//区间1叠加：每64灰度值划分一个区间
            {
                *(pData + i * lineByte + j) = 255;//叠加B分量
                j++;
                *(pData + i * lineByte + j) = 0;//叠加G分量
                j++;
                *(pData + i * lineByte + j) = 125 - 4.16 * (*p)[i][col];//叠加R分量
            }
            else if ((*p)[i][col] > 30 && (*p)[i][col] <= 87)//区间2叠加
            {
                *(pData + i * lineByte + j) = 255;//叠加B分量
                j++;
                *(pData + i * lineByte + j) = ((*p)[i][col] - 30) * 4.47;//叠加G分量
                j++;
                *(pData + i * lineByte + j) = 0;//叠加R分量
            }
            else if ((*p)[i][col] > 87 && (*p)[i][col] <= 142)//区间2叠加
            {
                *(pData + i * lineByte + j) = 255 - 4.64 * ((*p)[i][col] - 87);//叠加B分量
                j++;
                *(pData + i * lineByte + j) = 255;//叠加G分量
                j++;
                *(pData + i * lineByte + j) = 0;//叠加R分量
            }
            else if ((*p)[i][col] > 142 && (*p)[i][col] <= 201)//区间3叠加
            {
                *(pData + i * lineByte + j) = 0;//叠加B分量
                j++;
                *(pData + i * lineByte + j) = 255;//叠加G分量
                j++;
                *(pData + i * lineByte + j) = 4.32 * ((*p)[i][col] - 142);//叠加R分量
            }
            else if ((*p)[i][col] > 201)//区间4叠加
            {
                *(pData + i * lineByte + j) = 0;//叠加B分量
                j++;
                *(pData + i * lineByte + j) = 255 - 4.72 * ((*p)[i][col] - 201);//叠加G分量
                j++;
                *(pData + i * lineByte + j) = 255;//叠加R分量
            }
            col++;

        }
    }

    return 1;
}







int WangHaiBo()
{
    FILE* fp = fopen(PATH_READ, "rb");
    BITMAPFILEHEADER bfheader_read;//声明文件头结构体
    BITMAPINFOHEADER biheader_read;//声明信息头结构体
    unsigned char* pData = readBMP(fp, &bfheader_read, &biheader_read, 1);//读取图像返回图像数据
    /*生成灰度图*/
    fp = fopen(PATH_READ, "rb");
    pData = readBMP(fp, &bfheader_read, &biheader_read, 0);//读取图像返回图像数据

    unsigned char** grey;//声明二维数组，存储灰度值
    if (!getArray(&grey, biheader_read.biHeight, biheader_read.biWidth))//生成二维数组
        exit(-1);
    if (!readGrey(&bfheader_read, &biheader_read, pData, &grey))//读取灰度值并存入grey数组中
        exit(-1);
    if (!BMP_Grey(&bfheader_read, &biheader_read, pData, &grey))//修改图像的存储数据：灰度图
        exit(-1);
    fp = fopen(PATH_WRITE_GREY, "wb");
    if (!writeBMP(fp, &bfheader_read, &biheader_read, pData))//生成BMP文件
        exit(-1);
    /*生成灰度直方图*/
    int greyArray[256];
    if (!greyExcel(&bfheader_read, &biheader_read, &grey, greyArray))
        exit(-1);
    if (!printExcel(greyArray))
        exit(-1);
    /*由公式叠加生成伪彩色图*/
    if (!greyToColor(&bfheader_read, &biheader_read, pData, &grey))//叠加RGB分量修改图像数据
        exit(-1);
    fp = fopen(PATH_WRITE_GREYTOCOLOR, "wb");
    if (!writeBMP(fp, &bfheader_read, &biheader_read, pData))//生成BMP文件
        exit(-1);
    fclose(fp);

    return 0;
}




