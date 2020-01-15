/*
 Name:		ArduinoSDCard.ino
 Created:	1/15/2020 9:25:49 AM
 Author:	Michael
*/

#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 4;

// the setup function runs once when you press reset or power the board
void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(115400);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("Serial port started");

    if (!InitSDCard(&card)) {
        while (1);
    }
    PrintCardType(&card);

    if (!InitCardVolume(&card, &volume)) {
        while (1);
    }
    PrintSdVolumeClusterInfo(&volume);
    PrintSdVolumeInfo(&volume);
    PrintSdFileList(&volume);
    PrintFileContent("File1.txt");
    PrintFileContent("File2.txt");
    PrintFileContent("DIR1/F3Dir1.txt");

}

// the loop function runs over and over again until power down or reset
void loop() {
  
}

bool InitSDCard(Sd2Card* sdCard) {
    // we'll use the initialization code from the utility libraries
    // since we're just testing if the card is working!
    if (!sdCard->init(SPI_HALF_SPEED, chipSelect)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card inserted?");
        Serial.println("* is your wiring correct?");
        Serial.println("* did you change the chipSelect pin to match your shield or module?");
        return false;
    }

    Serial.println("Wiring is correct and a card is present.");
    return true;
}

void PrintCardType(Sd2Card* sdCard) {
    // print the type of card
    Serial.println();
    Serial.print("Card type:         ");
    switch (sdCard->type()) {
    case SD_CARD_TYPE_SD1:
        Serial.println("SD1");
        break;
    case SD_CARD_TYPE_SD2:
        Serial.println("SD2");
        break;
    case SD_CARD_TYPE_SDHC:
        Serial.println("SDHC");
        break;
    default:
        Serial.println("Unknown");
    }
}

bool InitCardVolume(Sd2Card* sdCard, SdVolume* sdVolume) {
    // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
    if (sdVolume->init(sdCard)) {
        return true;
    }
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return false;
}

void PrintSdVolumeClusterInfo(SdVolume* sdVolume) {
    Serial.print("Clusters:          ");
    Serial.println(sdVolume->clusterCount());
    Serial.print("Blocks x Cluster:  ");
    Serial.println(sdVolume->blocksPerCluster());
    Serial.print("Total Blocks:      ");
    Serial.println(sdVolume->blocksPerCluster() * sdVolume->clusterCount());
    Serial.println();
}

void PrintSdVolumeInfo(SdVolume* sdVolume) {
    // print the type and size of the first FAT-type volume
    Serial.print("Volume type is:    FAT");
    Serial.println(sdVolume->fatType(), DEC);

    uint32_t volumesize;
    volumesize = sdVolume->blocksPerCluster();    // clusters are collections of blocks
    volumesize *= sdVolume->clusterCount();       // we'll have a lot of clusters
    volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
    Serial.print("Volume size (Kb):  ");
    Serial.println(volumesize);
    Serial.print("Volume size (Mb):  ");
    volumesize /= 1024;
    Serial.println(volumesize);
    Serial.print("Volume size (Gb):  ");
    Serial.println((float)volumesize / 1024.0);
}

void PrintSdFileList(SdVolume* sdVolume) {
    Serial.println("\nFiles found on the card");
    Serial.println("name\tdate\tsize in bytes): ");
    root.openRoot(sdVolume);

    // list all files in the card with date and size
    root.ls(LS_R | LS_DATE | LS_SIZE);
}

void PrintFileContent(char* name) {

    // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
    File dataFile = SD.open(name);

    // if the file is available, write to it:
    if (dataFile) {
        Serial.print("File Contents for: ");
        Serial.println(name);
        while (dataFile.available()) {
            Serial.write(dataFile.read());
        }
        dataFile.close();
        Serial.println("");
    }
    // if the file isn't open, pop up an error:
    else {
        Serial.print("Error opening: ");
        Serial.println(name);
    }
}