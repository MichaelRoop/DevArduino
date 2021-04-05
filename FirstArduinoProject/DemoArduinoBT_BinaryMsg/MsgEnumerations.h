// MsgEnumerations.h

#ifndef _MSGENUMERATIONS_h
#define _MSGENUMERATIONS_h

//#if defined(ARDUINO) && ARDUINO >= 100
//	#include "arduino.h"
//#else
//	#include "WProgram.h"
//#endif

// Corresponds to data types on App
enum MsgDataType {
	tyepUndefined = 0,
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


#endif

