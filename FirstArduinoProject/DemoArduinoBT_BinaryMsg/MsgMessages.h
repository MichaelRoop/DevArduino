// MsgMessages.h

#ifndef _MSGMESSAGES_h
#define _MSGMESSAGES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "MsgDefines.h"
#include "MsgEnumerations.h"

template<typename T> struct MsgBinary {
public:
	uint8_t Soh = SOH;
	uint8_t Stx = STX;
	uint16_t Size = 0;
	uint8_t DataType = typeFloat32;
	uint8_t Id = 0;
	T Value = 0;
	uint8_t Etx = ETX;
	uint8_t Eot = EOT;

	MsgBinary();
};

// declare specializations
template<> MsgBinary<float>::MsgBinary();


// Packed typedefs
__attribute__((packed)) typedef MsgBinary<float> MsgFloat32;

//MsgBinary<float>



//MsgBinary::MsgBinary() {
//	this->Soh = SOH;
//	this->Stx = STX;
//	//this->Size = sizeof(MsgFloat32);
//	this->Id = 0;
//	//this->Value = 0;
//	this->Etx = ETX;
//	this->Eot = EOT;
//
//}




/*
__attribute__((packed)) struct MsgFloat32 {
public:
	uint8_t Soh;
	uint8_t Stx;
	uint16_t Size;
	uint8_t Id;
	float Value;
	uint8_t Etx;
	uint8_t Eot;

	MsgFloat32();


};


MsgFloat32::MsgFloat32() {
	this->Soh = SOH;
	this->Stx = STX;
	this->Size = sizeof(MsgFloat32);
	this->Id = 0;
	this->Value = 0;
	this->Etx = ETX;
	this->Eot = EOT;
}
*/

#endif

