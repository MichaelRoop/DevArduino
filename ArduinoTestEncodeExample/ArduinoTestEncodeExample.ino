/*
 Name:		ArduinoTestEncodeExample.ino
 Created:	12/12/2019 7:02:54 PM
 Author:	Michael

 Arduino 101: timer and interrupts
 * 3: Timer2 compare interrupt example. Quadrature Encoder
 * more infos: http://www.letmakerobots.com/node/28278
 * created by RobotFreak
 *
 * Credits:
 * based on code from Peter Dannegger
 * http://www.mikrocontroller.net/articles/Drehgeber

 https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072
 Brought here for learning and modification

*/

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include “WConstants.h”
#endif

#ifndef DEFINES

// Encoder Pins
#define encLtA 2
#define encLtB 3
#define encRtA 11 
#define encRtB 12
#define ledPin 13

#define LT_PHASE_A		digitalRead(encLtA)
#define LT_PHASE_B		digitalRead(encLtB)
#define RT_PHASE_A		digitalRead(encRtA)
#define RT_PHASE_B		digitalRead(encRtB)

static volatile int8_t encDeltaLt, encDeltaRt;

static int8_t lastLt, lastRt;

int encLt, encRt;

#endif // !DEFINES


#ifndef ARDUINO_MAIN_FUNCTIONS

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    QuadratureEncoderInit();
}

// the loop function runs over and over again until power down or reset
void loop() {
    encLt += QuadratureEncoderReadLt();
    encRt += QuadratureEncoderReadRt();

    Serial.print("Lt: ");
    Serial.print(encLt, DEC);
    Serial.print(" Rt : ");
    Serial.println(encRt, DEC);
    delay(1000);
}

#endif // !ARDUINO_MAIN_FUNCTIONS

#ifndef PRIVATE_HELPERS

// Interupt routine for timer 2
ISR(TIMER2_COMPA_vect) {
    int8_t val, diff;
    digitalWrite(ledPin, HIGH);   // toggle LED pin
    val = 0;

    if (LT_PHASE_A) {
        val = 3;
    }

    if (LT_PHASE_B) {
        // convert gray to binary
        val ^= 1; 
    }

    diff = lastLt - val; // difference last - new
    // bit 0 = value (1)
    if (diff & 1) {				
        lastLt = val;				    // store new as next last
        encDeltaLt += (diff & 2) - 1;	// bit 1 = direction (+/-)
    }

    val = 0;

    if (RT_PHASE_A) {
        val = 3;
    }

    if (RT_PHASE_B) {
        val ^= 1; // convert gray to binary
    }

    diff = lastRt - val; // difference last - new

    // bit 0 = value (1)
    if (diff & 1) {				
        lastRt = val;				// store new as next last
        encDeltaRt += (diff & 2) - 1;		// bit 1 = direction (+/-)
    }
    digitalWrite(ledPin, LOW);   // toggle LED pin
}


void QuadratureEncoderInit(void) {
    int8_t val;
    cli();
    TIMSK2 |= (1 << OCIE2A);
    sei();

    pinMode(encLtA, INPUT);
    pinMode(encRtA, INPUT);
    pinMode(encLtB, INPUT);
    pinMode(encRtB, INPUT);

    val = 0;

    if (LT_PHASE_A) {
        val = 3;
    }

    if (LT_PHASE_B) {
        val ^= 1;
    }

    lastLt = val;
    encDeltaLt = 0;
    val = 0;

    if (RT_PHASE_A) {
        val = 3;
    }

    if (RT_PHASE_B) {
        val ^= 1;
    }

    lastRt = val;
    encDeltaRt = 0;
    encLt = 0;
    encRt = 0;
}


// read single step encoders
int8_t QuadratureEncoderReadLt(void) {
    int8_t val;
    cli();
    val = encDeltaLt;
    encDeltaLt = 0;
    sei();
    // counts since last call
    return val; 
}


// read single step encoders
int8_t QuadratureEncoderReadRt(void) {
    int8_t val;
    cli();
    val = encDeltaRt;
    encDeltaRt = 0;
    sei();
    // counts since last call
    return val;					
}


#endif // !PRIVATE_HELPERS




