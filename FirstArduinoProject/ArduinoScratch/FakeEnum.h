// FakeEnum.h

#ifndef _FAKEENUM_h
#define _FAKEENUM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//#pragma once
#pragma error "------------------------------INCLUDED---------------------------------"



// Derive the enum from byte will have it use only 1 byte memory per 
// enum to save space. Variables with that enum type will be 1 byte
enum testEnum1 : byte {
	IDLE = 0,
	STOPPED = 1,
	STARTED = 2,
	FAILED = 3,
	EXITED = 4
};

// if you try to derive this one from byte it will fail compilation
// since the entries are out of byte range
enum testEnum2 /*: byte*/ {
	IDLE2 = 300,
	STOPPED2 = 301,
	STARTED2 = 302,
	FAILED2 = 303,
	EXITED2 = 304
};

#endif

