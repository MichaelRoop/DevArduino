/*
 Name:		ArduinoBLESerial.ino
 Created:	4/28/2020 8:55:58 PM
 Author:	Michael
*/


#include <ArduinoBLE.h>
#include "C:/Program Files (x86)/Arduino/hardware/tools/avr/avr/include/string.h"

#ifndef SECTION_DATA

// Create a service that will act as a serial port. Max 20 bytes at a time
BLEService serialService("9999");
BLEService serialService2("9990");

// You can write to it, read it by polling or subscribe to a notification
BLEByteCharacteristic outByteCharacteristic("9998", BLERead | BLENotify); // 39320
BLEByteCharacteristic inByteCharacteristic("9997", BLEWrite | BLERead | BLENotify );  // 39319 // cannot see it with just write


//BLECharacteristic dataCharacteristic("9998", BLERead | BLEWrite | BLENotify, 1, false);

bool hasInput = false;
#define IN_BUFF_SIZE 500
char buff[IN_BUFF_SIZE];
unsigned char inIndex = 0;
#define MAX_BLOCK_SIZE 20

#endif

// the setup function runs once when you press reset or power the board
void setup() {
    // TODO - This needs to be removed to work if not connected to USB serial port
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for Native USB only
    }
    Serial.println("Debug serial port active");

    pinMode(LED_BUILTIN, OUTPUT);
    if (!BLE.begin()) {
        Serial.println("Failed to start BLE");
        while (1);
    }
    else {
        Serial.println("BLE begun");
    }

    // Name that users see in list for 'Add Device'
    BLE.setLocalName("Mikies Arduino serial device");
    // The Device Name Characteristic
    BLE.setDeviceName("Mikies serial device");

    // assign event handlers for connected, disconnected to peripheral
    BLE.setEventHandler(BLEConnected, bleOnConnectHandler);
    BLE.setEventHandler(BLEDisconnected, bleOnDisconnectHandler);

    BLE.setAdvertisedService(serialService2);
    serialService2.addCharacteristic(outByteCharacteristic);
    BLE.addService(serialService2);

    serialService.addCharacteristic(inByteCharacteristic);
    inByteCharacteristic.subscribe();
    inByteCharacteristic.setEventHandler(BLEWritten, inBytesWritten);
    BLE.addService(serialService);

    BLE.setAdvertisedService(serialService);

    BLE.advertise();
    Serial.println("Bluetooth device active, waiting for connections...");
}

// the loop function runs over and over again until power down or reset
void loop() {
    BLEDevice central = BLE.central();
    if (central) {
        //Serial.println("CONNECTED");
        while (central.connected()) {
            //ReadIncoming();
            //ProcessIncomingBuff();
        }
        digitalWrite(LED_BUILTIN, LOW);
    }
}


void ReadIncoming() {
    if (inByteCharacteristic.valueUpdated()) {
        digitalWrite(LED_BUILTIN, HIGH);
        byte value = 0;
        inByteCharacteristic.readValue(value);
        buff[inIndex] = value;
        DebugStreamIncoming(buff[inIndex]);
        digitalWrite(LED_BUILTIN, LOW);
    }


    //byte data[50];
    //if (dataCharacteristic.valueUpdated()) {
    //    dataCharacteristic.valueLength();
    //    int count = dataCharacteristic.readValue(data, 50);
    //    // At this point we would copy count # of bytes from the in block over to the main buffer
    //    // TODO - need to preserve the block size copied. need start of index
    //}

}



void ProcessIncomingBuff() {
    // Doing \n\r terminator sequence
    if (buff[inIndex] == '\r') {
        Serial.println("Printing msg in buffer");
        hasInput = true;

        // At this point we would parse the message to determine what to do

        // For demo, just bounce the message back
        //Serial.write(buff, inIndex + 1);
        for (int i = 0; i <= inIndex; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            outByteCharacteristic.writeValue(buff[i]);
            digitalWrite(LED_BUILTIN, LOW);
        }

        //// or write by 20 byte block in raw
        //int byteCount = inIndex + 1;
        //int blockCount = byteCount / MAX_BLOCK_SIZE;
        //int last = byteCount - (blockCount * MAX_BLOCK_SIZE);
        //for (int i = 0; i < blockCount; i++) {
        //    dataCharacteristic.writeValue(&buff[i * MAX_BLOCK_SIZE], MAX_BLOCK_SIZE);
        //}
        //if (last > 0) {
        //    dataCharacteristic.writeValue(&buff[blockCount * MAX_BLOCK_SIZE], last);
        //}
        ResetInBuffer();
    }
    else {
        //
        inIndex++;
        // Wipe out buffer if too long
        if (inIndex >= IN_BUFF_SIZE) {
            ResetInBuffer();
            Serial.println("Corrupt BT input. Buffer purged");
        }
    }
}


void ResetInBuffer() {
    memset(buff, 0, IN_BUFF_SIZE);
    inIndex = 0;
}


void DebugStreamIncoming(char value) {
    if (value == '\r') {
        Serial.write("CR");
        Serial.println("");
    }
    else if (value == '\n') {
        Serial.write("LN");
    }
    else {
        Serial.write(value);
    }
}


// Event handler for when data is written to the inByte characteristic
void inBytesWritten(BLEDevice device, BLECharacteristic  byteCharacteristic) {
    //Serial.println((char*)byteCharacteristic.value());

    digitalWrite(LED_BUILTIN, HIGH);
    // Only expecting 1 byte per write from outside
    byte value = byteCharacteristic.value()[0];
    //inByteCharacteristic.readValue(value);
    buff[inIndex] = value;
    DebugStreamIncoming(buff[inIndex]);

    // Direct push back to caller. Works well
    //outByteCharacteristic.writeValue(buff[inIndex]);


    digitalWrite(LED_BUILTIN, LOW);

    ProcessIncomingBuff();

}


void bleOnConnectHandler(BLEDevice central) {
    // central connected event handler
    Serial.print("CONNECTED, central: ");
    Serial.println(central.address());
}


void bleOnDisconnectHandler(BLEDevice central) {
    // central disconnected event handler
    Serial.print("DISCONNECTED, central: ");
    Serial.println(central.address());
}

