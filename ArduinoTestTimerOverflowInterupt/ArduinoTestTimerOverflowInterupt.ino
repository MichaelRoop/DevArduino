/*
 Name:		ArduinoTestTimerOverflowInterupt.ino
 Created:	12/12/2019 6:33:14 PM
 Author:	Michael

 Arduino 101: timer and interrupts
   1: Timer1 compare match interrupt example
   more infos: http://www.letmakerobots.com/node/28278
   created by RobotFreak

 https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072
  Brought here to modify and learn

*/

#define ledPin 13

#ifndef ARDUINO_MAIN_FUNCTIONS

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    SetupTimer1OverflowInterupt();
}

// the loop function runs over and over again until power down or reset
void loop() {
    // Nothing to do here. The interupt does it all
    // Added a delay here to show that the timer overflow 
    // interupt keeps working independently of main loop
    delay(2000);
    Serial.println("Wake on loop delay");
}

#endif // !ARDUINO_MAIN_FUNCTIONS

#ifndef PRIVATE_HELPER_FUNCTIONS

void SetupTimer1OverflowInterupt() {
    // initialize timer1 

    noInterrupts();           // disable all interrupts

    // Set masks to 0
    TCCR1A = 0;
    TCCR1B = 0;


    // preload timer 65536-16MHz/256/2Hz
    // All bits set on is 0xFFFF (65536)
    TCNT1 = 34286;            
    TCCR1B |= (1 << CS12);    // 256 prescaler 
    TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
 
    interrupts();             // enable all interrupts
}


bool isOn = 0;

// interrupt service routine that wraps a user defined function supplied by attachInterrupt
ISR(TIMER1_OVF_vect) {
    // Need to preload timer for next time
    TCNT1 = 34286;            
    digitalWrite(ledPin, digitalRead(ledPin) ^ 1);

    // My addition to show toggle on serial monitor
    Serial.println((isOn ? "++" : "--"));
    isOn = !isOn;
}



#endif // !PRIVATE_HELPER_FUNCTIONS


