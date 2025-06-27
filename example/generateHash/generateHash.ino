
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
1. hash myHash
2. myHash.initalizeHashFile("myFile.bin", "write")
3. myHash.initalizeHashFile("myFile.bin", "read")
4. [Internal] myHash.hashFileStream()  [External] myHash.getIncomingFileHash()


updates:

0-0-0000 [COMPLETE/TESTED] 0-0-0000

*/


#include "generateHash.h"


hash myHash;

//File myFile;

/* FILE NAME CAN ONLY BE 8 CHARACTERS LONG*/
//String file = "TEST22.bin";

//bool transferComplete = true;
//uint8_t fileArray[20000];

uint8_t anotherArray[35] = { 0 };

void setup() {
  SerialUSB.begin(9600);
  pinMode(13, OUTPUT);
  // while (!SerialUSB) {
  //   ;  // wait for serial port to connect. Needed for native USB port only
  // }
  //Make sure we are connected to the SD card and are accessing the correct file
  // myHash.initalizeHashFile("file1.bin", "read");

  //SerialUSB.print("this is the file hash 1: ");
  //SerialUSB.println(myHash.getFileHash());
  //myHash.readFile();

  //myHash.initalizeHashFile(file, "read");
  //lets get the file size
  //SerialUSB.print("File size of " + file + " is: ");
  //SerialUSB.println(myHash.fileSize());

  //myHash.hashFileStream();

  // SerialUSB.print("this is the file hash: ");
  //SerialUSB.println(myHash.getFileHash());

  //initalize new file
  // myHash.initalizeHashFile("file1.bin", "write");
}

void loop() {
  // put your main code here, to run repeatedly:
  //myHash.writeFile();  //shouldnt need this anymore

  myHash.receiveCommand();


  if (myHash.transferComplete()) {
    myHash._fileTransferComplete = false;
    myHash._pushBinaryRemote = true;

    SerialUSB.println("file transfer complete");

    //Read from the file we just wrote to in order to get the hash
    myHash.initalizeHashFile("file1.bin", "read");
    myHash.hashFileStream();

    //lets print the incoming file hash
    SerialUSB.println(myHash.getIncomingFileHash());

    SerialUSB.println(myHash.getFileHash());

    if (myHash.getIncomingFileHash() == myHash.getFileHash()) {
      SerialUSB.println("match");
      //SerialUSB.print("ptr for _arrayHash: ");
      // uint8_t anotherArray[35] = {0};

      memcpy(anotherArray, myHash.getFileHashPtr(), sizeof(anotherArray));
     // *** Printing out the address takes up to much time and messes up the array printing ***
     // SerialUSB.println((unsigned long)myHash.getFileHashPtr());
      for (int i = 0; i < sizeof(anotherArray); i++) {
        SerialUSB.println(anotherArray[i]);
      }
    } else {
      SerialUSB.println("no match");
    }
  }
}
