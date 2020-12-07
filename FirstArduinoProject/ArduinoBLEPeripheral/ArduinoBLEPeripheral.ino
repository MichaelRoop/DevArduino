/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald


https://github.com/arduino-libraries/ArduinoBLE/blob/master/examples/Peripheral/BatteryMonitor/BatteryMonitor.ino


 */




#include <ArduinoBLE.h>


#ifndef SECTION_DATA

#define MAX_BLOCK_SIZE 20

// Services
BLEService dataService("18EF");

// Characteristics
// Out channel 0x99,0x98 = 39,320 base 10. Caller reads or gets notifications from this device
// Need a write in Setup with block of MAX_BLOCK_SIZE size OR it is not recognized by caller
BLECharacteristic outputCharacteristic("9998", BLERead | BLENotify, MAX_BLOCK_SIZE);

// In channel 0x99,0x97 = 39,319 base 10. Caller writes to this device 
//BLEByteCharacteristic inputCharacteristic("9997", BLEWrite);
BLECharacteristic inputCharacteristic("9997", BLEWrite, MAX_BLOCK_SIZE);

// 0x2901 is CharacteristicUserDescription data type
BLEDescriptor outputDescriptor("2901", "Data Output");
BLEDescriptor inputDescriptor("2901", "Data Input");

bool hasInput = false;
#define IN_BUFF_SIZE 500
char buff[IN_BUFF_SIZE];
unsigned char inIndex = 0;
byte inBuff[MAX_BLOCK_SIZE];

#endif // !SECTION_DATA





// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(9600);
    while (!Serial) {}
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
    BLE.setLocalName("Mikies Arduino data device");
    // The Device Name Characteristic
    BLE.setDeviceName("Mikies data device char");

    // assign event handlers for connected, disconnected events
    BLE.setEventHandler(BLEConnected, bleOnConnectHandler);
    BLE.setEventHandler(BLEDisconnected, bleOnDisconnectHandler);

    setupDescriptor(dataService, outputCharacteristic, outputDescriptor);

    // Setup the serial service - input
    setupDescriptor(dataService, inputCharacteristic, inputDescriptor);
    inputCharacteristic.setEventHandler(BLEWritten, inBytesWritten);

    BLE.setConnectionInterval(0x0006, 0x0FFF);

    // Add service to BLE and start advertising
    BLE.addService(dataService);
    BLE.setAdvertisedService(dataService);
    BLE.advertise();
    Serial.println("BLE data device active, waiting for connections...");
    
    //Serial.print("Name:"); Serial.println(BLELocalDevice::);

}


// the loop function runs over and over again forever
void loop() {
    BLEDevice central = BLE.central();

    // if a central is connected to the peripheral:
    if (central) {
        Serial.print("Connected to central: ");
        // print the central's BT address:
        Serial.println(central.address());
        // turn on the LED to indicate the connection:
        digitalWrite(LED_BUILTIN, HIGH);

        // check the battery level every 200ms
        // while the central is connected:
        while (central.connected()) {

            // could use this for a heartbeat    
            //long currentMillis = millis();
            //// if 200ms have passed, check the battery level:
            //if (currentMillis - previousMillis >= 200) {
            //    previousMillis = currentMillis;
            //    updateBatteryLevel();
            //}
        }
        // when the central disconnects, turn off the LED:
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print("Disconnected from central: ");
        Serial.println(central.address());
    }





}




#ifndef SECTION_EVENT_HANDLERS

void bleOnConnectHandler(BLEDevice central) {
    // central connected event handler
    Serial.print("CONNECTED, central: ");
    Serial.println(central.address());
}


void bleOnDisconnectHandler(BLEDevice central) {
    // central disconnected event handler
    Serial.print("DISCONNECTED, central: ");
    Serial.println(central.address());
    ResetInBuffer();
}

// Event handler for when data is written to the inByte characteristic
void inBytesWritten(BLEDevice device, BLECharacteristic  byteCharacteristic) {
    digitalWrite(LED_BUILTIN, HIGH);

    int count = inputCharacteristic.readValue(inBuff, MAX_BLOCK_SIZE);
    ProcessIncomingBuff2(inBuff, count);
    digitalWrite(LED_BUILTIN, LOW);
}


#endif

#ifndef SECTION_PRIVATE

void ResetInBuffer() {
    // Reset with spaces. Acts weird if you use 0x00 null)
    memset(buff, 0x20, IN_BUFF_SIZE);
    inIndex = 0;
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
}


// Initialize the serial Characteristic, add Descriptor and connect to serial
void setupDescriptor(BLEService& service, BLECharacteristic& characteristic, BLEDescriptor& desc) {
    byte initBuffContents[MAX_BLOCK_SIZE];
    // If we initialize with 0x00 (null) creates weird output in future writes
    memset(initBuffContents, 0x20, MAX_BLOCK_SIZE);
    characteristic.writeValue(initBuffContents, MAX_BLOCK_SIZE);
    characteristic.addDescriptor(desc);
    service.addCharacteristic(characteristic);
}

#endif // !SECTION_PRIVATE




