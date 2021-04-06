// 
// 
// 
#include "MsgMessages.h"

// Init() specialization
template<> void MsgBinary<bool>::Init() {
	this->DataType = typeBool;
	this->Size = sizeof(MsgBool);
}


template<> void MsgBinary<int8_t>::Init() {
	this->DataType = typeInt8;
	this->Size = sizeof(MsgInt8);
}


template<> void MsgBinary<int16_t>::Init() {
	this->DataType = typeInt16;
	this->Size = sizeof(MsgInt16);
}


template<> void MsgBinary<int32_t>::Init() {
	this->DataType = typeInt32;
	this->Size = sizeof(MsgInt32);
}


template<> void MsgBinary<uint8_t>::Init() {
	this->DataType = typeUInt8;
	this->Size = sizeof(MsgUInt8);
}


template<> void MsgBinary<uint16_t>::Init() {
	this->DataType = typeUInt16;
	this->Size = sizeof(MsgUInt16);
}


template<> void MsgBinary<uint32_t>::Init() {
	this->DataType = typeUInt32;
	this->Size = sizeof(MsgUInt32);
}


template<> void MsgBinary<float>::Init() {
	this->DataType = typeFloat32;
	this->Size = sizeof(MsgFloat32);
}

// Default constructor specialization
template<> MsgBinary<bool>::MsgBinary() {
	this->Init();
}


template<> MsgBinary<int8_t>::MsgBinary() {
	this->Init();
}


template<> MsgBinary<int16_t>::MsgBinary() {
	this->Init();
}


template<> MsgBinary<int32_t>::MsgBinary() {
	this->Init();
}


template<> MsgBinary<uint8_t>::MsgBinary() {
	this->Init();
}


template<> MsgBinary<uint16_t>::MsgBinary() {
	this->Init();
}


template<> MsgBinary<uint32_t>::MsgBinary() {
	this->Init();
}


template<> MsgBinary<float>::MsgBinary() {
	this->Init();
}


// Need to have an int function

// Default constructor with parameters specialization
template<> MsgBinary<bool>::MsgBinary(uint8_t id, bool value) {
	this->Init();
	this->Id = id;
	this->Value = value;
}


template<> MsgBinary<int8_t>::MsgBinary(uint8_t id, int8_t value) {
	this->Init();
	this->Id = id;
	this->Value = value;
}


template<> MsgBinary<int16_t>::MsgBinary(uint8_t id, int16_t value) {
	this->Init();
	this->Id = id;
	this->Value = value;
}


template<> MsgBinary<int32_t>::MsgBinary(uint8_t id, int32_t value) {
	this->Init();
	this->Id = id;
	this->Value = value;
}


template<> MsgBinary<uint8_t>::MsgBinary(uint8_t id, uint8_t value) {
	this->Init();
	this->Id = id;
	this->Value = value;
}


template<> MsgBinary<uint16_t>::MsgBinary(uint8_t id, uint16_t value) {
	this->Init();
	this->Id = id;
	this->Value = value;
}


template<> MsgBinary<uint32_t>::MsgBinary(uint8_t id, uint32_t value) {
	this->Init();
	this->Id = id;
	this->Value = value;
}


template<> MsgBinary<float>::MsgBinary(uint8_t id, float value) {
	this->Init();
	this->Id = id;
	this->Value = value;
}



