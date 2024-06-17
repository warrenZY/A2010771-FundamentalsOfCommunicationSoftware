
#ifndef __CRC8_H__
#define __CRC8_H__


typedef signed          char int8_t;
typedef signed short    int int16_t;
typedef signed          int int32_t;

typedef unsigned        char uint8_t;
typedef unsigned short  int uint16_t;
typedef unsigned        int uint32_t;

#define TRUE 0;
#define FALSE -1;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "PacketEth.h"
#include "stdio.h"


//Polynom: (x^8 + x^2 + x^1 + 1)



 /**
  * Check the final crc value(Receiver code)
  *
  * \param p The tv buffer containing the data.
  * \param len Number of bytes in the message.
  * \param offset Offset in the message.
  * \param received_fcs The received FCS.
  * \return     Returns 0 if the checksum is correct, -1 if it is not correct
  *****************************************************************************/
	int check_fcs(uint8_t* p, uint16_t len, uint8_t received_fcs);
	uint8_t CalCRCTable(uint8_t value);
	int CreateCRCTable();
	uint8_t CheckCRC8MAXIM(uint8_t chCurrByte, uint8_t chNextByte);

#endif	// __CRC8_H__