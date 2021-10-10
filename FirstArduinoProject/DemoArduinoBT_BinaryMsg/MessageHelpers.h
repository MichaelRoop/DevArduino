// MessageHelpers.h

#ifndef _MESSAGEHELPERS_h
#define _MESSAGEHELPERS_h

#include "MsgDefines.h"
#include "MsgEnumerations.h"

#define MAX_IN_ID_REG 25

#ifdef DEBUG
struct ErrMsg {
public:
	uint8_t SOH = 0;
	uint8_t STX = 0;
	uint16_t Size = 0;
	MsgDataType DataType = typeUndefined;
	byte PayloadSize = 0;
	uint16_t RequiredSize = 0;
	uint8_t Id = 0;
	MsgError Error = err_NoErr;
	void Init();
	ErrMsg();
};
#endif // DEBUG


class MsgHelpers {
	// Typedefs for execute methods
	typedef void (*msgFuncPtrBool)(uint8_t id, bool value);
	typedef void (*msgFuncPtrInt8)(uint8_t id, int8_t value);
	typedef void (*msgFuncPtrInt16)(uint8_t id, int16_t value);
	typedef void (*msgFuncPtrInt32)(uint8_t id, int32_t value);
	typedef void (*msgFuncPtrUInt8)(uint8_t id, uint8_t value);
	typedef void (*msgFuncPtrUInt16)(uint8_t id, uint16_t value);
	typedef void (*msgFuncPtrUInt32)(uint8_t id, uint32_t value);
	typedef void (*msgFuncPtrFloat32)(uint8_t id, float value);
#ifdef DEBUG
	typedef void (*errEventPtr)(ErrMsg* err);
#endif // DEBUG

public:

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

	static void RegisterFuncBool(msgFuncPtrBool ptr);
	static void RegisterFuncInt8(msgFuncPtrInt8 ptr);
	static void RegisterFuncInt16(msgFuncPtrInt16 ptr);
	static void RegisterFuncInt32(msgFuncPtrInt32 ptr);
	static void RegisterFuncUInt8(msgFuncPtrUInt8 ptr);
	static void RegisterFuncUInt16(msgFuncPtrUInt16 ptr);
	static void RegisterFuncUInt32(msgFuncPtrUInt32 ptr);
	static void RegisterFuncFloat32(msgFuncPtrFloat32 ptr);
#ifdef DEBUG
	static void RegisterErrCallback(errEventPtr ptr);
#endif // DEBUG

private:
	static msgFuncPtrBool ptrBool;
	static msgFuncPtrInt8 ptrInt8;
	static msgFuncPtrInt16 ptrInt16;
	static msgFuncPtrInt32 ptrInt32;
	static msgFuncPtrUInt8 ptrUInt8;
	static msgFuncPtrUInt16 ptrUInt16;
	static msgFuncPtrUInt32 ptrUInt32;
	static msgFuncPtrFloat32 ptrFloat32;
#ifdef DEBUG
	static errEventPtr errCallback;
#endif // DEBUG

	// Each entry has id,data type. Used for validation
	static uint8_t inMsgIds[MAX_IN_ID_REG][2];
	static uint8_t currentIdListNextPos;
#ifdef DEBUG
	static ErrMsg errMsg;
#endif // DEBUG

	MsgHelpers();
	static uint8_t GetIdFromHeader(uint8_t* buff);
	static byte GetPayloadSize(MsgDataType dt);
#ifdef DEBUG
	static bool RaiseError(ErrMsg* msg);
#endif // DEBUG
	static bool RaiseRegisteredEvents(uint8_t* buff);
	static bool RaiseBool(uint8_t id, uint8_t* buff, uint8_t offset);
	static bool RaiseInt8(uint8_t id, uint8_t* buff, uint8_t offset);
	static bool RaiseInt16(uint8_t id, uint8_t* buff, uint8_t offset);
	static bool RaiseInt32(uint8_t id, uint8_t* buff, uint8_t offset);
	static bool RaiseUInt8(uint8_t id, uint8_t* buff, uint8_t offset);
	static bool RaiseUInt16(uint8_t id, uint8_t* buff, uint8_t offset);
	static bool RaiseUInt32(uint8_t id, uint8_t* buff, uint8_t offset);
	static bool RaiseFloat32(uint8_t id, uint8_t* buff, uint8_t offset);

};

#endif

