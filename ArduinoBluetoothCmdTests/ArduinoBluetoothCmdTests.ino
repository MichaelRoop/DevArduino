/*
 Name:		ArduinoBluetoothCmdTests.ino
 Created:	12/12/2019 2:44:52 PM
 Author:	Michael

 Sample to send commands to Arduino Bluetooth shield via serial monitor
 
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

 HC-05 supported baud rate: 9600,19200,38400,57600,115200,230400,460800.
 However, there are multiple errors over 115,200 when running on an UNO

*/
#include <SoftwareSerial.h>


// The jumpers on BT board are set to 4TX and 5RX. 
// They are reversed on serial since RX from BT gets TX to serial
SoftwareSerial btSerial(5, 4); //RX,TX


// the setup function runs once when you press reset or power the board
void setup() {
     //Need 38400 for BT and 9600 for serial to send AT commands
     //strange because BT AT cmd shows BT baud is 9600. 
    
     //If I set BT serial higher via an AT command, it still only 
     //works if I set the software serial for BT at 38400
    
     //Although we can set the default debug serial port higher 
     //9600 is adequate
    SetupCommunications(9600, 38400);
    //SetupCommunications(115200, 460800);
}


// the loop function runs over and over again until power down or reset
void loop() {
    SendReceiveBT_AT_Cmds();
}


void SetupCommunications(long dbgBaud, long btBaud) {
    Serial.begin(dbgBaud);
    btSerial.begin(btBaud);

    while (!Serial) {
        ; // wait for default serial port to connect. Needed for Native USB only
    }
    Serial.println("Debug serial active");
    // example had pin 9 set low, then high but does not seem necessary

    Serial.println("BT CMD Mode up and running");

    btSerial.write("AT\r\n");
    Serial.println(btSerial.readStringUntil('\n'));

    btSerial.write("AT+NAME?\r\n");
    Serial.println(btSerial.readStringUntil('\n'));

    btSerial.write("AT+UART\r\n");
    Serial.println(btSerial.readStringUntil('\n'));


    //Serial.println(btSerial.write("AT+NAME?"));
    //Serial.println(btSerial.write("AT+UART"));
    //
    //Serial.println(btSerial.read(""));
    
    //Serial.println(btSerial.write(""));
    //Serial.println(btSerial.write(""));


}

// Private methods

/**
 * Receive AT commands from VS serial monitor and pass them to Bluetooth module.
 * Set the serial monitor to add LF and CRL to outgoing
 * The shield must already be switched via toggle to CMD mode. Cannot be done
 * programatically.  Here is the configuration info from AT commands
 * https://www.itead.cc/wiki/BT_Shield_(Master_Slave)
 *
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
 *
 * incoming responses from Bluetooth are ended with \r\n
*/

#define IN_BUFF_SIZE 100
char buff[IN_BUFF_SIZE];
unsigned char inIndex = 0;

void SendReceiveBT_AT_Cmds() {

    //btSerial.

    // Stream any input from bluetooth to the debug serial stream
    // each entry 1 character (int). Each response finished with \r\n
    if (btSerial.available() > 0) {
        buff[inIndex] = (char)btSerial.read();

        Serial.write(buff[inIndex]);
        //btSerial.write(buff[inIndex]);

        if (buff[inIndex] == '\n') {
            //Serial.write(buff);
            // In a normal program we could see what msg came in and act on it
            // Not sure what comes in when in data mode
            memset(buff, 0, IN_BUFF_SIZE);
            inIndex = 0;
        }
        else {
            inIndex++;
            // Wipe out buffer if too long
            if (inIndex >= IN_BUFF_SIZE) {
                inIndex = 0;
                Serial.println("Corrupt BT input. Purge buffer");
            }
        }
    }
    else {
        //char[1] = {'a'};
       // btSerial.write("aa\r\n");
    }

    // This is the simple version which just pushes each character to debug serial
    //if (btSerial.available() > 0) {
    //	Serial.write(btSerial.read());
    //}


    // Get AT cmd from Visual Studio Serial monitor incoming stream to send to Bluetooth
    if (Serial.available() > 0) {
        btSerial.write(Serial.read());
    }
}


void SendReceiveBT_AT_CmdsEcho() {
}

