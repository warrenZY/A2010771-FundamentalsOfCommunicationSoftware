#define _CRT_SECURE_NO_WARNINGS 1

/*
@auther 2022210226-–§‘Û”Ó
@date 2024.4.13

=================Add Border======================
Input "./tsk3_source.bmp"
Output "./tsk3_AddedBorder.bmp"

=================Gray Image======================
Input "./tsk3_source.bmp"
Output "./tsk3_GrayImage.bmp"

=================Histogram Image=================
Input: "./tsk3_source.bmp"
Output: "./tsk3_HistogramImage.bmp"

=================Pseudocolor Image==============
Input: "./tsk3_source.bmp"
Output: "./tsk3_PseudocolorImage.bmp"

=================Bitmap Scale====================
Input: "./tsk3_source.bmp"
Output: "./tsk3_Scaled.bmp"

=================Bitmap Rotation=================
Input "./tsk3_source.bmp"
Output DstRotatedFilePath "./tsk3_Rotated.bmp"

=================Edge Detection==================
Input: "./tsk3_EdgeImage.bmp"
Output: edge
*/

#include "BitmapAlterService.h"
#include "WangHaiBo.h"
#include "stdio.h"
#include "stdlib.h"

#define KeyValue_ESC 27
unsigned char Menu[][40] = {
    "0-ÃÌº”±ﬂøÚ(–§‘Û”Ó)","1-ª“∂»ªØ(–§‘Û”Ó)","2-÷±∑ΩÕº(ÕÙ∫£≤®)","3-Œ±…´Õº(ÕÙ∫£≤®)","4-Àı∑≈ÕºœÒ(–§‘Û”Ó)","5-–˝◊™ÕºœÒ(–§‘Û”Ó)","6-±ﬂ‘µºÏ≤‚(–§‘Û”Ó)"
};

/*
* @brief convert Color BMP to type select
* @author –§‘Û”Ó
*/
int main() {
    int keyValue = -1;
    int command = 1;
    while (command)
    {
        int index = 0;
        printf("%sOnly Support 24Bit Color BMP files.\n", InformationMsg);
        for (int hIndex = 0; strlen(Menu[index + 1]) != 0; hIndex++) {
            for (int wIndex = 0; wIndex < 3; wIndex++) {
                index = wIndex + hIndex * 3;
                printf("\t%s\t", Menu[index]);
            }
            printf("\n");
        }
        printf("%sEnter number to active function, e/ESC or other to quit.\n", InformationMsg);
        
        keyValue = getch();
        printf("User input: %c\n\n", keyValue);
        switch (keyValue)
        {
        case '0':BMPAddColorBorder(SrcBmpFilePath, DstAddBorderFilePath); break;
        case '1':BMPRGB2Gray(SrcBmpFilePath, DstGrayFilePath); break;
        case '2':
        case '3':WangHaiBo(); break;
        case '4':BMPScale(); break;
        case '5':BMPRotation(); break;
        case '6':BMPEdgeDetection(SrcEdgeDetectionFilePath, DstEdgeDetectionFilePath); break;
        case 'e':
        case KeyValue_ESC:command = 0; break;

        default:
            break;
        }
        
        
        
        
        

    }


    printf("%sProgram exits.\n", InformationMsg);
	return 0;
}

