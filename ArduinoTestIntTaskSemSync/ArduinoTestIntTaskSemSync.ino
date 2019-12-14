

/*
 Name:		ArduinoTestIntTaskSemSync.ino
 Created:	12/13/2019 7:29:16 PM
 Author:	Michael

 Used to test out the FreeRTOS library to synchronize an
 interupt and task and access to the Bluetooth port and 
 a data queue

*/


int incr = 0;
int index = 0;
typedef int (*testFunc)();

volatile int vIncr = 0;
volatile int vIndex = 0;

unsigned char cIndex = 0;
unsigned char cIncr = 0;

void TimeFunction(testFunc fn, int iterations);


// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    Serial.println("Serial port started");

    int iterations = 1000;

    // Timing function pointer calls (1,180 us)
    TimeFunction(Func1, iterations);


    // The timings in the notes were based on an 8 bit Arduino Uno board (Atmel).
    // Will need to test on a 32 bit Arduino 
    // Due 
    //    https://store.arduino.cc/usa/due
    //    Atmel SAM3X8E ARM Cortex-M3 CPU.
    //    Mega footprint
    //    Digi IO 54 Analog In 12 (12bit ADC) Analog out 2(DAC) Flash 512KB SRAM 96KB Clock 84MHz
    //    1 HW UART 3 HW USARTs for TTL
    // Zero 
    //    Atmel’s SAMD21 MCU, which features a 32-bit ARM Cortex® M0+ core
    //    https://store.arduino.cc/usa/arduino-zero
    //    Uno footprint
    //    Digi IO 20 Analog In 6 (12bit ADC) Analog out 1(10bit DAC) Flash 256KB SRAM 32KB Clock 48MHz


    //---------------------------------------
    // Direct processing 
    //---------------------------------------

    // operations on 8 bit local variables (88us)
    // By limiting to 8 bit variables we get 88us. Done 4 times would give 352us for 1000 rather than 556us with int
    unsigned long start = micros();
    unsigned char cInc1 = 0;
    for (unsigned char cI = 0; cI < 250; cI++) { 
        cInc1++; 
    }
    PrintLoopResults(
        GetMicroSecondInteral(start, micros()),
        250 * 4, "iterations of 8 bit direct for loop");



    // single operation (556us)
    start = micros();
    int inc = 0;
    for (int i = 0; i < iterations; i++) { inc++; }
    PrintLoopResults(
        GetMicroSecondInteral(start, micros()),
        iterations, "iterations of direct for loop");

    // Second for loop with more operations (4,828us)
    int inc2 = 0;
    int inc3 = 0;
    int inc4 = 0;
    int inc5 = 0;
    for (int i = 0; i < iterations; i++) {
        inc++;
        inc2++;
        inc3++;
        inc4++;
        inc5++;
    }
    PrintLoopResults(
        GetMicroSecondInteral(start, micros()),
        iterations, "iterations of (2) direct for loop");

    // Second for loop with more operations (10,360us)
    inc2 = 0;
    inc3 = 0;
    inc4 = 0;
    inc5 = 0;
    for (int i = 0; i < iterations; i++) {
        inc = inc + i;
        inc2 = inc2 + i;
        inc3 = inc3 + i;
        inc4 = inc4 + i;
        inc5 = inc5 + i;
    }
    PrintLoopResults(
        GetMicroSecondInteral(start, micros()),
        iterations, "iterations of (3) direct for loop");


    // while loop local (552us)
    start = micros();
    int ii = 0;
    while (ii < iterations) {
        inc++;
        ii++;
    }
    PrintLoopResults(
        GetMicroSecondInteral(start, micros()),
        iterations, "iterations - while loop local var");

    //---------------------------------------
    // Global variables
    //---------------------------------------

    // for loop (1,804us) - same as first local for loop but with global variable
    // takes 1,805us rather than 556us.
    start = micros();
    index = 0;
    for (; index < iterations; index++) { incr++; }
    PrintLoopResults(
        GetMicroSecondInteral(start, micros()),
        iterations, "iterations - for loop global var");

    // while loop (1,804us)
    start = micros();
    incr = 0;
    index = 0;
    while (index < iterations) {
        incr++;
        index++;
    }
    PrintLoopResults(
        GetMicroSecondInteral(start, micros()),
        iterations, "iterations - while loop global var");

    //---------------------------------------
    // Global volatile variables
    //---------------------------------------
    // for loop (2,808us) - same as first local for loop but with global variable
    // takes 2,808us rather than 556us for local and 1,804 for regular global.
    start = micros();
    index = 0;
    for (/*vIndex = 0*/; vIndex < iterations; vIndex++) { vIncr++; }
    PrintLoopResults(
        GetMicroSecondInteral(start, micros()),
        iterations, "iterations - for loop volatile global var");

    // Test with variables limited to 8 bit so as not to have more than one
    // operation to read and change variables
    // Result is 248us of 254 iterations
    // Multiplied by 4 is 992us for 1,000 iterations - with 32 bit variables it is 1,804. So, half the processing time
    start = micros();
    for (cIndex = 0; cIndex < 255; cIndex++) { cIncr++; }
    PrintLoopResults(
        GetMicroSecondInteral(start, micros()),
        255, "iterations - for loop (8bit) volatile global var");


    Serial.println("Done...");


    //    //// In case the time has exceeded the 1ms
    //    //if (interval >= 1000) {
    //    //    // No wait
    //    //    Serial.print("microseconds used in 1ms loop ");
    //    //    Serial.print(interval);
    //    //    Serial.println(" NO WAIT REQUIRED");
    //    //}
    //    //else {
    //    //    delayMicroseconds((1000 - interval));
    //    //    Serial.print("microseconds used in 1ms loop ");
    //    //    Serial.print(interval);
    //    //    Serial.println("");
    //    //}

}


void loop() {
}

//// the loop function runs over and over again until power down or reset
//void loop() {
//    //unsigned long start = micros();
//    //unsigned long stop = start;
//    //unsigned long interval = 0;
//
//    //if (!doIt) {
//    //    return;
//    //}
//
//    //doIt = false;
//    //// Do the serial accrual from Bluetooth
//    ////   If a message is complet (/r/n) send to state machine
//    ////   If a message is not available send default Tick to state machine
//    //// At end of call to state machine
//    ////   Get the response
//    ////   If required send response through Bluetooth
//
//    //// recalculate the microwait based on 1ms minus time to execute the 
//    //// state machine call
//
//    //// Do some stuff - 10,000 = 5 seconds - seems high for 10000 iterations
//    //int inc = 0;
//    //for (int i = 0; i < 10000; i++) { inc++; }
//
//    //stop = micros();
//    //// Check for overflow
//    //Serial.print("   start:"); Serial.println(start);
//    //Serial.print("    stop:"); Serial.println(stop);
//    //if (stop < start) {
//    //    interval = stop + (0xFFFFFFFF - start);
//    //}
//    //else {
//    //    interval = stop - start;
//    //}
//
//    ////interval = (stop > start) ? (stop - start) : (stop + (0xFFFFFFFF - start));
//    //Serial.print("interval:"); Serial.println(interval);
//
//
//
//    //// In case the time has exceeded the 1ms
//    //if (interval >= 1000) {
//    //    // No wait
//    //    Serial.print("microseconds used in 1ms loop ");
//    //    Serial.print(interval);
//    //    Serial.println(" NO WAIT REQUIRED");
//    //}
//    //else {
//    //    delayMicroseconds((1000 - interval));
//    //    Serial.print("microseconds used in 1ms loop ");
//    //    Serial.print(interval);
//    //    Serial.println("");
//    //}
//}
//
//


unsigned long GetMicroSecondInteral(unsigned long startMicroSeconds, unsigned long stopMicroSeconds) {
    // Scenario where the micros() overflowed since start (about 70 minutes)
    if (stopMicroSeconds < startMicroSeconds) {
        return stopMicroSeconds + (0xFFFFFFFF - startMicroSeconds);
    }
    // Normal scenario with no overflow of tick 
    return stopMicroSeconds - startMicroSeconds;
}

void PrintLoopResults(unsigned long interval, int iterations, const char* msg) {
    Serial.print("Timing loop: ");
    Serial.print(interval);
    Serial.print(" us for ");
    Serial.print(iterations);
    Serial.print(" ");
    Serial.println(msg);
}



void TimeFunction(testFunc fn, int iterations) {
    unsigned long start = micros();
    int val = 0;
    //int iterations = 0;
    for (int i = 0; i < iterations; i++) {
        val = fn();
    }
    //unsigned long stop = micros();

    //Serial.print("   start:"); Serial.println(start);
    //Serial.print("    stop:"); Serial.println(stop);
    //unsigned long duration = 0;
    //if (stop < start) {
    //    duration = stop + (0xFFFFFFFF - start);
    //}
    //else {
    //    duration = stop - start;
    //}
    //Serial.print("interval:"); Serial.println(duration);

    PrintLoopResults(
        GetMicroSecondInteral(start, micros()),
        iterations, "iterations of function calls");

    //return duration;
    return 0;
}
//
//
int Func1() {
    int sum = 0;
    //for (int i = 0; i < 8; i++) {
    //    sum += i;
    //}
    sum++;
    return sum;
}


