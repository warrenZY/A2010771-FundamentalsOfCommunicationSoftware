#include "PacketEth.h"
#include <stdio.h>
#include <stdlib.h>
#include "CRC8.h"

int PrintEthData(EthernetFrame* parsedFrame);

int main(int argc, char* argv[]) {




   if (argc != 2) {
        printf("Invalid input, please input eth filename\n");
        return -1;
    }

    char** ptr;
    for (ptr = argv; *ptr != NULL; ptr++)
        printf("Parameter: %s\n", *ptr);
    printf("\n");
    
    //Calculate CRC table
    //CreateCRCTable();

    uint16_t lSize, lCount;
    FILE* srcFrameStream = fopen(argv[1], "rb");
    if (!srcFrameStream) {
        perror("invalid eth-file");
        printf("\n");
        return -1;
    }
   
    fseek(srcFrameStream, 0l, SEEK_END);
    lSize = ftell(srcFrameStream);
    printf("Size of input data: %hu\n\n", lSize);
    lCount = lSize / sizeof(uint8_t);
    uint8_t* rawFrame = (uint8_t*)malloc(lSize);

    rewind(srcFrameStream);
    fread(rawFrame, sizeof(uint8_t), lCount, srcFrameStream);
    size_t frameLength = lSize;// 从某个地方获取帧长度 
    fclose(srcFrameStream);

    uint16_t readLength = 0;
    EthernetFrame parsedFrame;
    while(readLength < frameLength){
        int result = CheckEthHeader(rawFrame+readLength, lCount-readLength, &parsedFrame);
        if (result < 0) {
            printf("Error parsing Ethernet frame\n");
            return -1;
        }
        PrintEthData(&parsedFrame);
        
        
        printf("\n");
        readLength += parsedFrame.packetLength;
    }
}   


int PrintEthData(EthernetFrame* parsedFrame) {
    uint8_t curData;
    
#ifndef NDEBUG
    printf("Binary Data:\t");
    for (int index = 0; index < parsedFrame->packetLength - parsedFrame->dataOffset; index++) {
        printf("%.2X ", parsedFrame->data[index]);
    }
    printf("\n");
#endif // !NDEBUG
    printf("Decoded Data:\t");
    for (int index = 0; index < parsedFrame->packetLength - parsedFrame->dataOffset - 1; index++) {
        curData = parsedFrame->data[index];
        if (curData == '\0')
            curData = 0x20;
        printf("%c", curData);
        
    }
    //uint8_t nCheck=0x00;
    //for (int index = 0; index < parsedFrame->packetLength; index++) {
    //    curData = parsedFrame->destination[index];
    //    printf("%.2X ", curData);
    //    if (index == 0) {
    //        nCheck = curData;
    //    }
    //    else {
    //        nCheck = CheckCRC8MAXIM(nCheck, curData);
    //    }

    //}
    //for (int index = 0; index < 6; index++) {
    //    curData = parsedFrame->data[index];
    //    if (index == 0) {
    //        nCheck = curData;
    //    }
    //    else {
    //        nCheck = CheckCRC8MAXIM(nCheck, curData);
    //    }

    //}
    //for (int index = 0; index < 2; index++) {
    //    curData = parsedFrame->type[index];
    //    if (index == 0) {
    //        nCheck = curData;
    //    }
    //    else {
    //        nCheck = CheckCRC8MAXIM(nCheck, curData);
    //    }

    //}
    //


    //printf("\nFCS %.2X", nCheck);

    
    
    printf("\n");
}
/*
int FCS = check_fcs(&parsedFrame, 6, parsedFrame.destination[0]);
        if (FCS == 0)
            printf("FCS:\tTRUE\n");
        else
            printf("FCS:\tFlase\n");




for (int index = 0; index < lCount-parsedFrame.rawOffset ; index++) {
        if (parsedFrame.data[index] == '\0')
            parsedFrame.data[index] = 0x20;
        printf("%.2X", parsedFrame.data[index]);
    }

for (int i = 0; i < lCount; i++) {
        printf("%c", (rawFrame[i]));
    }
rewind(srcFrameStream);
       for (int j = 0; j < 7; j++) {
        fread(rawFrame, sizeof(uint8_t), 1, srcFrameStream);
        if (rawFrame != 0xAA) {
            j = -1;
        }
        else {
            printf("found 0xAA\n!");
        }
    }

    for (int index = 0; index < parsedFrame->packetLength - parsedFrame->dataOffset; index++) {
        curData = parsedFrame->data[index];
        if (index == 0) {
            nCheck = curData;
        }
        else {
            nCheck = CheckCRC8MAXIM(nCheck, curData);
        }

    }
*/