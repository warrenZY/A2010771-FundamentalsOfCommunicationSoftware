#include "OspfPacket.h"
#include "Message.h"
#define myNew(className,Num) (className*)malloc(Num*sizeof(className));

FloodingInfo* InitFloodingInfo(unsigned int src, unsigned int dst) {
	FloodingInfo* floodInfo = myNew(FloodingInfo*, 1);
	ipv4Header* _ipv4 = myNew(ipv4Header*, 1);
	ospfHeader* _ospfHeader = myNew(ospfHeader*, 1);
	valueString* Src = myNew(valueString*, 1);
	valueString* Dst = myNew(valueString*, 1);
	valueString* Version = myNew(valueString*, 1);
	valueString* PacketLength = myNew(valueString*, 1);
	valueString* SourceOspfRouter = myNew(valueString*, 1);
	valueString* AreaID = myNew(valueString*, 1);
	valueString* AuthType = myNew(valueString*, 1);
	if (Src && Dst && _ipv4 && Version && PacketLength && SourceOspfRouter
		&& AreaID && AuthType && _ospfHeader && floodInfo) {
		Src->value = src;
		Src->strptr = "Source IPv4 Address";
		Dst->value = dst;
		Dst->strptr = "Source IPv4 Address";
		_ipv4->Src = *Src;
		_ipv4->Dst = *Dst;
		Version->value = OSPF_VERSION_2;
		Version->strptr = "OSPF Version";
		PacketLength->value = 0;
		PacketLength->strptr = "Packet Length";
		SourceOspfRouter->value = 0dSrcImgWIndex;
		SourceOspfRouter->strptr = "Source OSPF Router";
		AreaID->value = OSPF_AREA_ID;
		AreaID->strptr = "Area ID";
		AuthType->value = OSPF_AUTH_NONE;
		AuthType->strptr = "Auth Type";
		_ospfHeader->Version = *Version;
		_ospfHeader->PacketLength = *PacketLength;
		_ospfHeader->SourceOspfRouter = *SourceOspfRouter;
		_ospfHeader->AreaID = *AreaID;
		_ospfHeader->AuthType = *AuthType;
		floodInfo->IPV4 = *_ipv4;
		floodInfo->OSPFHeader = *_ospfHeader;
	}else {
		printf("%sEncounter critical error, malloc failed(%s)", ErrorMsg, __FUNCTION__);
	};
	return floodInfo;
}


ospfData* InitOspfData(unsigned int advID, unsigned int seqNum,
	unsigned int linkID, unsigned int linkData, unsigned int metrics) {
	ospfData* data =myNew(ospfData,1);
	if (data) {
		data->LSH = InitLSHeader(advID, seqNum);
		data->LSP = InitLSP(linkID, linkData, metrics);
	}else {
		printf("%sEncounter critical error, malloc failed(%s)", ErrorMsg, __FUNCTION__);
	}
	
	return data;
}

ospfLSHeader* InitLSHeader(unsigned int advID,unsigned int seqNum) {
	ospfLSHeader* lSHeader = (ospfLSHeader*)malloc(sizeof(ospfLSHeader));
	valueString* _lsType = (valueString*)malloc(sizeof(valueString));
	valueString* _advID = (valueString*)malloc(sizeof(valueString));
	valueString* _seqNum = (valueString*)malloc(sizeof(valueString));
	if (_lsType && _advID && _seqNum && lSHeader) {
		_lsType->value = 1;
		_lsType->strptr = "LS Type: Router-LSA";
		_advID->value = advID;
		_advID->strptr = "Advertising Router";
		_seqNum->value = 0x800000001;
		_seqNum->strptr = "Sequence Number";
		lSHeader->LSType = *_lsType;
		lSHeader->AdvID = *_advID;
		lSHeader->SeqNum = *_seqNum;
	}else {
		printf("%sEncounter critical error, malloc failed(%s)", ErrorMsg, __FUNCTION__);
	}
	return lSHeader;
}

/*
//origin format of link list
list* InitList(unsigned int numOfNode) {
	list* headPointer = (list*)malloc(sizeof(list));//create head node
	list* tempPointer = headPointer;
	//create link list
	for (int index = 1; index < numOfNode; index++) {
		//create nodes
		list* currentPointer = (list*)malloc(sizeof(list));
		currentPointer->value = index;
		currentPointer->next = NULL;
		//create link with preview node
		tempPointer->next = currentPointer;
		tempPointer = tempPointer->next;
	}
	return headPointer;
}
*/

ospfLSPacket* InitLSP(unsigned int linkID, unsigned int linkData, unsigned int metrics) {
	ospfLSPacket* headLSP=myNew(ospfLSPacket,1);
	valueString* _linkID = myNew(valueString, 1);
	valueString* _linkData = myNew(valueString, 1);
	valueString* _metrics = myNew(valueString, 1);
	if (headLSP && _linkID && _linkData && _metrics){
		headLSP->NextLSP = NULL;
		_linkID->value = linkID;
		_linkID->strptr = "Link ID - IP network/subnet number";
		_linkData->value = linkData;
		_linkData->strptr = "Link Data";
		_metrics->value = metrics;
		_metrics->strptr = "Number of Metrics";
		headLSP->LinkID = *_linkID;
		headLSP->LinkData = *_linkData;
		headLSP->Metrics = *_metrics;
	}
	return headLSP;
}

ospfLSPacket* AddLSP(ospfLSPacket* currentLSP,unsigned int linkID,unsigned int linkData,unsigned int metrics) {
	ospfLSPacket* insertLSP = (ospfLSPacket*)malloc(sizeof(ospfLSPacket));
	valueString* _linkID = myNew(valueString, 1);
	valueString* _linkData = myNew(valueString, 1);
	valueString* _metrics = myNew(valueString, 1);
	if (insertLSP && _linkID && _linkData && _metrics) {
		insertLSP->NextLSP = NULL;
		_linkID->value = linkID;
		_linkID->strptr = "Link ID - IP network/subnet number";
		_linkData->value = linkData;
		_linkData->strptr = "Link Data";
		_metrics->value = metrics;
		_metrics->strptr = "Number of Metrics";
		insertLSP->LinkID = *_linkID;
		insertLSP->LinkData = *_linkData;
		insertLSP->Metrics = *_metrics;
		insertLSP->NextLSP = NULL;
		currentLSP->NextLSP = insertLSP;
	}
	return insertLSP;
}

void DestroyLSP(ospfLSPacket* targetLSP) {
	ospfLSPacket* currentLSP = targetLSP;
	while (currentLSP) {
		//set targetLSP-Next as targetLSP and destory targetLSP
		targetLSP = targetLSP->NextLSP;
		free(currentLSP);
		currentLSP = targetLSP;
	}
}

/*

typedef struct _ospfLSPacket {
	valueString LinkID = {0dSrcImgWIndex,"Link ID - IP network/subnet number"};
	valueString LinkData = { OSPF_NETWORK_MASK_24 ,"Link Data"};
	valueString Metrics = {0,"Number of Metrics"};
	ospfLSPacket* NextLSP;
}ospfLSPacket;

*/