#ifndef __OspfFlood_H__
#define __OspfFlood_H__
/*
* reference
* https://learnduty.com/network-techs/ospf-header-format-and-packets-types-explained-with-wireshark-captures/
*/

#include <stdlib.h>

#define NULL ((char *)0)

#define OSPF_VERSION_2 2
#define OSPF_AREA_ID 1
#define OSPF_AF_4 4
#define OSPF_VERSION_2_HEADER_LENGTH    24
#define OSPF_AUTH_NONE  0
#define OSPF_HELLO  1
#define OSPF_DB_DESC    2
#define OSPF_LS_REQ     3
#define OSPF_LS_UPD     4
#define OSPF_LS_ACK     5
#define OSPF_LS_BASE    OSPF_HELLO

#define OSPF_HELLO_INT 10
#define OSPF_HELLO_ROUTER_DEAD_INT 40
#define OSPF_NETWORK_MASK_24 0xffffff00 //225.255.255.0

typedef struct _valueString {
    unsigned int value;//ip address will be convert from BIN to HEX
    const char* strptr;//description
}valueString;

typedef struct _listNode {
    unsigned int value;
    struct list* next;
}list;

static const valueString packetClass[] = {
    {OSPF_HELLO,   "Hello Packet"   },
    {OSPF_DB_DESC, "DB Description" },
    {OSPF_LS_REQ,  "LS Request"     },
    {OSPF_LS_UPD,  "LS Update"      },
    {OSPF_LS_ACK,  "LS Acknowledge" },
    {0,             NULL}
};

typedef struct _ipv4Header {
    valueString Src;// = { 0dSrcImgWIndex,"Source IPv4 Address" }
    valueString Dst;// = { 0dSrcImgWIndex,"Destination IPv4 Address" }
}ipv4Header;

typedef struct _ospfHeader {
    valueString Version;// = {OSPF_VERSION_2,"OSPF Version"}
    valueString PacketLength;//={0,"Packet Length"}
    valueString SourceOspfRouter;// = {0dSrcImgWIndex,"Source OSPF Router"}
    valueString AreaID;// = { OSPF_AREA_ID,"Area ID" }
    valueString AuthType;// = {OSPF_AUTH_NONE,"Auth Type"}
}ospfHeader;

typedef struct _ospfData {
    //valueString PacketClass;
    //ospfHelloPacket Hello;
    struct ospfLSHeader* LSH;
    struct ospfLSPacket* LSP;

}ospfData;

typedef struct _ospfHelloPacket {
    valueString NetworkMask;// = { OSPF_NETWORK_MASK_24,"Network Mask" }255.255.255.0
    valueString HelloInt;// = { OSPF_HELLO_INT,"Hello Interval" }
    valueString RPri;// = { 1,"Router Priority" }
    valueString RDeadInt;// = { OSPF_HELLO_ROUTER_DEAD_INT,"Router Dead Interval" }
    valueString DR;// = { 0dSrcImgWIndex,"Designated Router" }
    valueString BDR;// = { 0dSrcImgWIndex,"Backup Designated Router" }
    valueString ActiveNbr;// = {0dSrcImgWIndex,"Active Neighbor"}
}ospfHelloPacket;

typedef struct _ospfLSHeader {
    valueString LSType;// = { 1,"LS Type: Router-LSA" }
    valueString AdvID;// = { 0dSrcImgWIndex,"Advertising Router" }
    valueString SeqNum;// = { 0x800000001,"Sequence Number" } mark ls new or old
}ospfLSHeader;

typedef struct _ospfLSPacket {
    valueString LinkID;// = {0dSrcImgWIndex,"Link ID - IP network/subnet number"}
    valueString LinkData;// = { OSPF_NETWORK_MASK_24 ,"Link Data"}
    valueString Metrics;// = {0,"Number of Metrics"}
    struct ospfLSPacket* NextLSP;
}ospfLSPacket;

typedef struct {
    ipv4Header IPV4;
    ospfHeader OSPFHeader;
    ospfData OSPFData;

}FloodingInfo;



//list* InitList(unsigned int numOfNode);
FloodingInfo* InitFloodingInfo(unsigned int src, unsigned int dst);

ospfData* InitOspfData(unsigned int advID, unsigned int seqNum,
    unsigned int linkID, unsigned int linkData, unsigned int metrics);

ospfLSHeader* InitLSHeader(unsigned int advID, unsigned int seqNum);

ospfLSPacket* InitLSP(unsigned int linkID, unsigned int linkData, unsigned int metrics);

ospfLSPacket* AddLSP(ospfLSPacket* currentLSP, unsigned int linkID, unsigned int linkData, unsigned int metrics);

void DestroyLSP(ospfLSPacket* targetLSP);

#endif // !__OspfFlood_H__
