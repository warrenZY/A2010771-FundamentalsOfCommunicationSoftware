#include "PacketEth.h"
#include <stdio.h>  
#include <string.h>

#include "Message.h"
#define Log(msg)  printf("%s%s: %s (%s,%d,%s)\n",LogMsg,"EthPacketService",msg, __FILE__, __LINE__ ,__FUNCTION__)





int CheckEthHeader(const uint8_t* rawFrame, size_t inputLength, EthernetFrame* parsedFrame) {

    for (int index = 0; index < 54; index++) {
        printf("=");
    }
    printf("\n");

    uint16_t frameLength = 0;
    int offset = 0;
    int numOf0xAA = 0;
    while (offset < inputLength - 8) {
        for (offset = 0; offset < inputLength - 8; offset++) {
            if (rawFrame[offset] == 0xAA) {
                numOf0xAA++;
                //check 7*0xAA
                for (int j = 1; j < 7; j++) {
                    if (rawFrame[offset + j] == 0xAA) {
                        numOf0xAA++;
                    }
                    else {
                        offset += numOf0xAA;
                        numOf0xAA = 0;
                    }
                }
                break;
            }

        }
        if (numOf0xAA == 7) {
            printf("Preamble code:\t%.2X %.2X %.2X %.2X %.2X %.2X %.2X\n",
                rawFrame[offset], rawFrame[offset + 1], rawFrame[offset + 2], rawFrame[offset + 3],
                rawFrame[offset + 4], rawFrame[offset + 5], rawFrame[offset + 6]);
            offset += 7;
            break;
        }
    }
    if (numOf0xAA == 7 && rawFrame[offset] == 0xAB) {
        printf("Eth header:\t%.2X\n", rawFrame[offset]);
        offset++;
    }
    else {
        printf("%sNo legal input!\n",ErrorMsg);
        return -1;
    }
    memcpy(parsedFrame->destination, rawFrame + offset, 6); //dest MAC
    printf("Dst MAC addr:\t%.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
        parsedFrame->destination[0], parsedFrame->destination[1], parsedFrame->destination[2],
        parsedFrame->destination[3], parsedFrame->destination[4], parsedFrame->destination[5]);
    memcpy(parsedFrame->source, rawFrame + offset + 6, 6);  //Source MAC
    printf("Src MAC addr:\t%.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
        parsedFrame->source[0], parsedFrame->source[1], parsedFrame->source[2],
        parsedFrame->source[3], parsedFrame->source[4], parsedFrame->source[5]);
    memcpy(parsedFrame->type, rawFrame + offset + 12, 2);
    printf("Type phrase:\t%.2X %.2X", parsedFrame->type[0], parsedFrame->type[1]);
    if (parsedFrame->type[0]==0x08) {
        char* str;
        switch (parsedFrame->type[1])
        {
        case 0x00: str = "IP"; break;
        case 0x06: str = "ARP"; break;
        case 0x08: str = "Frame Relay ARP"; break;
        default:
            str = "Not recognize";
            break;
        }
        printf(" (%s)",str);
    }
    printf("\n");


    offset += 14;


    parsedFrame->dataOffset = offset;

    numOf0xAA = 0;
    while (offset < inputLength - 8) {
        for (offset; offset < inputLength; offset++) {
            if (rawFrame[offset] == 0xAA) {
                numOf0xAA++;
                //check 7*0xAA
                for (int j = 1; j < 7; j++) {
                    if (rawFrame[offset + j] == 0xAA) {
                        numOf0xAA++;
                    }
                    else {
                        offset += numOf0xAA;
                        numOf0xAA = 0;
                    }
                }
                break;
            }

        }
        if (numOf0xAA == 7) {
            break;
        }
    }
    if (numOf0xAA == 7 && rawFrame[offset + 7] == 0xAB) {
#ifndef NDEBUG
        Log("Find next header");
#endif // NDEBUG
        frameLength = offset;
    }
    else {
#ifndef NDEBUG
        Log("No next header found");
#endif // NDEBUG
        frameLength = inputLength;
    }
    parsedFrame->packetLength= frameLength;
    printf("Lenth of frame:\t%hu\n", frameLength);
    memcpy(parsedFrame->data, rawFrame + parsedFrame->dataOffset, parsedFrame->packetLength - parsedFrame->dataOffset);
    //memcpy(parsedFrame->crc8, rawFrame + parsedFrame->packetLength, 1);

    uint8_t curData = 0x00;
    uint8_t nCheck = 0x00;
    for (int index = 8; index < parsedFrame->packetLength; index++) {
        curData = rawFrame[index];
        //printf("%.2X ", curData);
        if (index == 0) {
            nCheck = curData;
        }
        else {
            nCheck = CheckCRC8MAXIM(nCheck, curData);
        }

    }
    //printf("\nFCS %.2X", nCheck);
    parsedFrame->crc8 = nCheck;
    if (nCheck == 0x00)
        printf("FCS:\tTrue\n");
    else
        printf("FCS:\tFalse\n");
    for (int index = 0; index < 54; index++) {
        printf("=");
    }
    printf("\n");
    return 0;
}

/*
    uint8_t computedCrc8 = crc8(rawFrame + 14, dataLength);

    // 复制数据字段（不包括CRC8值）
    memcpy(parsedFrame->data, rawFrame + 14, dataLength);

    // 验证CRC8值
    parsedFrame->crc8 = rawFrame[frameLength - 1]; // 假设CRC8在帧末尾
    if (computedCrc8 != parsedFrame->crc8) {
        return -2; // 错误：CRC8校验失败
    }

*/