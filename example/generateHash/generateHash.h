/*

      Author: Ethan Dechant
Date created: [Start] October 28th, 2024   


This Example show how to write to an SD card from the computer, hash the 
data and then check the hash with what is sent. The computer initially hashes
the file when it opens it and then embeds this hash in an array that is 35 bytes
long, out of the 35 bytes the first three indexes, index[0] isn't used and index[1]
& index[2] are used to get the file size while indexes[3-34] are used to extract
the hash that was sent by the computer. Once the file size is reached (subtracting the 
bytecount) from the array[35] size of 35 will then allow for a condition to close()
the SD file which handles the writing of the file. Hashing the file once it is made, 
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
1-4-2025 [TESTED/COMPLETE] 1-7-2024   
  DESCRIPTION: Have the ability to control file transfer by using a command
               structure, I was also able to pack all of functions of the file
               transfer within the receiveCommand() which allows for better 
               integration as I'm not having to write additional code for the 
               SAMD21g18A (Transmitter), will have to figure out what I'm doing
               on the (receiver) side as I may have to rewrite or add functions.

               Steps within the GUI
               1. Open file 
               2. file transfer (command entered) --> moves command to the file transfer block
               3. Send binary 
               4. Upon receiving the binary, the SAMD21 will write the binary to a SD card, Hash 
                  the file and then compare the Hash with what is sent from the computer first 35
                  bytes are ignored as they contain the hash along with the file size. 
               5. Once file is hashed and compared, the results are sent to the computer. 

      ---- NEXT STEPS ARE TO FIGURE OUT HOW TO BREAK THE FILE UP AND SEND -----
      ******  WRITE TO SPI FLASH MEMORY OR CREATE AN ARRAY OF 20K TO DO THE UPLOADING ******
                                             
1-12-2025 [TESTED/COMPLETE] 1-7-2024    
  FUNCTIONS ADDED: getFileHashPtr();
  DESCRIPTION: Added the function to be able to return the ptr of the _arrayHash so that way 
               from outside the library we are able to memcpy from that array to an array
               that will be sent with the binary file data to the remote RF unit. This has 
               been tested and seems to work as expected. Make sure to called the method from
               outside of the library and within the main program. 
                                        
6-26-2025 [IN PROGRESS...] 
  DESCRIPTION: Adding functionality to the shooting target GUI. Everything serially works from
               the computer to the board that's used to send the binary file, now I'm working on 
               incorporating the sendBinaryFileRemote code into the library code, it will be embedded 
               into where the command coming from the computer is "upload remote". I need to add another
               method to handle the actual sending of the file remotely. Also need to change file names 
               and class name to better reference what the class is ment to do. 
*/

#ifndef _generateHash_h_
#define _generateHash_h_

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>



// My hash class
class hash {
public:

  // Start the SD card and then
  bool initalizeHashFile(const char file[], const char readWrite[]);

  // Read from the file
  void readFile();

  // Write to a file
  void writeFile();

  // Used as the main command block for receiving input from computer
  void receiveCommand();

  // Just a flag to let us know when the transfer is complete
  bool transferComplete();

  // Returns the size of the file to be hashed
  uint32_t fileSize();

  //This is used to input
  void hashFileStream();

  // Used to convert just the hash array to String
  void convertHashArray2String(uint8_t * arrayToConvert, uint8_t size);

  //Will return the _hashValue
  String getFileHash();

  //Returns a pointer to the _arrayHash so it can then be memcpy to the hash --> fileData.hash
  uint8_t * getFileHashPtr();

  // Retreives the hash from the inbound array
  String getIncomingFileHash();

  // Used to reset variables  in order to be able to send files consecutively 
  void resetFileWriteVariables();

  bool _fileTransferComplete = false;

  bool _pushBinaryRemote = false;


private:
  //This will be called once the hashes match
  void prepareForFileUpload();
  uint8_t _value = 0;
  int _fileSize = 0;
  int _byteCount = 1;
  bool _dataFlag, _fileData = false;
  uint8_t _arrayHash[35];
  uint8_t _b = 0;

  // Timer
  unsigned long currentTime, previousTime, delay = 10000;




  String _hashValue = "";  //store the hash
  File _myFile;
  String _file;
  uint8_t _readWriteFlag = 0xFF;

  String _command = " ";
  int _inValue = 0;
  uint8_t _commandMain = 0;
};

#endif