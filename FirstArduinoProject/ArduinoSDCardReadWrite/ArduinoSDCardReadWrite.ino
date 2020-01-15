/*
 Name:		ArduinoSDCardReadWrite.ino
 Created:	1/15/2020 10:27:24 AM
 Author:	Michael
*/

#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

// the setup function runs once when you press reset or power the board
void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(115400);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }

    if (!InitSD(chipSelect)) {
        while (1);
    }
    ListAllSdFiles();
    PrintFileContent("File1.txt");
    PrintFileContent("File2.txt");
    PrintFileContent("/Dir1/F3Dir1.txt");

    CreateFile("fx1.txt");
    ListAllSdFiles();
    PrintAllSdFileContents();
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

void PrintFileContent(char* name) {

    // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
    File file = SD.open(name);

    // if the file is available, write to it:
    if (file) {
        Serial.print("File Contents for: ");
        Serial.println(name);
        while (file.available()) {
            Serial.write(file.read());
        }
        file.close();
        Serial.println("");
    }
    // if the file isn't open, pop up an error:
    else {
        Serial.print("Error opening: ");
        Serial.println(name);
    }
}

void ListAllSdFiles() {
    File root;
    root = SD.open("/");
    PrintDirectory(root, 0);
    root.close();
}

void PrintAllSdFileContents() {
    File root;
    root = SD.open("/");
    PrintDirectoryFileContents(root);
    root.close();
}


void PrintDirectory(File dir, int numSpace) {
    while (true) {

        File entry = dir.openNextFile();
        if (!entry) {
            // no more files
            break;
        }
        for (uint8_t i = 0; i < numSpace; i++) {
            Serial.print(" ");
        }
        Serial.print(entry.name());
        if (entry.isDirectory()) {
            Serial.println("/");
            PrintDirectory(entry, numSpace + strlen(entry.name()) + 1);
        }
        else {
            // files have sizes, directories do not
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
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

void CreateFile(char* name) {
    DeleteFile(name);
    Serial.print("Create file: ");
    Serial.println(name);
    File file = SD.open(name, FILE_WRITE);

    if (file == NULL) {
        Serial.println("FAILED TO CREATE");
    }
    else {
        file.close();
    }
}


void PrintDirectoryFileContents(File dir) {
    while (true) {

        File entry = dir.openNextFile();
        if (!entry) {
            // no more files
            break;
        }

        Serial.print(entry.name());
        if (entry.isDirectory()) {
            Serial.print("/");
            PrintDirectoryFileContents(entry);
        }
        else {
            Serial.println("");
            
            
            if (strstr(entry.name(), ".txt") != NULL || strstr(entry.name(), ".TXT") != NULL) {
                // Indent line
                Serial.print(" ");
                while (entry.available()) {
                    int16_t c = entry.read();
                    Serial.write(c);
                    if (((char)c) == '\n') {
                        // indent next line
                        Serial.print(" ");
                    }

                    //Serial.write(entry.read());
                }
            }
            Serial.println("");

            //// files have sizes, directories do not
            //Serial.print("\t\t");
            //Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}
