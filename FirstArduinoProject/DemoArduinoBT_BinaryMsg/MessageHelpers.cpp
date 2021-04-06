// 
// 
// 

#include "MessageHelpers.h"
#include "MsgDefines.h"

uint8_t MsgHelpers::inMsgIds[MAX_IN_ID_REG][2];
uint8_t MsgHelpers::currentIdListNextPos;

// TODO - remove - record of how to use when not typedef
//void (*MsgHelpers::MyBoolFunc) (bool) = NULL;
MsgHelpers::boolMsgFuncPtr MsgHelpers::MyBoolFunc = NULL;


MsgHelpers::MsgHelpers() {
	MsgHelpers::currentIdListNextPos = 0;
}


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


bool MsgHelpers::ValidateHeader(uint8_t* buff, uint8_t size) {
	if (size < MSG_HEADER_SIZE) {
		return false;
	}

	bool isOk =
		(*(buff + SOH_POS) == _SOH) &&
		(*(buff + STX_POS) == _STX) &&
		(*(buff + TYPE_POS) > typeUndefined) &&
		(*(buff + TYPE_POS) < typeInvalid);

	if (isOk) {
		// Get size and validate the number against the data type
		MsgDataType dt = (MsgDataType)(*(buff + TYPE_POS));
		uint16_t sizeField = MsgHelpers::GetSizeFromHeader(buff);
		isOk = sizeField > 0;
		if (isOk) {
			byte payloadSize = GetPayloadSize(dt);
			isOk = (sizeField == MSG_HEADER_SIZE + MSG_FOOTER_SIZE + payloadSize);
		}
		if (isOk) {
			bool tmp = false;
			uint8_t id = MsgHelpers::GetIdFromHeader(buff);
			// validate id and expected data type
			for (int i = 0; i < MsgHelpers::currentIdListNextPos; i++) {
				if (MsgHelpers::inMsgIds[i][0] == id) {
					tmp = (MsgHelpers::inMsgIds[i][1] == dt);
					break;
				}
			}
			isOk = tmp;
		}
	}
	return isOk;
}


uint16_t MsgHelpers::GetSizeFromHeader(uint8_t* buff) {
	uint16_t size = 0;
	memcpy(&size, (buff + SIZE_POS), sizeof(uint16_t));
	// Message size must be at least big enough for header, footer and 1 byte payload
	if (size <= (MSG_HEADER_SIZE + MSG_FOOTER_SIZE + 1)) {
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
		return MsgHelpers::GetSizeFromHeader(buff) == length;
	}
	return false;
}

void MsgHelpers::Execute() {
	// TODO this will be replaced with an internal method which fires by registered id
	MyBoolFunc(true);
}


void MsgHelpers::RegisterBoolFunc(uint8_t id, boolMsgFuncPtr ptr) {
	MyBoolFunc = ptr;
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








