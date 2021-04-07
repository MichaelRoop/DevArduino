// 
// 
// 
#include "MsgDefines.h"
#include "MessageHelpers.h"

uint8_t MsgHelpers::inMsgIds[MAX_IN_ID_REG][2];
uint8_t MsgHelpers::currentIdListNextPos;

// TODO - remove - record of how to use when not typedef
//void (*MsgHelpers::MyBoolFunc) (bool) = NULL;
MsgHelpers::msgFuncPtrBool MsgHelpers::ptrBool = NULL;
MsgHelpers::msgFuncPtrInt8 MsgHelpers::ptrInt8 = NULL;
MsgHelpers::msgFuncPtrInt16 MsgHelpers::ptrInt16 = NULL;
MsgHelpers::msgFuncPtrInt32 MsgHelpers::ptrInt32 = NULL;
MsgHelpers::msgFuncPtrUInt8 MsgHelpers::ptrUInt8 = NULL;
MsgHelpers::msgFuncPtrUInt16 MsgHelpers::ptrUInt16 = NULL;
MsgHelpers::msgFuncPtrUInt32 MsgHelpers::ptrUInt32 = NULL;
MsgHelpers::msgFuncPtrFloat32 MsgHelpers::ptrFloat32 = NULL;
MsgHelpers::errEventPtr MsgHelpers::errCallback = NULL;

MsgHelpers::MsgHelpers() {
	MsgHelpers::currentIdListNextPos = 0;
}

#ifndef SECTION_PUBLIC_METHODS

bool MsgHelpers::RegisterInIds(uint8_t id, MsgDataType dataType) {
	if (MsgHelpers::currentIdListNextPos < MAX_IN_ID_REG) {
		if (dataType > typeUndefined && dataType < typeInvalid) {
			inMsgIds[currentIdListNextPos][0] = id;
			inMsgIds[currentIdListNextPos][1] = dataType;
			MsgHelpers::currentIdListNextPos++;
		}
	}
	return false;
}


void MsgHelpers::RegisterErrCallback(errEventPtr ptr) {
	MsgHelpers::errCallback = ptr;
}


void MsgHelpers::RegisterFuncBool(msgFuncPtrBool ptr) {
	MsgHelpers::ptrBool = ptr;
}


void MsgHelpers::RegisterFuncInt8(msgFuncPtrInt8 ptr) {
	MsgHelpers::ptrInt8 = ptr;
}


void MsgHelpers::RegisterFuncInt16(msgFuncPtrInt16 ptr) {
	MsgHelpers::ptrInt16 = ptr;
}


void MsgHelpers::RegisterFuncInt32(msgFuncPtrInt32 ptr) {
	MsgHelpers::ptrInt32 = ptr;
}


void MsgHelpers::RegisterFuncUInt8(msgFuncPtrUInt8 ptr) {
	MsgHelpers::ptrUInt8 = ptr;
}


void MsgHelpers::RegisterFuncUInt16(msgFuncPtrUInt16 ptr) {
	MsgHelpers::ptrUInt16 = ptr;
}


void MsgHelpers::RegisterFuncUInt32(msgFuncPtrUInt32 ptr) {
	MsgHelpers::ptrUInt32 = ptr;
}


void MsgHelpers::RegisterFuncFloat32(msgFuncPtrFloat32 ptr) {
	MsgHelpers::ptrFloat32 = ptr;
}

#endif // !SECTION_PUBLIC_METHODS


bool MsgHelpers::ValidateHeader(uint8_t* buff, uint8_t size) {
	if (size < MSG_HEADER_SIZE) {
		return MsgHelpers::RaiseError(err_InvalidHeaderSize);
	}

	if ((*(buff + SOH_POS) != _SOH) || (*(buff + STX_POS) != _STX)) {
		return MsgHelpers::RaiseError(err_StartDelimiters);
	}

	if (!(*(buff + TYPE_POS) > typeUndefined) && (*(buff + TYPE_POS) < typeInvalid)) {
		return MsgHelpers::RaiseError(err_InvalidType);
	}

	// Get size and validate the number against the data type
	MsgDataType dt = (MsgDataType)(*(buff + TYPE_POS));
	uint16_t sizeField = MsgHelpers::GetSizeFromHeader(buff);
	if (sizeField == 0) {
		return MsgHelpers::RaiseError(err_InvalidSizeField);
	}

	byte payloadSize = GetPayloadSize(dt);
	if (sizeField != (MSG_HEADER_SIZE + MSG_FOOTER_SIZE + payloadSize)) {
		return MsgHelpers::RaiseError(err_InvalidPayloadSizeField);
	}

	bool ok = false;
	uint8_t id = MsgHelpers::GetIdFromHeader(buff);
	// validate id and expected data type
	for (int i = 0; i < MsgHelpers::currentIdListNextPos; i++) {
		if (MsgHelpers::inMsgIds[i][0] == id) {
			ok = (MsgHelpers::inMsgIds[i][1] == dt);
			break;
		}
	}
	return ok ? true : MsgHelpers::RaiseError(err_InvalidDataTypeForRegisteredId);
}


uint16_t MsgHelpers::GetSizeFromHeader(uint8_t* buff) {
	uint16_t size = 0;
	memcpy(&size, (buff + SIZE_POS), sizeof(uint16_t));
	// Message size must be at least big enough for header, footer and minimum 1 byte payload
	if (size < (MSG_HEADER_SIZE + MSG_FOOTER_SIZE + 1)) {
		size = 0;
	}
	return size;
}

MsgDataType MsgHelpers::GetDataTypeFromHeader(uint8_t* buff) {
	return (MsgDataType)(*(buff + TYPE_POS));
}


uint8_t MsgHelpers::GetIdFromHeader(uint8_t* buff) {
	return (MsgDataType)(*(buff + ID_POS));
}


bool MsgHelpers::ValidateMessage(uint8_t* buff, int length) {
	if (MsgHelpers::ValidateHeader(buff, length)) {
		if (MsgHelpers::GetSizeFromHeader(buff) == length) {
			return RaiseRegisteredEvents(buff);
		}
	}
	return false;
}


bool MsgHelpers::RaiseError(MsgError err) {
	if (MsgHelpers::errCallback != NULL) {
		MsgHelpers::errCallback(err);
	}
	// Return false to optimize use to exit bool methods
	return false;
}


bool MsgHelpers::RaiseBool(uint8_t id, uint8_t* buff, uint8_t offset) {
	if (ptrBool != NULL) {
		bool bVal;
		memcpy(&bVal, (buff + offset), sizeof(bool));
		ptrBool(id, bVal);
		return true;
	}
	return false;
}


bool MsgHelpers::RaiseInt8(uint8_t id, uint8_t* buff, uint8_t offset) {
	if (ptrInt8 != NULL) {
		int8_t val;
		memcpy(&val, (buff + offset), sizeof(int8_t));
		ptrInt8(id, val);
		return true;
	}
	return false;
}


bool MsgHelpers::RaiseInt16(uint8_t id, uint8_t* buff, uint8_t offset) {
	if (ptrInt16 != NULL) {
		int16_t val;
		memcpy(&val, (buff + offset), sizeof(int16_t));
		ptrInt16(id, val);
		return true;
	}
	return false;
}


bool MsgHelpers::RaiseInt32(uint8_t id, uint8_t* buff, uint8_t offset) {
	if (ptrInt32 != NULL) {
		int32_t val;
		memcpy(&val, (buff + offset), sizeof(int32_t));
		ptrInt32(id, val);
		return true;
	}
	return false;
}


bool MsgHelpers::RaiseUInt8(uint8_t id, uint8_t* buff, uint8_t offset) {
	if (ptrUInt8 != NULL) {
		uint8_t val;
		memcpy(&val, (buff + offset), sizeof(uint8_t));
		ptrUInt8(id, val);
		return true;
	}
	return false;
}


bool MsgHelpers::RaiseUInt16(uint8_t id, uint8_t* buff, uint8_t offset) {
	if (ptrUInt16 != NULL) {
		uint16_t val;
		memcpy(&val, (buff + offset), sizeof(uint16_t));
		ptrUInt16(id, val);
		return true;
	}
	return false;
}


bool MsgHelpers::RaiseUInt32(uint8_t id, uint8_t* buff, uint8_t offset) {
	if (ptrUInt32 != NULL) {
		uint32_t val;
		memcpy(&val, (buff + offset), sizeof(uint32_t));
		ptrUInt32(id, val);
		return true;
	}
	return false;
}


bool MsgHelpers::RaiseFloat32(uint8_t id, uint8_t* buff, uint8_t offset) {
	if (ptrFloat32 != NULL) {
		float val;
		memcpy(&val, (buff + offset), sizeof(float));
		ptrFloat32(id, val);
		return true;
	}
	return false;
}



bool MsgHelpers::RaiseRegisteredEvents(uint8_t* buff) {
	uint8_t offset = VALUE_POS;
	uint8_t id = MsgHelpers::GetIdFromHeader(buff);
	switch (MsgHelpers::GetDataTypeFromHeader(buff)) {
	case typeBool:
		return RaiseBool(id, buff, offset);
	case typeInt8:
		return RaiseInt8(id, buff, offset);
	case typeUInt8:
		return RaiseUInt8(id, buff, offset);
	case typeInt16:
		return RaiseInt16(id, buff, offset);
	case typeUInt16:
		return RaiseUInt16(id, buff, offset);
	case typeInt32:
		return RaiseInt32(id, buff, offset);
	case typeUInt32:
		return RaiseUInt32(id, buff, offset);
	case typeFloat32:
		return RaiseFloat32(id, buff, offset);
	case typeUndefined:
	case typeInvalid:
	default:
		// Should not happen raise err and return true to avoid unregietered error
		RaiseError(err_InvalidType);
		return true;
	}
}


byte MsgHelpers::GetPayloadSize(MsgDataType dt) {
	switch (dt) {
	case typeBool:
	case typeInt8:
	case typeUInt8:
		// Same size for above
		return sizeof(uint8_t);
	case typeInt16:
	case typeUInt16:
		return sizeof(uint16_t);
	case typeInt32:
	case typeUInt32:
	case typeFloat32:
		return sizeof(uint32_t);
	case typeUndefined:
	case typeInvalid:
	default:
		// Should not happen. Return big size to cause failure
		return 1000;
	}
}








