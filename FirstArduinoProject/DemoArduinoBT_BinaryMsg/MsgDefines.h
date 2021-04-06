// MsgDefines.h

#ifndef _MSGDEFINES_h
#define _MSGDEFINES_h

//#if defined(ARDUINO) && ARDUINO >= 100
//	#include "arduino.h"
//#else
//	#include "WProgram.h"
//#endif

// Start of heading, start of text
#define _SOH 0x01
#define _STX 0x02
// End of text end of transmission
#define _ETX 0x03
#define _EOT 0x04

#define MIN_MSG_SIZE 9
#define MAX_MSG_SIZE 12
#define MSG_HEADER_SIZE 6
#define MSG_FOOTER_SIZE 2

#define SOH_POS 0
#define STX_POS 1
#define SIZE_POS 2
#define TYPE_POS 4
#define ID_POS 5
#define VALUE_POS 6


#endif

