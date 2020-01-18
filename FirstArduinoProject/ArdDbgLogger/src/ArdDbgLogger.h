/*
 Name:		ArdDbgLogger.h
 Created:	1/18/2020 6:39:34 AM
 Author:	Michael
 Editor:	http://www.visualmicro.com
*/

#ifndef _ArdDbgLogger_h
#define _ArdDbgLogger_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define DBG_INFO

// Separate macro for the init to keep log macro from cluture
#if defined (DBG_OFF) || defined (DBG_INFO)
    #define DBG_STREAM_INIT(baud) Serial.begin(baud); while(!Serial){;}
#else
    #define DBG_STREAM_INIT(baud)
#endif

// Go with two levels only and OFF
#if defined (DBG_OFF)
    #define DBG_I(val)
    #define DBG_I_NL(val)
    #define DBG_I_F(format,...)
    #define DBG_E(val)
    #define DBG_E_NL(val)
    #define DBG_E_F(format,...)
#else
    #if defined (DBG_ERR)
        char __LOG_data__[100];
        #define DBG_I(val)
        #define DBG_I_NOL(val)
        #define DBG_I_F(format,...)
        #define DBG_E(val) Serial.println(val)
        #define DBG_E_NOL(val) Serial.print(val)
        #define DBG_E_F(format,...) sprintf(__LOG_data__, format, ##__VA_ARGS__); Serial.print(__LOG_data__)
    #else
        #if defined (DBG_INFO)
            char __LOG_data__[100];
            #define DBG_I(val) Serial.println(val)
            #define DBG_I_NOL(val) Serial.print(val)
            #define DBG_I_F(format,...) sprintf(__LOG_data__, format, ##__VA_ARGS__); Serial.print(__LOG_data__)
            #define DBG_E(val) Serial.println(val)
            #define DBG_E_NOL(val) Serial.print(val)
            #define DBG_E_F(format,...) sprintf(__LOG_data__, format, ##__VA_ARGS__); Serial.print(__LOG_data__)
        #else
            #define DBG_I(val)
            #define DBG_I_NOL(val)
            #define DBG_I_F(format,...)
            #define DBG_E(val)
            #define DBG_E_NOL(val)
            #define DBG_E_F(format,...)
        #endif
    #endif

#endif

// end _ArdDbgLogger_h
#endif

