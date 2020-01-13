/*
 Name:		ArduinoEthernetShieldTests.ino
 Created:	12/16/2019 3:43:40 PM
 Author:	Michael


 Code found and modified from 
 https://www.instructables.com/id/Arduino-Ethernet-Shield-Tutorial/

 Ethernet shield attached to pins 10, 11, 12, 13

*/
#include <IPAddress.cpp>
#include <Ethernet.h>

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
IPAddress ip(192, 168, 1, 110); //<<< ENTER YOUR IP ADDRESS HERE!!!
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(255, 255, 255, 0);

EthernetServer server(80);

bool attempDHCP = false;

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for default serial port to connect. Needed for Native USB only
    }
    Serial.println("Debug serial active");

    if (attempDHCP && Ethernet.begin(mac)) {
        Serial.println("Attempt Initialise IP with DHCP");
        Serial.println("DHCP Successful");
        ip = Ethernet.localIP();
    }
    else {
        Serial.println("Attempt Initialise IP manually");
        Ethernet.begin(mac, ip);
    }

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
        Serial.println("Failed Ethernet setup");
        while (true) {
            delay(1);
        }
    }
    else if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
        Serial.println("Failed Ethernet setup");
        while (true) {
            delay(1);
        }
    }
    else {
        Serial.println("Ethernet setup");
    }



    //// For getting the IP from an DHCP server
    //if (Ethernet.begin(mac) == 0) {
    //    Serial.println("Initialise IP with DHCP");
    //    Serial.println("Failed to configure Ethernet using DHCP");
    //    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    //        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    //    }
    //    else if (Ethernet.linkStatus() == LinkOFF) {
    //        Serial.println("Ethernet cable is not connected.");
    //    }
    //    else {
    //        // Do manual IP setup
    //        Serial.println("No DHCP - setting manually - make sure client on same network");
    //        Ethernet.begin(mac, ip);
    //            //Serial.println("Failed to configure Ethernet manually");
    //            if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    //                Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    //            }
    //            else if (Ethernet.linkStatus() == LinkOFF) {
    //                Serial.println("Ethernet cable is not connected.");
    //            }
    //        
    //    }
    //    //// no point in carrying on, so do nothing forevermore:
    //    //while (true) {
    //    //    delay(1);
    //    //}
    //}
    //else {
    //    ip = Ethernet.localIP();
    //}

    //Ethernet.begin(mac, ip);

    // print your local IP address:
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());

    server.begin();
    Serial.println("Ethernet server begun");



}

bool isDark = false;
int count = 0;

// the loop function runs over and over again until power down or reset
void loop() {
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
        // an http request ends with a blank line
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = (char)client.read();
                //Serial.print("Got data:");
                //if (c == '\r') {
                //    Serial.println('\\r');
                //}
                //else if (c == '\n') {
                //    Serial.println('\\n');
                //}
                //else {
                //    Serial.print(c);
                //}

                // For this simple test just check for end of line and send back a response
                if (c == '\n' && currentLineIsBlank) {

                    //Serial.println("Got \\n sending response OK");


                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println();
                    client.println("<!DOCTYPE html>");

                   //Toggles a different version of a website each time a refresh happens. Two complete lines per refresh
                    count++;
                    if (count % 2 == 0) {
                        isDark = !isDark;
                    }

                    if (isDark) {
                        client.print("<html>");
                        client.print("<body style='background-color:darkblue; color:white'>");
                        client.print("<h1>DARK!</h1>");
                        client.print("</body>");
                        client.println("</html>");
                    }
                    else {
                        client.print("<html>");
                        client.print("<body style='background-color:powderblue;'>");
                        client.print("<h1>LIGHT!</h1>");
                        client.print("</body>");
                        client.println("</html>");
                    }



                    break;
                }
                if (c == '\n') {
                    // you're starting a new line
                    currentLineIsBlank = true;
                }
                else if (c != '\r') {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }
            }
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();

        //Serial.println("Client not connected");
    }
}
