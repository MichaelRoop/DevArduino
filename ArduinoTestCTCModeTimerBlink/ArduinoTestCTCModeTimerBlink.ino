/*
 Name:		ArduinoTestCTCModeTimerBlink.ino
 Created:	12/12/2019 6:09:55 PM
 Author:	Michael

 This is taken from a tutorial on timers and interrupts
 https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072

 Arduino 101: timer and interrupts
   1: Timer1 compare match interrupt example
   more infos: http://www.letmakerobots.com/node/28278

 Brought here and modified to experiment and learn

*/

#define ledPin 13

#ifndef MAIN_ARDUINO_FUNCTIOS

// the setup function runs once when you press reset or power the board
void setup() {
    pinMode(ledPin, OUTPUT);
    SetupTimer1();
}

// the loop function runs over and over again until power down or reset
void loop() {
    // Nothing to do here. The interupt does it all
    // Added a delay here to show that the timer keeps working independently of main loop
    delay(2000);
    Serial.println("Wake on loop delay");
}

#endif // !MAIN_ARDUINO_FUNCTIOS

#ifndef PRIVATE_HELPER_FUNCTIONS

/*
Initialize Timer 1 as a CTC mode interupt that fires twice a second independant
of the main loop
*/
void SetupTimer1() {
    // initialize timer1 

    noInterrupts();           // disable all interrupts
    // Set all masks to 0
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    // Compare match register 16MHz/256/2Hz - sets the timer frequency to 2x second
    // 
    // 1. CPU frequency 16Mhz for Arduino
    // 2. Maximum timer counter value(256 for 8bit, 65536 for 16bit timer) - Timer 1 is 8 bits
    // 3. Divide CPU frequency through the choosen prescaler(16000000 / 256 = 62500)
    // 4, Divide result through the desired frequency(62500 / 2Hz = 31250)
    // 5. Verify the result against the maximum timer counter value(31250 < 65536 success) if fail, choose bigger prescaler.
    TCNT1 = 0;
    OCR1A = 31250;
    TCCR1B |= (1 << WGM12);   // CTC mode (Clean Timer on Compare)
    TCCR1B |= (1 << CS12);    // 256 prescaler 
    TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

    interrupts();             // enable all interrupts
}


bool isOn = 0;
/*
Timer compare interrupt service routine
This is the routine that gets called when the timer fires
*/
ISR(TIMER1_COMPA_vect) {
    digitalWrite(ledPin, digitalRead(ledPin) ^ 1);   // toggle LED pin
    // My addition to show on serial monitor
    Serial.println((isOn ? "+" : "-"));
    isOn = !isOn;
}

#endif // !PRIVATE_HELPER_FUNCTIONS

