
#ifndef __PACKET_ETH_H__
#define __PACKET_ETH_H__
#include <stdint.h>  
typedef struct {
    uint8_t destination[6];  // 目的MAC地址  
    uint8_t source[6];       // 源MAC地址  
    uint8_t type[2];           // 类型字段  
    uint8_t data[1500];      // data
    uint8_t crc8;            //  CRC Result 0x00 for TRUE
    uint16_t packetLength;    //raw length
    uint16_t dataOffset;
} EthernetFrame;


int CheckEthHeader(const uint8_t* rawFrame, size_t inputLength, EthernetFrame* parsedFrame);


#endif
