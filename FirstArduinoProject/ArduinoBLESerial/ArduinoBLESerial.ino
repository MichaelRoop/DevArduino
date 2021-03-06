
// Name:		ArduinoBLESerial.ino
// Created:	4/28/2020 8:55:58 PM
// Author:	Michael
//
// Sets up the BLE to simulate a serial connection by having one characteristic
// configured as an incoming channel, and another one as the outgoing
//
// Tested on the Arduino UNO WIFI Rev2
#include <ArduinoBLE.h>
#include "C:/Program Files (x86)/Arduino/hardware/tools/avr/avr/include/string.h"

#ifndef SECTION_DATA

const int MAX_BLOCK_SIZE = 20;
// BT spec 0x2A3D
const char* CHARACTERISTIC_STR_TYPE = "2a3d";
// BT spec 0x2901
const char* DESCCRIPTOR_CHARACTERISTIC_USER_DESCRIPTION = "2901";
// Custom Service Uuid 0x9999
const char* SERVICE_CUSTOME_UUID = "9999";
// BT spec 1792 Generic access
const int GENERIC_ACCESS_APPEARANCE = 1792;

// Create services that will act as a serial port. Max 20 bytes at a time
BLEService serialService(SERVICE_CUSTOME_UUID);

// Need a write in Setup with block of MAX_BLOCK_SIZE size OR it is not recognized by caller
BLECharacteristic outputCharacteristic(CHARACTERISTIC_STR_TYPE, BLERead | BLENotify, MAX_BLOCK_SIZE);
BLEDescriptor outputDescriptor(DESCCRIPTOR_CHARACTERISTIC_USER_DESCRIPTION, "Serial Output");

// Input characteristic
BLECharacteristic inputCharacteristic(CHARACTERISTIC_STR_TYPE, BLEWrite, MAX_BLOCK_SIZE);
BLEDescriptor inputDescriptor(DESCCRIPTOR_CHARACTERISTIC_USER_DESCRIPTION, "Serial Input");


bool hasInput = false;
#define IN_BUFF_SIZE 500
char buff[IN_BUFF_SIZE];
unsigned char inIndex = 0;

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
    
    // Name that users see in list for 'Add Device'
    BLE.setLocalName("Mikie BLE Serial");
    // The GenericAccess Characteristic - Device Name Descriptor
    BLE.setDeviceName("MR SerialBLE Device");
    BLE.setAppearance(GENERIC_ACCESS_APPEARANCE);

    // assign event handlers for connected, disconnected events
    BLE.setEventHandler(BLEConnected, bleOnConnectHandler);
    BLE.setEventHandler(BLEDisconnected, bleOnDisconnectHandler);

    // TODO - this writes to is which is not good I think
    setupSerialDescriptor(serialService, outputCharacteristic, outputDescriptor);

    // Setup the serial service - input
    //inputCharacteristic.addDescriptor(inputDescriptor);
    //serialService.addCharacteristic(inputCharacteristic);
    setupSerialDescriptor(serialService, inputCharacteristic, inputDescriptor);
    inputCharacteristic.setEventHandler(BLEWritten, inBytesWritten);


    // Add service to BLE and start advertising
    BLE.addService(serialService);
    BLE.setAdvertisedService(serialService);
    BLE.advertise();
    Serial.println("BLE awaiting connections...");
}


// the loop function runs over and over again until power down or reset
void loop() {
    BLEDevice central = BLE.central();
    if (central) {
        // This will hold up the loop function as long as the connection persists
        while (central.connected()) {
            // The message processing is done via the event handler for incoming bytes
            delayMicroseconds(100);
        }
    }
}


// Initialize the serial Characteristic, add Descriptor and connect to serial
void setupSerialDescriptor(BLEService& service, BLECharacteristic& characteristic, BLEDescriptor& desc) {
    byte initBuffContents[MAX_BLOCK_SIZE];
    // If we initialize with 0x00 (null) creates weird output in future writes
    memset(initBuffContents, 0x20, MAX_BLOCK_SIZE);
    characteristic.writeValue(initBuffContents, MAX_BLOCK_SIZE);
    characteristic.addDescriptor(desc);
    service.addCharacteristic(characteristic);
}




// TODO - sample code to process multi byte incoming data
//byte data[50];
//if (dataCharacteristic.valueUpdated()) {
//    dataCharacteristic.valueLength();
//    int count = dataCharacteristic.readValue(data, 50);
//    // At this point we would copy count # of bytes from the in block over to the main buffer
//    // TODO - need to preserve the block size copied. need start of index
//}


void ProcessIncomingBuff() {
    // Doing \n\r terminator sequence
    if (buff[inIndex] == '\r') {
        Serial.println("Printing msg in buffer");
        hasInput = true;

        // At this point we would parse the message to determine what to do

        //// For demo, just bounce the message back one byte at a time
        ////Serial.write(buff, inIndex + 1);
        //for (int i = 0; i <= inIndex; i++) {
        //    digitalWrite(LED_BUILTIN, HIGH);
        //    outByteCharacteristic.writeValue(buff[i]);
        //    digitalWrite(LED_BUILTIN, LOW);
        //}

        Serial.print("Sending back ");
        Serial.print(inIndex);
        Serial.println(" bytes to write");


        // Will not send more than 20 at a time
        //int result = bleCharString.writeValue(buff, 8);
        //int result = bleCharString.writeValue(buff, inIndex);
        //Serial.print("Write result ");
        //Serial.println(result);

        //// This works by iterating through 5 byte blocks. Suspect because had a 5 byte value on init
        //int count = inIndex / 5;
        //int last = (inIndex % 5);
        //int lastIndex = 0;
        //for (int i = 0; i < count; i++) {
        //    lastIndex = i * 5;
        //    bleCharString.writeValue(&buff[lastIndex], 5);
        //}
        //if (last > 0) {
        //    bleCharString.writeValue(&buff[lastIndex + 5], last);
        //}

        // This works by iterating through BLOCK_SIZE byte blocks.
        int count = inIndex / MAX_BLOCK_SIZE;
        int last = (inIndex % MAX_BLOCK_SIZE);
        int lastIndex = 0;
        for (int i = 0; i < count; i++) {
            lastIndex = i * MAX_BLOCK_SIZE;
            outputCharacteristic.writeValue(&buff[lastIndex], MAX_BLOCK_SIZE, false);
        }
        if (last > 0) {
            if (lastIndex > 0) {
                lastIndex += MAX_BLOCK_SIZE;
            }
            outputCharacteristic.writeValue(&buff[lastIndex], last, false);
        }




        //bleCharString.writeValue(buff, 5);
        //bleCharString.writeValue(&buff[5], 5);
        //bleCharString.writeValue(&buff[10], 4);






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
            Serial.println("Corrupt input purged");
        }
    }
}


void ResetInBuffer() {
    // Reset with spaces. Acts weird if you use 0x00 null)
    memset(buff, 0x20, IN_BUFF_SIZE);
    inIndex = 0;
}


void DebugStreamIncoming(char value) {
    // TODO - disable to see if it speeds up
    //if (value == '\r') {
    //    Serial.write("CR");
    //    Serial.println("");
    //}
    //else if (value == '\n') {
    //    Serial.write("LN");
    //}
    //else {
    //    Serial.write(value);
    //}
}


byte inBuff[MAX_BLOCK_SIZE];

// Event handler for when data is written to the inByte characteristic
void inBytesWritten(BLEDevice device, BLECharacteristic  byteCharacteristic) {
    digitalWrite(LED_BUILTIN, HIGH);

    int count = inputCharacteristic.readValue(inBuff, MAX_BLOCK_SIZE);
    ProcessIncomingBuff2(inBuff, count);

    //// Only expecting 1 byte per write from outside
    //byte value = byteCharacteristic.value()[0];
    ////inByteCharacteristic.readValue(value);
    //buff[inIndex] = value;
    //DebugStreamIncoming(buff[inIndex]);

    //// Direct push back to caller. Works well
    ////outByteCharacteristic.writeValue(buff[inIndex]);


    digitalWrite(LED_BUILTIN, LOW);

//    ProcessIncomingBuff();

}


void ProcessIncomingBuff2(uint8_t* inData, int length) {
    // index is pos of next write
    memcpy(&buff[inIndex], inData, length);

    int newIndex = inIndex + length;
    // This is where I would process the data. Remove complete command, etc
    inIndex = newIndex;

    Serial.write(inData, length); Serial.println("");
    Serial.write(buff, newIndex); Serial.println("");

    // Just bounce back whatever came in
    outputCharacteristic.writeValue(inData, length);
    // Check if there was a \n and simply obliterate main buffer
    for (int i = 0; i < length; i++) {
        if (inData[i] == '\n') {
            ResetInBuffer();
            break;
        }
    }






    /*
    // Doing \n\r terminator sequence
    if (buff[inIndex] == '\r') {
        Serial.println("Printing msg in buffer");
        hasInput = true;

        // At this point we would parse the message to determine what to do

        //// For demo, just bounce the message back one byte at a time
        ////Serial.write(buff, inIndex + 1);
        //for (int i = 0; i <= inIndex; i++) {
        //    digitalWrite(LED_BUILTIN, HIGH);
        //    outByteCharacteristic.writeValue(buff[i]);
        //    digitalWrite(LED_BUILTIN, LOW);
        //}

        Serial.print("Sending back ");
        Serial.print(inIndex);
        Serial.println(" bytes to write");


        // Will not send more than 20 at a time
        //int result = bleCharString.writeValue(buff, 8);
        //int result = bleCharString.writeValue(buff, inIndex);
        //Serial.print("Write result ");
        //Serial.println(result);

        //// This works by iterating through 5 byte blocks. Suspect because had a 5 byte value on init
        //int count = inIndex / 5;
        //int last = (inIndex % 5);
        //int lastIndex = 0;
        //for (int i = 0; i < count; i++) {
        //    lastIndex = i * 5;
        //    bleCharString.writeValue(&buff[lastIndex], 5);
        //}
        //if (last > 0) {
        //    bleCharString.writeValue(&buff[lastIndex + 5], last);
        //}

        // This works by iterating through BLOCK_SIZE byte blocks.
        int count = inIndex / MAX_BLOCK_SIZE;
        int last = (inIndex % MAX_BLOCK_SIZE);
        int lastIndex = 0;
        for (int i = 0; i < count; i++) {
            lastIndex = i * MAX_BLOCK_SIZE;
            outputCharacteristic.writeValue(&buff[lastIndex], MAX_BLOCK_SIZE);
        }
        if (last > 0) {
            if (lastIndex > 0) {
                lastIndex += MAX_BLOCK_SIZE;
            }
            outputCharacteristic.writeValue(&buff[lastIndex], last);
        }




        //bleCharString.writeValue(buff, 5);
        //bleCharString.writeValue(&buff[5], 5);
        //bleCharString.writeValue(&buff[10], 4);






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
    */
}


void bleOnConnectHandler(BLEDevice central) {
    // central connected event handler
    Serial.println("CONNECTED");
}


void bleOnDisconnectHandler(BLEDevice central) {
    // central disconnected event handler
    Serial.println("DISCONNECTED");
    ResetInBuffer();
}

