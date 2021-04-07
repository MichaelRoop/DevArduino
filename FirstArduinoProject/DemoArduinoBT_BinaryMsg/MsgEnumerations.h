// MsgEnumerations.h

#ifndef _MSGENUMERATIONS_h
#define _MSGENUMERATIONS_h

// Corresponds to data types on App
enum MsgDataType {
	typeUndefined = 0,
	typeBool = 1,
	typeInt8 = 2,
	typeUInt8 = 3,
	typeInt16 = 4,
	typeUInt16 = 5,
	typeInt32 = 6,
	typeUInt32 = 7,
	typeFloat32 = 8,
	//typeString = 9,
	typeInvalid = 9
};


enum MsgError {
	err_CallbackNotRegistered = 0,
	err_InvalidType = 1,
	err_InvalidHeaderSize = 2,
	err_StartDelimiters = 3,
	err_InvalidSizeField = 4,
	err_InvalidPayloadSizeField = 5,
	err_InvalidDataTypeForRegisteredId = 6
};


#endif

