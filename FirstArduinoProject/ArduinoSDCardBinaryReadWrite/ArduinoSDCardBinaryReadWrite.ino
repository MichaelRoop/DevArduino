/*
 Name:		ArduinoSDCardBinaryReadWrite.ino
 Created:	1/17/2020 11:53:22 AM
 Author:	Michael
*/


//#include "TestSubClass.h"
//#include "ArdDbgLogger.h"
//#include "ArdDbgLogger.h"
//#include "ArdDbgLogger.h"
#include <SPI.h>
#include <SD.h>

#define DBG_LOG
#include <ArduinoDbgLog.h>
#include <ArduinoFileManager.h>
//#include <TestS>

const int sdPin = 4;

typedef struct  __attribute__((packed)) dataStruct_t {
public:
    float fVal;
    byte bVal;
    int iVal;
    dataStruct_t() {
        this->bVal = 0;
        this->fVal = 0;
        this->iVal = 0;
    }
} dataStruct;

// the setup function runs once when you press reset or power the board
void setup() {
    // Open serial communications and wait for port to open:
    DBG_INIT(115400);
   
    DBG_OUT("This is: %i of: %i, then: 0x%X\n", 233, 6233, 32133);
    DBG_OUT("BLAH BLAH\n");


    //TestSub ts;
    //ts.PrintMsg();
    TestSubCaller tsc;
    tsc.InvokeSub();


    return;
    if (!InitSD(sdPin)) {
        while (1);
    }

    if (CreateFile("data1.Bin")) {
        File fs = SD.open("data1.Bin", FILE_WRITE);
        if (fs != NULL) {
            dataStruct_t ds;
            //Serial.print("bVal="); Serial.println(ds.bVal);
            //Serial.print("fVal="); Serial.println(ds.fVal);
            //Serial.print("iVal="); Serial.println(ds.iVal);
            ds.bVal = 111;
            ds.fVal = 2.22;
            ds.iVal = 3389;
            //Serial.print("bVal="); Serial.println(ds.bVal);
            //Serial.print("fVal="); Serial.println(ds.fVal);
            //Serial.print("iVal="); Serial.println(ds.iVal);

            uint8_t* ptr = (uint8_t*)&ds;
            unsigned int time = 0;
            unsigned int start = 0;

            start = micros();
            size_t size = fs.write(ptr, sizeof(ds));
            time = micros() - start;

            Serial.print("Size of write: "); Serial.println(size);
            Serial.print("Time: "); Serial.println(time);
            fs.flush();
            fs.close();
            File fs2 = SD.open("data1.Bin", FILE_READ);
            if (fs2 != NULL) {
                ds.bVal = 0;
                ds.fVal = 0;
                ds.iVal = 0;

                start = micros();
                size = fs2.readBytes(ptr, sizeof(ds));
                time = micros() - start;

                Serial.print("Size of read: "); Serial.println(size);
                Serial.print("Time: "); Serial.println(time);
                fs2.close();
                Serial.print("bVal="); Serial.println(ds.bVal);
                Serial.print("fVal="); Serial.println(ds.fVal);
                Serial.print("iVal="); Serial.println(ds.iVal);
            }
            else {
                Serial.println("Failed to open for read");
            }

        }
        else {
            Serial.println("Failed to open for write");
        }



    }

}

// the loop function runs over and over again until power down or reset
void loop() {
}


bool InitSD(uint8_t pin) {
    Serial.print("Initializing SD card...");
    // see if the card is present and can be initialized:
    if (SD.begin(pin)) {
        Serial.println("card initialized.");
        return true;
    }
    Serial.println("Card failed, or not present");
    return false;
}

void DeleteFile(char* name) {
    Serial.print("Deleting file: ");
    Serial.println(name);
    if (SD.exists(name)) {
        if (SD.remove(name)) {
            Serial.println("removed");
        }
        else {
            Serial.println("Failed to remove");
        }
    }
    else {
        Serial.println("Does not exist");
    }
}

bool CreateFile(char* name) {
    DeleteFile(name);
    Serial.print("Create file: ");
    Serial.println(name);
    File file = SD.open(name, FILE_WRITE);

    if (file == NULL) {
        Serial.println("FAILED TO CREATE");
        return false;
    }
    else {
        file.close();
        return true;
    }
}