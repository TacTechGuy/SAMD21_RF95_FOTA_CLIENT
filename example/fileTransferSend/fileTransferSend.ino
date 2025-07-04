/*

      Author: Ethan Dechant
Date created: [Start] October 28th, 2024   


This Example show how to write to an SD card from the computer, hash the 
data and then check the hash with what is sent. The computer initally hashes
the file when it opens it and then enbeds this hash in an array that is 35 bytes
long, out of the 35 bytes the first three indexes, index[0] is't used and index[1]
& index[2] are used to get the file size while indexes[3-34] are used to extract
the hash that was sent by the computer. Once the filezie is reached (subracting the 
bytecount) from the array[35] size of 35 will then allow for a condition to close()
the SD file which handles the writing of the file. Hasing the file once it is made, 
we need to make another call but this time to read from the file and then we can 
use the function hashFileStream() to extract the hash from the newly created file.   

STEPS:
1. make an object for the hash class
2. initalize SD File [file name, read or write]   #file name CANNOT exceed 8 characters
3. reinitalize the SD file for the opposite [file name, read or write] opposite of before
4. hash the file and or get the incoming hash

STEPS:
1. hash firmwareUpdate
2. firmwareUpdate.initalizeHashFile("myFile.bin", "write")
3. firmwareUpdate.initalizeHashFile("myFile.bin", "read")
4. [Internal] firmwareUpdate.hashFileStream()  [External] firmwareUpdate.getIncomingFileHash()


updates:

0-0-0000 [COMPLETE/TESTED] 0-0-0000

*/


#include "fileTransfer.h"

#include <RHEncryptedDriver.h>
#include <Speck.h>
#include <RH_RF95.h>
Speck myCipher;

const int RFM_95CS = 12;
const int RFM_95INT = 6;
const int RFM_95RST = 7;
double frequency = 921.2;

RH_RF95 rf95(RFM_95CS, RFM_95INT);
RHEncryptedDriver myDriver(rf95, myCipher);

unsigned char encryptKey[16] = **REDACTED**;

// Object for the fileTransfer Class
firmware firmwareUpdate;



/* FILE NAME CAN ONLY BE 8 CHARACTERS LONG*/
//String file = "TEST22.bin";

//uint8_t anotherArray[35] = { 0 };



void firmware::sendDataRF(uint8_t* dataArray) {

  myDriver.send((uint8_t*)&dataArray, sizeof(dataArray));
}

void setup() {
  SerialUSB.begin(9600);
  pinMode(13, OUTPUT);
  // while (!SerialUSB) {
  //   ;  // wait for serial port to connect. Needed for native USB port only
  // }
  //Make sure we are connected to the SD card and are accessing the correct file
  // firmwareUpdate.initalizeHashFile("file1.bin", "read");

  //SerialUSB.print("this is the file hash 1: ");
  //SerialUSB.println(firmwareUpdate.getFileHash());
  //firmwareUpdate.readFile();

  //firmwareUpdate.initalizeHashFile(file, "read");
  //lets get the file size
  //SerialUSB.print("File size of " + file + " is: ");
  //SerialUSB.println(firmwareUpdate.fileSize());

  //firmwareUpdate.hashFileStream();

  // SerialUSB.print("this is the file hash: ");
  //SerialUSB.println(firmwareUpdate.getFileHash());

  //initalize new file
  // firmwareUpdate.initalizeHashFile("file1.bin", "write");

  
  //-------------------//
  //  SETUP THE RADIO  //
  //-------------------//
  if (rf95.init() == false) {
    //while(1);
    SerialUSB.println("RF radio not attached, test!");
  } else {
    digitalWrite(LED_BUILTIN, HIGH);  
    rf95.setFrequency(921.2);
    delay(100);

    rf95.setTxPower(15, false);
    delay(100);

    myCipher.setKey(encryptKey, 16);
    delay(100);
    SerialUSB.println("RF Receiver- Good to Go!");
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void loop() {
  //------------------------------//
  // HANDLES SERIAL COMMUNICATION //
  //------------------------------//
  firmwareUpdate.receiveCommand();



}