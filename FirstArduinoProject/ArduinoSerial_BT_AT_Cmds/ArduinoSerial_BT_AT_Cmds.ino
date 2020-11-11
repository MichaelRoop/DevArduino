/*
 Name:		ArduinoSerial_BT_AT_Cmds.ino
 Created:	11/11/2020 1:08:41 PM
 Author:	Michael

 To enable the Multi Comm Terminal to send AT commands to the 
 HC-05 module

 Module must be set to CMD via switch.  
 If the module is software switched I will need to add 
 that in a command

  Written and tested in Visual Studio using Visual Micro
 Tested against Arduino Uno and itea Bluetooth shield with HC-05 module

 MUST HAVE BOARD TOGGLED TO CMD
 MUST HAVE BOARD JUMPERS SET FOLLOWING for IO4 TX and IO5 RX
 같�|같
 같|같�
 같같같
 MUST HAVE DEBUG SERIAL SET TO 9600 Baud
 MUST HAVE BT SERIAL SET TO 38400 Baud

  * AT 			    +NAME:itead
 * AT+VERSION 	    +VERSION:hc01.comV2.1
 * AT+ADDR		    +ADDR:2016:4:76101
 * AT+UART		    +UART:9600,0,0
 * AT+NAME=<param>  OK set the module name
 * AT+NAME?         +NAME:<Param> Get the module name
 * AT+ORGL          OK - restore to defaults
 * AT+RESET         OK - reset
 *
 * AT+ROLE		    +ROLE:0 (1=master, 0=slave)
 * AT+PSWD		    +PSWD:1234 (pairing PIN)

*/
#include <SoftwareSerial.h>

#define IN_BUFF_SIZE 100
char buff[IN_BUFF_SIZE];
unsigned char inIndex = 0;

// The jumpers on BT board are set to 4TX and 5RX. 
// They are reversed on serial since RX from BT gets TX to serial
SoftwareSerial btSerial(5, 4); //RX,TX


// the setup function runs once when you press reset or power the board
void setup() {
    //Need 38400 for BT and 9600 for serial to send AT commands
    //strange because BT AT cmd shows BT baud is 9600. 
    SetupCommunications(9600, 38400);
}


// the loop function runs over and over again until power down or reset
void loop() {
    int count = Serial.available();
    if (count > 0) {
        Serial.readBytes(buff, count);
        btSerial.write(buff, count);
    }

    count = btSerial.available();
    if (count > 0) {
        btSerial.readBytes(buff, count);
        Serial.write(buff, count);
    }
}


void SetupCommunications(long dbgBaud, long btBaud) {
    Serial.begin(dbgBaud);
    btSerial.begin(btBaud);
    while (!Serial) {
        ; // wait for default serial port to connect. Needed for Native USB only
    }
}

