// 
// 
// 

#include "MsgMessages.h"


// Implement specializations
template<> MsgBinary<bool>::MsgBinary() {
	DataType = typeBool;
	Size = sizeof(MsgBool);
}


template<> MsgBinary<int8_t>::MsgBinary() {
	DataType = typeInt8;
	Size = sizeof(MsgInt8);
}


template<> MsgBinary<int16_t>::MsgBinary() {
	DataType = typeInt16;
	Size = sizeof(MsgInt16);
}


template<> MsgBinary<int32_t>::MsgBinary() {
	DataType = typeInt32;
	Size = sizeof(MsgInt32);
}


template<> MsgBinary<uint8_t>::MsgBinary() {
	DataType = typeUInt8;
	Size = sizeof(MsgUInt8);
}


template<> MsgBinary<uint16_t>::MsgBinary() {
	DataType = typeUInt16;
	Size = sizeof(MsgUInt16);
}


template<> MsgBinary<uint32_t>::MsgBinary() {
	DataType = typeUInt32;
	Size = sizeof(MsgUInt32);
}


template<> MsgBinary<float>::MsgBinary() {
	DataType = typeFloat32;
	Size = sizeof(MsgFloat32);
}



