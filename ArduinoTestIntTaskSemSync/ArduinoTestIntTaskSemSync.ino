

/*
 Name:		ArduinoTestIntTaskSemSync.ino
 Created:	12/13/2019 7:29:16 PM
 Author:	Michael

 Used to test out the FreeRTOS library to synchronize an
 interupt and task and access to the Bluetooth port and 
 a data queue

*/


//int incr = 0;
//int index = 0;
typedef int (*testFunc)();

int Func1();

//
//volatile int vIncr = 0;
//volatile int vIndex = 0;
//
//unsigned char cIndex = 0;
//unsigned char cIncr = 0;

void TimeFunction(testFunc fn, int iterations);


// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    Serial.println("Serial port started");



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
    
    TimeFunction(Func1, 250);           // 300us avg(1.20)
    Iterate8BitSimpleIncrement(250);    // 88us avg(0.35) 
    Iterate16BitSimpleIncrement(250);   // 120us avg(0.48)
    Iterate8Bit5Increments(250);        // 88us avg(0.35) 
    Iterate16Bit5Increments(250);       // 120us avg(0.48)
    Iterate16bitMoreOperations(250);    // 120us avg(0.48)
    WhileLoop8bitLocalVar(250);         // 96us avg(0.38) 
    WhileLoop16bitLocalVar(250);        // 128us avg(0.51)

    // 120us avg(0.48) 250 iterations - for loop local var 16 bit
    // 124us avg(0.50) 250 iterations - while loop local var 16 bit
    CompareLocalWhileAndFor16bit(250);

    // 432us avg(1.73) 250 iterations - for loop global var 16 bit
    // 436us avg(1.74) 250 iterations - while loop global var 16 bit
    CompareGlobalWhileAndFor16bit(250);

    VolatileLoop8Bit(250);          // 276us avg(1.10)
    VolatileLoop16Bit(250);         // 300us avg(1.20)

    // 88us avg(0.35) 250 iterations - for loop local var 8 bit
    // 284us avg(1.14) 250 iterations - for loop volatile 8 bit global var
    // 440us avg(1.76) 250 iterations - for loop global var 8 bit
    LocalGlobalVolatileCompare8Bit(250);

    // 128us avg(0.51) 250 iterations - for loop local var 16 bit
    // 292us avg(1.17) 250 iterations - for loop volatile 16 bit global var
    // 436us avg(1.74) 250 iterations - for loop global var 16 bit
    LocalGlobalVolatileCompare16Bit(250);

    Serial.println("Done...");
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

void Iterate8BitSimpleIncrement(byte iterations) {
    unsigned long start = 0;
    unsigned long stop;
    byte inc = 0;
    start = micros();
    for (byte i = 0; i < iterations; i++) {
        inc++;
    }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations of 8 bit direct for loop (in func)");
}

void Iterate16BitSimpleIncrement(int iterations) {
    // single operation (120us)
    unsigned long start = 0;
    unsigned long stop;
    int inc = 0;
    start = micros();
    for (int i = 0; i < iterations; i++) { inc++; }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations of 16 bit direct for loop");
}

void Iterate8Bit5Increments(byte iterations) {
    unsigned long start = 0;
    unsigned long stop;
    // Second 8 bit for loop with more operations (us)
    byte b1 = 0;
    byte b2 = 0;
    byte b3 = 0;
    byte b4 = 0;
    byte b5 = 0;
    start = micros();
    for (byte i = 0; i < iterations; i++) {
        b1++;
        b2++;
        b3++;
        b4++;
        b5++;
    }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations 8 bit of 5 increments");
}

void Iterate16Bit5Increments(int iterations) {
    unsigned long start = 0;
    unsigned long stop;
    // Second for loop with more operations (4,828us)
    int inc = 0;
    int inc2 = 0;
    int inc3 = 0;
    int inc4 = 0;
    int inc5 = 0;
    start = micros();
    for (int i = 0; i < iterations; i++) {
        inc++;
        inc2++;
        inc3++;
        inc4++;
        inc5++;
    }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations 16 bit of 5 increments");
}

void Iterate16bitMoreOperations(int iterations) {
    unsigned long start = 0;
    unsigned long stop;
    // Second for loop with more operations (10,360us)
    int inc = 0;
    int inc2 = 0;
    int inc3 = 0;
    int inc4 = 0;
    int inc5 = 0;
    start = micros();
    for (int i = 0; i < iterations; i++) {
        inc = inc + i;
        inc2 = inc2 + i;
        inc3 = inc3 + i;
        inc4 = inc4 + i;
        inc5 = inc5 + i;
    }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - more operations");
}

void WhileLoop8bitLocalVar(byte iterations) {
    unsigned long start = 0;
    unsigned long stop;
    byte inc = 0;
    // while loop local (552us)
    start = micros();
    byte i = 0;
    while (i < iterations) {
        inc++;
        i++;
    }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - while loop local var 8 bit");
}

void WhileLoop16bitLocalVar(int iterations) {
    unsigned long start = 0;
    unsigned long stop;
    int inc = 0;
    // while loop local (552us)
    start = micros();
    int i = 0;
    while (i < iterations) {
        inc++;
        i++;
    }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - while loop local var 16 bit");
}

void CompareLocalWhileAndFor16bit(int iterations) {
    unsigned long start = 0;
    unsigned long stop;
    int i = 0;
    int incr = 0;
    start = micros();
    for (; i < iterations; i++) { incr++; }
    stop = micros();
    Serial.println("---------------------------------------------------");
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - for loop local var 16 bit");

    // while loop (1,804us)
    start = micros();
    incr = 0;
    i = 0;
    while (i < iterations) {
        incr++;
        i++;
    }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, micros()),
        iterations, "iterations - while loop local var 16 bit");
    Serial.println("---------------------------------------------------");
}


int gbli = 0;
int gblincr = 0;
void CompareGlobalWhileAndFor16bit(int iterations) {
    unsigned long start = 0;
    unsigned long stop;
    gbli = 0;
    gblincr = 0;
    start = micros();
    for (; gbli < iterations; gbli++) { gblincr++; }
    stop = micros();
    Serial.println("---------------------------------------------------");
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - for loop global var 16 bit");

    // while loop (1,804us)
    start = micros();
    gblincr = 0;
    gbli = 0;
    while (gbli < iterations) {
        gblincr++;
        gbli++;
    }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, micros()),
        iterations, "iterations - while loop global var 16 bit");
    Serial.println("---------------------------------------------------");
}


volatile byte vbI = 0;
volatile byte vbIncr = 0;
void VolatileLoop8Bit(byte iterations) {
    unsigned long start = 0;
    unsigned long stop;
    
    start = micros();
    for (vbI = 0; vbI < iterations; vbI++) { vbIncr++; }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - for loop volatile 8 bit global var");
}

volatile byte viI = 0;
volatile byte viIncr = 0;
void VolatileLoop16Bit(int iterations) {
    unsigned long start = 0;
    unsigned long stop;

    start = micros();
    for (viI = 0; viI < iterations; viI++) { viIncr++; }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - for loop volatile 16 bit global var");
}


int gblbi = 0;
int gblbincr = 0;
void LocalGlobalVolatileCompare8Bit(byte iterations) {
    unsigned long start = 0;
    unsigned long stop;
    byte incr = 0;
    start = micros();
    for (byte i = 0; i < iterations; i++) { incr++; }
    stop = micros();
    Serial.println("---------------------------------------------------");
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - for loop local var 8 bit");

    vbIncr = 0;
    start = micros();
    for (vbI = 0; vbI < iterations; vbI++) { vbIncr++; }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - for loop volatile 8 bit global var");

    gblbincr = 0;
    start = micros();
    for (gblbi = 0; gblbi < iterations; gblbi++) { gblbincr++; }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - for loop global var 8 bit");

    Serial.println("---------------------------------------------------");
}


void LocalGlobalVolatileCompare16Bit(int iterations) {
    unsigned long start = 0;
    unsigned long stop;
    int i = 0;
    int incr = 0;
    start = micros();
    for (; i < iterations; i++) { incr++; }
    stop = micros();
    Serial.println("---------------------------------------------------");
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - for loop local var 16 bit");

    viIncr = 0;
    start = micros();
    for (viI = 0; viI < iterations; viI++) { viIncr++; }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - for loop volatile 16 bit global var");

    gblincr = 0;
    start = micros();
    for (gbli = 0; gbli < iterations; gbli++) { gblincr++; }
    stop = micros();
    PrintLoopResults2(
        GetMicroSecondInteral(start, stop),
        iterations, "iterations - for loop global var 16 bit");

    Serial.println("---------------------------------------------------");
}


unsigned long GetMicroSecondInteral(unsigned long startMicroSeconds, unsigned long stopMicroSeconds) {
    // Scenario where the micros() overflowed since start (about 70 minutes)
    if (stopMicroSeconds < startMicroSeconds) {
        return stopMicroSeconds + (0xFFFFFFFF - startMicroSeconds);
    }
    // Normal scenario with no overflow of tick 
    return stopMicroSeconds - startMicroSeconds;
}

void PrintLoopResults(unsigned long interval, int iterations, const char* msg) {
    //Serial.print("Timing loop: ");
    Serial.print(interval);
    Serial.print(" us for ");
    Serial.print(iterations);
    Serial.print(" ");
    Serial.println(msg);
}

void PrintLoopResults2(unsigned long interval, int iterations, const char* msg) {
    //Serial.print("Timing loop: ");
    Serial.print(interval);
    Serial.print("us avg(");
    double avg = ((double)interval) / ((double)iterations);
    Serial.print(avg);
    Serial.print(") ");
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

    PrintLoopResults2(
        GetMicroSecondInteral(start, micros()),
        iterations, "iterations of function calls");

    //return duration;
    //return 0;
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


