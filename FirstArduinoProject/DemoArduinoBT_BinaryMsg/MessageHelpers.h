// MessageHelpers.h

#ifndef _MESSAGEHELPERS_h
#define _MESSAGEHELPERS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "MsgEnumerations.h"

#define MAX_IN_ID_REG 25

class MsgHelpers {


public:
	typedef void (*boolMsgFuncPtr)(bool value);

	// Register in message ids and expected data type for validation
	static bool RegisterInIds(uint8_t id, MsgDataType dataType);

	// Validate only the header fragment on read
	static bool ValidateHeader(uint8_t* buff, uint8_t size);

	// Read the size field from the header. Call ValidateHeader first
	static uint16_t GetSizeFromHeader(uint8_t* buff);

	// Read data type field from header. Call ValidateHeader first
	static MsgDataType GetDataTypeFromHeader(uint8_t* buff);

	// Validate the entire message after it is read
	static bool ValidateMessage(uint8_t* buff, int length);

	// TODO - remove test only
	static void Execute();

	static void RegisterBoolFunc(uint8_t id, boolMsgFuncPtr ptr);

private:
	// TODO - replace. Will have an array of them to register
	static boolMsgFuncPtr MyBoolFunc;

	MsgHelpers();
	static uint8_t GetIdFromHeader(uint8_t* buff);
	static byte GetPayloadSize(MsgDataType dt);
	// Each entry has id,data type. Used for validation
	static uint8_t inMsgIds[MAX_IN_ID_REG][2];
	static uint8_t currentIdListNextPos;
};

#endif

