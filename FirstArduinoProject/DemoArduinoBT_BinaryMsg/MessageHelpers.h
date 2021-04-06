// MessageHelpers.h

#ifndef _MESSAGEHELPERS_h
#define _MESSAGEHELPERS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "MsgEnumerations.h"


class MsgHelpers {
public:
	// Validate only the header fragment on read
	static bool ValidateHeader(uint8_t* buff);

	// Read the size field from the header. Call ValidateHeader first
	static uint16_t GetSizeFromHeader(uint8_t* buff);

	// Read data type field from header. Call ValidateHeader first
	static MsgDataType GetDataTypeFromHeader(uint8_t* buff);

	// Read the Id field from the header. Call ValidateHeader first
	static uint8_t GetIdFromHeader(uint8_t* buff);

	// Validate the entire message after it is read
	static bool ValidateMessage(uint8_t* buff, int length);

private:
	MsgHelpers() {}
	static byte GetPayloadSize(MsgDataType dt);
};

#endif

