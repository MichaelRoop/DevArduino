// MsgMessages.h

#ifndef _MSGMESSAGES_h
#define _MSGMESSAGES_h

#include "MsgDefines.h"
#include "MsgEnumerations.h"

// Binary message template for all messages
template<typename T> struct MsgBinary {
public:
	uint8_t SOH = _SOH;
	uint8_t STX = _STX;
	uint16_t Size = 0;
	uint8_t DataType = typeUndefined;
	uint8_t Id = 0;
	T Value = 0;
	uint8_t ETX = _ETX;
	uint8_t EOT = _EOT;

	MsgBinary();
};

// Declare specializations on the constructor
template<> MsgBinary<bool>::MsgBinary();
template<> MsgBinary<int8_t>::MsgBinary();
template<> MsgBinary<int16_t>::MsgBinary();
template<> MsgBinary<int32_t>::MsgBinary();
template<> MsgBinary<uint8_t>::MsgBinary();
template<> MsgBinary<uint16_t>::MsgBinary();
template<> MsgBinary<uint32_t>::MsgBinary();
template<> MsgBinary<float>::MsgBinary();

// Create Packed typedefs
__attribute__((packed)) typedef MsgBinary<bool> MsgBool;
__attribute__((packed)) typedef MsgBinary<int8_t> MsgInt8;
__attribute__((packed)) typedef MsgBinary<int16_t> MsgInt16;
__attribute__((packed)) typedef MsgBinary<int32_t> MsgInt32;
__attribute__((packed)) typedef MsgBinary<uint8_t> MsgUInt8;
__attribute__((packed)) typedef MsgBinary<uint16_t> MsgUInt16;
__attribute__((packed)) typedef MsgBinary<uint32_t> MsgUInt32;
__attribute__((packed)) typedef MsgBinary<float> MsgFloat32;

#endif

