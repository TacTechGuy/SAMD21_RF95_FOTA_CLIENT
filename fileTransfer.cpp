#include <sys/types.h>
#include <string.h>
#include <stdint.h>
/*

      Author: Ethan Dechant
Date created: [Start] October 28th, 2024   


This Example show how to write to an SD card from the computer, hash the 
data and then check the hash with what is sent. The computer initally hashes
the file when it opens it and then enbeds this hash in an array that is 35 bytes
long, out of the 35 bytes the first three indexes, index[0] isn't used and index[1]
& index[2] are used to get the file size while indexes[3-34] are used to extract
the hash that was sent by the computer. Once the filezie is reached (subracting the 
bytecount) from the array[35] size of 35 will then allow for a condition to close()
the SD file which handles the writing of the file. Hasing the file once it is made, 
we need to make another call but this time to read from the file and then we can 
use the function _hashFileStream() to extract the hash from the newly created file.   

STEPS:
1. make an object for the fileTransfer class
2. initalize SD File [file name, read or write]   #file name CANNOT exceed 8 characters
3. reinitalize the SD file for the opposite [file name, read or write] opposite of before
4. hash the file and or get the incoming hash

STEPS:
1. fileTransfer firmwareUpdate
2. firmwareUpdate.initalizeHashFile("myFile.bin", "write")
3. firmwareUpdate.initalizeHashFile("myFile.bin", "read")
4. [Internal] firmwareUpdate._hashFileStream()  [External] firmwareUpdate.getIncomingFileHash()


updates:

0-0-0000 [COMPLETE/TESTED] 0-0-0000

[IDEAS] --> change the commands 
         FROM:               TO:
     'push binary'       'upload remote'  **changed 6-10-2025**

              

*/
#include "fileTransfer.h"
#include "wiring_digital.h"
#include "SD.h"
#include "USB/USBAPI.h"
#include "variant.h"
#include "secrets.h"



fileTransfer::fileTransfer(uint8_t csPin, uint8_t interruptPin, uint8_t SD_pinInt, const char file[])
  : rf95(csPin, interruptPin), myDriver(rf95, myCipher) {
    myCipher.setKey(encryptKey, sizeof(encryptKey));
  strncpy(_file, file, sizeof(_file) - 1);
  _SDpin = SD_pinInt;

  // Old way utilizing String for the file name
  // _file = file;
}


//Single function to connect and either open the file or make the file READ/WRITE
//convert from String to char
bool fileTransfer::_initalizeHashFile(const char readWrite[]) {
  _resetFileWriteVariables();


  if (readWrite == "read") {
    //set the _readWriteFlag to 0x01
    _readWriteFlag = 0x01;
  } else if (readWrite == "write") {
    //set the _readWriteFlag to 0x02
    _readWriteFlag = 0x02;
  } else {
    _readWriteFlag = 0xFF;
  }

  bool initalized = true;


  //Make sure that we are connected to the SD card
  if (!SD.begin(_SDpin)) {
    initalized = false;
  }

  //Lets open the file for reading if we receive an Ox01
  if (_readWriteFlag == 0x01) {
    _myFile = SD.open(_file);
  } else if (_readWriteFlag == 0x02) {
    SerialUSB.println("making file...");
    //Check to see if there is a file by that name already if so delete it
    if (SD.exists(_file)) {
      SD.remove(_file);
    }
    _myFile = SD.open(_file, FILE_WRITE);
  }

  //SerialUSB.print("read write flag: ");
  //SerialUSB.println(_readWriteFlag);
  return initalized;
}

void fileTransfer::_resetFileWriteVariables() {
  _b = 0;
  _byteCount = 1;
  _fileSize = 0;
  __fileInfo = false;
  // Zero out the hashArray
  // for (int i = 0; sizeof(_arrayHash); i++) {
  //   _arrayHash[i] = 0;
  // }
}

// Main command block to handle the input coming from the computer
void fileTransfer::receiveCommandLocal() {



  if (_commandMain == 0) {
    while (SerialUSB.available() > 0) {

      char c = SerialUSB.read();
      // Old way 7-9-2025
      //_command += c;

      _inValue = (uint8_t)c;

      if (c != '\n') {
        incomingCommand[index] = c;
        index++;
        if (index >= sizeof(incomingCommand)) {
          index = sizeof(incomingCommand) - 1;
        }
      } else if (c == '\n') {
        incomingCommand[index] = '\0';
        _readCommand = true;
        index = 0;
      }
    }
    if (_readCommand) {
      SerialUSB.println("main");
      //SerialUSB.println(incomingCommand);

      //no line ending
      if (strcmp(incomingCommand, "file transfer") == 0) {
        _pushBinaryRemote = false;
        //SerialUSB.println("this is working correctly");

        //let initalize the file first... "file1.bin"
        _initalizeHashFile("write");

        _commandMain = 1;
        // transferFlag = true;
        SerialUSB.println("file transfer block");
        _previousTime = millis();
      }
      //---------------------//
      // SENDING FILE REMOTE //
      //---------------------//
      else if ((strcmp(incomingCommand, "upload remote") == 0) && (_pushBinaryRemote == true)) {
        SerialUSB.println("remote transfer block");  // used for the visual basic code so we can't remove
        fileData.remoteStatus = 0x25;
        SerialUSB.println("inside of the upload command");

        // Check remote status
        if (fileData.remoteStatus == 0x25) {

          memcpy(fileDataBuf, &fileData, sizeof(fileData));
          myDriver.send((uint8_t*)&fileDataBuf, sizeof(fileData));
          //sendDataRF((uint8_t*)&fileDataBuf); **
          myDriver.waitPacketSent();
          //waitToSendDataRF(); **
        }
      } else if (strcmp(incomingCommand, "get state") == 0) {
        SerialUSB.println("main");
      }
      //_command = "";
      _readCommand = false;
    }
  } else if (_commandMain == 1) {
    if (SerialUSB) {
      SerialUSB.println("file transfer block");
    }
    _writeFile();
  }

  if (_inValue == 1) {
    digitalWrite(LED_BUILTIN, HIGH);
    SerialUSB.println("light on");
    _inValue = 0xFF;
  }
  if (_inValue == 0) {
    digitalWrite(LED_BUILTIN, LOW);
    SerialUSB.println("light off");
    _inValue = 0xFF;
  }

  if (_transferComplete()) {
    _fileTransferComplete = false;
    _pushBinaryRemote = true;

    SerialUSB.println("file transfer complete");

    //Read from the file we just wrote to in order to get the hash
    _initalizeHashFile("read");
    _hashFileStream();

    //lets print the incoming file hash
    char* _fileHash = _getIncomingFileHash("sender");
    SerialUSB.println(_fileHash);
    //SerialUSB.println(getIncomingFileHash());

    SerialUSB.println(_hashValue);

    //.c_str()
    if (strcmp(_fileHash, _hashValue) == 0) {
      //if (getIncomingFileHash() == _hashValue){
      SerialUSB.println("match");
      //SerialUSB.print("ptr for _arrayHash: ");
      // uint8_t fileData.hash[35] = {0};

      memcpy(fileData.hash, &_arrayHash, sizeof(fileData.hash));
      // *** Printing out the address takes up to much time and messes up the array printing ***
      // OPEN THE FILE
      _myFile = SD.open(_file);


      binaryData.iterationCount = _myFile.size() / sizeof(binaryData.sendData);
      binaryData.leftOver = _myFile.size() % sizeof(binaryData.sendData);

      // DEBUG --> used to show the interationCount and leftover
      //SerialUSB.println(binaryData.iterationCount);
      //SerialUSB.println(binaryData.leftOver);

      _myFile.close();
      // **DEBUG**
      // for (int i = 0; i < sizeof(fileData.hash); i++) {
      //   SerialUSB.println(fileData.hash[i]);
      // }
    } else {
      SerialUSB.println("no match");
    }
  }

  //-------------------------//
  //   SENDING BINARY DATA   //
  //-------------------------//
  if (fileData.transferBegin) {
    if (_myFile) {
      SerialUSB.println("Sending File...");
      while (_myFile.available()) {

        //then use a for loop to populate the array and get it ready to send
        if (_count <= binaryData.iterationCount) {
          for (int i = 0; i < sizeof(binaryData.sendData); i++) {
            _stream = _myFile.read();
            binaryData.sendData[i] = _stream;
          }
          //add the packet count to the structure
          binaryData.packetCount = _count;
          //lets send the data to the receiver
          memcpy(binaryDataBuf, &binaryData, sizeof(binaryData));
          myDriver.send((uint8_t*)&binaryDataBuf, sizeof(binaryData));
          //sendDataRF((uint8_t*)&binaryDataBuf); **
          //myDriver.send((uint8_t *)binaryData.sendData, sizeof(binaryData.sendData));
          //myDriver.waitPacketSent();



          // SerialUSB.print("send data array ");
          // SerialUSB.print(count);
          // SerialUSB.print(" : ");
          // for (int i = 0; i < sizeof(binaryData.sendData); i++) {
          //   SerialUSB.print(binaryData.sendData[i]);
          //   if (i < 199) {
          //     SerialUSB.print(" , ");
          //   }
          // }
          //SerialUSB.println(" ");

        } else if (_count > binaryData.iterationCount) {
          // we will run the last for loop for the last array element
          memset(binaryData.sendData, 0, sizeof(binaryData.sendData));
          for (int i = 0; i < binaryData.leftOver; i++) {
            _stream = _myFile.read();
            binaryData.sendData[i] = _stream;
          }
          //add the packet count to the structure
          binaryData.packetCount = _count;

          memcpy(binaryDataBuf, &binaryData, sizeof(binaryData));
          //delay(10);
          myDriver.send((uint8_t*)&binaryDataBuf, sizeof(binaryData));
          //sendDataRF((uint8_t*)&binaryDataBuf); **
          //myDriver.send((uint8_t *)binaryData.sendData, sizeof(binaryData.sendData));
          //myDriver.waitPacketSent();

          // SerialUSB.print("send data array ");
          // SerialUSB.print(count);
          // SerialUSB.print(" : ");
          // for (int i = 0; i < binaryData.leftOver; i++) {
          //   SerialUSB.print(binaryData.sendData[i]);
          //   if (i < (binaryData.leftOver - 1)) {
          //     SerialUSB.print(" , ");
          //   }
          // }
          //SerialUSB.println(" ");
        }
        //SerialUSB.print("sending data chunk ");
        //SerialUSB.println(count);
        _count++;

        // Used for updating the progress bar on the GUI
        if (_count % 5 == 0) {
          sprintf(buffer, "pb%d", _count * 200);
          SerialUSB.println(buffer);
        }

        myDriver.waitPacketSent();
        //waitToSendDataRF(); **
        //delay();
      }
      _myFile.close();

      //Reset the packet count and counter
      binaryData.packetCount = 0;
      _count = 1;


      // RESET the variable for the sending of the file
      fileData.transferBegin = false;
      // Tell the remote system that we are done sending the file
      fileData.transferComplete = true;

      memcpy(fileDataBuf, &fileData, sizeof(fileData));
      myDriver.send((uint8_t*)&fileDataBuf, sizeof(fileData));
      myDriver.waitPacketSent();
      //sendDataRF((uint8_t*)&fileDataBuf); **
      //waitToSendDataRF(); **

      //SerialUSB.println("data sent...");
      SerialUSB.println("remote file transfer complete");  // return to 'main' command block
      // Need to reset the transferComplete flag as it will print the 'total Amout of Received Bytes' on the receive side if I don't
      fileData.transferComplete = false;
    }
  }
}

// BufferSize is the amount of data that we are execpting to pad the incoming array
void fileTransfer::_writeFile() {

  // This is a timeout function for sending the file locally, if after 10's the file isn't transfered then we can assume
  // that we want to back out or we have lost connection
  if ((millis() - _previousTime >= _delay) && (__fileInfo == false)) {
    _commandMain = 0;
    SerialUSB.println("main");
  }

  while (SerialUSB.available() > 0) {

    _value = SerialUSB.read();

    //changed from 2 --> 34 because we added 32 bytes  35byte array
    if ((_b <= 34) && (!__fileInfo)) {
      _arrayHash[_b] = _value;

      if (_b == 34) {
        //Calculate the file size from incoming data
        _fileSize = int(_arrayHash[1] << 8) | _arrayHash[2];
        __fileInfo = true;
        //SerialUSB.println(String(_fileSize));
      }
      // else if (_b == 8) {
      //   for (int i = 0; i < sizeof(state)-1; i++){
      //     if (state[i] == char(_arrayHash[i])){
      //       SerialUSB.println("file transfer block");
      //     }
      //   }
      // }
      _b++;

      //SerialUSB.println("pb"+String(_b));

      //SerialUSB.println("within b if statement");
      // myFile.write(value);
    } else {
      _myFile.write(_value);
      //SerialUSB.println("else statement");
    }
    //changed from 3 --> 35 because we again added 32 bytes
    if ((_byteCount - 35) == _fileSize) {
      _dataFlag = true;
      sprintf(buffer, "pb%d", _byteCount - 35);
      SerialUSB.println(buffer);
    }
    _byteCount++;
    // **this is able to give me progress but i dont need to see every byte as it takes twice as long**
    if (_byteCount % 2000 == 0) {
      sprintf(buffer, "pb%d", _byteCount);
      SerialUSB.println(buffer);
    }
  }


  if (_dataFlag) {
    if (_myFile) {
      _myFile.close();
    }
    _dataFlag = false;
    _fileTransferComplete = true;
    _commandMain = 0;
    SerialUSB.println("main");
  }
}
// move us back to the main
bool fileTransfer::_transferComplete() {

  return _fileTransferComplete;
}
// Old way 7-10-2025
// String fileTransfer::getIncomingFileHash() {
//   String incomingHash = "";
//   String _arrayHashLength = "";

//   // Start at index 3 because the first 3 bytes are not apart of the hash
//   // index 0 = 22, index 1 & index 2 = file size
//   for (int i = 3; i < sizeof(_arrayHash); i++) {
//     _arrayHashLength = String(_arrayHash[i], HEX);
//     if (_arrayHashLength.length() == 1) {
//       String modified = "0";
//       modified += _arrayHashLength;
//       incomingHash += modified;
//     } else {
//       incomingHash += _arrayHashLength;
//     }
//     //incomingHash += String(_arrayHash[i], HEX);
//   }
//   return incomingHash;
// }


// Hashes the file received, stores in char array 7-10-2025
// Re-written to where it can be used on the receive side also
char* fileTransfer::_getIncomingFileHash(const char deviceLocation[]) {
  memset(incomingHash, 0, sizeof(incomingHash));
  char tempBuffer[3] = { 0 };

  if (strcmp(deviceLocation, "sender") == 0) {
    for (int i = 3; i < sizeof(_arrayHash); i++) {
      sprintf(tempBuffer, "%02x", _arrayHash[i]);
      //SerialUSB.println(incomingHash);
      strcat(incomingHash, tempBuffer);
      if (i == sizeof(_arrayHash) - 1) {
        //SerialUSB.println("terminating char string");
        incomingHash[64] = '\0';
        break;
      }
    }
  } else if (strcmp(deviceLocation, "receiver") == 0) {
    for (int i = 3; i < sizeof(fileData.hash); i++) {
      sprintf(tempBuffer, "%02x", fileData.hash[i]);
      //SerialUSB.println(incomingHash);
      strcat(incomingHash, tempBuffer);
      if (i == sizeof(fileData.hash) - 1) {
        //SerialUSB.println("terminating char string");
        incomingHash[64] = '\0';
        break;
      }
    }
  }
  return incomingHash;
}

// Was going to try and combine the incoming/stream hash methods
// char* fileTransfer::getHash(const char stream[]) {


//   if (strcmp(stream, "incoming") == 0) {
//     char* hashPTR = &incomingHash;
//     char tempBuffer[3] = { 0 };

//     for (int i = 3; i < sizeof(_arrayHash); i++) {
//       sprintf(tempBuffer, "%02x", _arrayHash[i]);
//       //SerialUSB.println(incomingHash);
//       strcat(incomingHash, tempBuffer);
//       if (i == sizeof(_arrayHash) - 1) {
//         SerialUSB.println("terminating char string");
//         incomingHash[64] = '\0';
//         break;
//       }
//     }
//   } else if (strcmp(stream, "stream")) {

//   }

//   return hashPTR;
// }



void fileTransfer::readFile() {
  _myFile = SD.open(_file);

  if (_myFile) {
    SerialUSB.println("inside read file function");
    while (_myFile.available()) {

      uint8_t stream = _myFile.read();
      SerialUSB.println(stream, BIN);
    }
    _myFile.close();
  }
}


// Hashes the file and then returns the hash
void fileTransfer::_hashFileStream() {
  unsigned int _count = 0;
  uint8_t _bitShiftCount = 0;
  uint32_t _hashBank = 0;
  unsigned int _hashBankCount = 0;
  uint32_t _hashArray[9] = { 0, 0xB9D1FBFD, 0xFF206483, 0xC829A0B9, 0x832459EC, 0xEFE83F42, 0xEFB7A415, 0xDF4BD379, 0xE21B5202 };
  uint8_t convertedHashArray[32];

  if (_myFile) {
    //_hashValue = "";  //reset the hash
    memset(_hashValue, 0, sizeof(_hashValue));  //Reset the hashValue so we don't keep concatenating to it...
    while (_myFile.available()) {

      for (int i = 0; i < 4; i++) {



        uint8_t stream = _myFile.read();
        //fileArray[count] = stream;
        switch (_bitShiftCount) {
          case 0:
            _hashBank = stream << 24; /*SerialUSB.print("The hashbank is: "); SerialUSB.println(hashBank, BIN);*/
            break;
          case 1:
            _hashBank |= stream << 16; /* SerialUSB.print("The hashbank is: ");  SerialUSB.println(hashBank, BIN);*/
            break;
          case 2:
            _hashBank |= stream << 8; /* SerialUSB.print("The hashbank is: "); SerialUSB.println(hashBank, BIN);*/
            break;
          case 3:
            _hashBank |= stream; /*SerialUSB.print("The hashbank is: "); SerialUSB.println(hashBank, BIN);*/
            break;
        }
        // SerialUSB.print("count: ");
        // SerialUSB.print(_count);
        // SerialUSB.print("\t");
        // SerialUSB.println(stream);
        _count++;
        _bitShiftCount++;


        if ((_count % 4) == 0) {
          // Increment the hashBankCount
          _hashBankCount++;

          //Reset the bitShiftCount
          _bitShiftCount = 0;
          // this is where we take 4 byte chunks
          //SerialUSB.print("hashBankCount is: ");
          //SerialUSB.println(hashBankCount);


          //this will be used to control which 32 bit integer gets the current data
          switch (_hashBankCount) {
            case 1:
              _hashArray[1] = (_hashBank != 0) ? _hashBank ^= _hashArray[1] : _hashBank = 0xC5A14356;
              _hashBank = 0;
              //SerialUSB.println(_hashArray[1], BIN);
              break;  //
            case 2:
              _hashArray[2] = (_hashBank != 0) ? _hashBank ^= ~_hashArray[2] : _hashBank = 0x84014006;
              _hashBank = 0;
              //SerialUSB.println(_hashArray[2], BIN);
              break;
            case 3:
              _hashArray[3] = (_hashBank != 0) ? _hashBank ^= _hashArray[3] : _hashBank = 0x634DE5A3;
              _hashBank = 0;
              //SerialUSB.println(_hashArray[3], BIN);
              break;  //
            case 4:
              _hashArray[4] = (_hashBank != 0) ? _hashBank |= ~_hashArray[4] : _hashBank = 0x92F80685;
              _hashBank = 0;
              //SerialUSB.println(_hashArray[4], BIN);
              break;
            case 5:
              _hashArray[5] = (_hashBank != 0) ? _hashBank ^= ~_hashArray[5] : _hashBank = 0x1FEE62AD;
              _hashBank = 0;
              //SerialUSB.println(_hashArray[5], BIN);
              break;
            case 6:
              _hashArray[6] = (_hashBank != 0) ? _hashBank |= ~_hashArray[6] : _hashBank = 0xDA4AC801;
              _hashBank = 0;
              //SerialUSB.println(_hashArray[6], BIN);
              break;
            case 7:
              _hashArray[7] = (_hashBank != 0) ? _hashBank ^= _hashArray[7] : _hashBank = 0x6D800AE8;
              _hashBank = 0;
              //SerialUSB.println(_hashArray[7], BIN);
              break;  //
            case 8:
              _hashArray[8] = (_hashBank != 0) ? _hashBank |= ~_hashArray[8] : _hashBank = 0xD46DC441;
              _hashBank = 0;
              //SerialUSB.println(_hashArray[8], BIN);
              break;
          }
          // Reset the hashBankCount back to zero
          if (_hashBankCount == 8) {
            _hashBankCount = 0;
          }
        }
      }
    }
    _myFile.close();
  } else {
    SerialUSB.println("error opening file!");
  }
  //lets print the hashArray
  uint8_t start = 0;

  //used to convert from uint32_t array to a uint8_t array
  for (int i = 1; i < 9; i++) {
    for (int j = 1; j <= 4; j++) {
      switch (j) {																// [FIRST ITERATION] 
        case 1:
          convertedHashArray[start] = _hashArray[i] >> 24 & 0xFF;				// CASE = 1 , START = 0 , i = 1
          start++;
          break;
        case 2:
          convertedHashArray[start] = _hashArray[i] >> 16 & 0xFF;											   // CASE = 1 , START = 1 , i = 1 
          start++;
          break;
        case 3:
          convertedHashArray[start] = _hashArray[i] >> 8 & 0xFF;																			  // CASE = 1 , START = 2 , i = 1
          start++;
          break;
        case 4:
          convertedHashArray[start] = _hashArray[i] & 0xFF;																													 // CASE = 1 , START = 3 , i = 1
          start++;
          break;
      }
    }
  }

  //-------------------//
  // MOVED TO FUNCTION //   ---> convertHashArray2String(* array pointer, sizeof(array))
  //-------------------//
  //*** NEW HASH ARRAY TO STRING CONVERSION ***
  //converted hash array to String with 2 character representation
  // String arrayHashLength = "";
  // for (int i = 0; i < sizeof(convertedHashArray); i++) {
  //   arrayHashLength = String(convertedHashArray[i], HEX);

  //   if (arrayHashLength.length() == 1) {
  //     String modified = "0";
  //     modified += arrayHashLength;
  //     _hashValue += modified;
  //   } else {
  //     _hashValue += arrayHashLength;
  //   }
  // }

  // Have to send the array size as an argument
  //_convertHashArray2String(convertedHashArray, sizeof(convertedHashArray));
  _getFileStreamHash(convertedHashArray);

  //This is to make sure that the hashValue is the same as the array print out
  //SerialUSB.println("\n" + hashValue);


  //lets print the size of the hash also
  //SerialUSB.print("hash size: ");
  // SerialUSB.println(hashSize);

  // SerialUSB.print("this is the hash length: ");
  // SerialUSB.println(stringLength);

  //No longer did a String return and did a seperate function to handle the hash return
  //return _hashValue;
}

//**Old way**
// This will take in the array and convert it to a String
// void fileTransfer::_convertHashArray2String(uint8_t* arrayToConvert, uint8_t size) {
//   String arrayHashLength = "";

//   for (int i = 0; i < size; i++) {
//     arrayHashLength = String(arrayToConvert[i], HEX);

//     if (arrayHashLength.length() == 1) {
//       String modified = "0";
//       modified += arrayHashLength;
//       _hashValue += modified;
//     } else {
//       _hashValue += arrayHashLength;
//     }
//   }
// }

// 7-10-2025
//I know the array is only going to be 32 bytes so theres no need to pass a second parameter
void fileTransfer::_getFileStreamHash(uint8_t* arrayToConvert) {

  char tempBuffer[3] = { 0 };

  for (int i = 0; i < 32; i++) {
    sprintf(tempBuffer, "%02x", arrayToConvert[i]);
    strcat(_hashValue, tempBuffer);
    if (i == sizeof(arrayToConvert) - 1) {
      //SerialUSB.println("terminating char string");
      _hashValue[64] = '\0';
    }
  }
}


// ADDED 6-29-2025
//------------------------//
// RESEND DROPPED PACKETS //
//------------------------//
void fileTransfer::_resendDroppedPackets(uint8_t* droppedPackets, uint8_t index) {
  //-----------//
  // OPEN FILE //
  //-----------//
  //-----------------//  Example: 49 packet dropped
  //ref. file - exel //  (49 -1) * 200 = 9600 bytes from file start
  //-----------------//  packet 49: start 9600, stop 9799  [200] bytes
  // wipe the memory location just for testing purposes
  uint8_t stream;
  uint8_t packetCount = 1;
  uint32_t byteCount = 0;
  uint8_t internalIndex = 0;

  _myFile = SD.open("file1.bin");


  if (_myFile) {
    while (_myFile.available()) {

      if (packetCount <= (_myFile.size() / 200)) {

        for (int i = 0; i < 200; i++) {
          byteCount++;
          stream = _myFile.read();
          binaryData.sendData[i] = stream;  //may need to use the dropped packet array....?
        }
      }
      if (packetCount > (_myFile.size() / 200)) {
        for (int i = 0; i < (_myFile.size() % 200); i++) {
          byteCount++;
          stream = _myFile.read();
          binaryData.sendData[i] = stream;  //may need to use the dropped packet array....?
        }
        // THIS WILL TRIP IF WE DROP A PACKET AND THE COUNT IS RIGHT
      }
      if ((packetCount == droppedPackets[internalIndex]) && (internalIndex != index)) {
        //Lets copy the data to the outbound buffer and then send
        SerialUSB.print("Sending packet: ");
        SerialUSB.print(packetCount);
        SerialUSB.print("\t Byte count: ");
        SerialUSB.println(byteCount - 200);  // This is because once we check the status we are on the backside of the for loop (200)
        //----------------------//
        //     SEND PACKETS     //
        //     2 FLAGS SET      //
        //     PACKET FLAG      //
        //   SET PACKET COUNT   //
        //----------------------//
        binaryData.droppedPacketFlag = true;
        binaryData.droppedPacket = packetCount;
        memcpy(binaryDataBuf, &binaryData, sizeof(binaryData));

        myDriver.send((uint8_t*)&binaryDataBuf, sizeof(binaryData));
        //sendDataRF((uint8_t*)&binaryDataBuf); **
        //------------------------------//    Used to do the intial check if the dropped packet is correctly indexed from the file for
        // PRINT OUT THE DROPPED PACKET //    the resend.
        //------------------------------//
        // for (int i = 0; i < 200; i++) {
        //   SerialUSB.print(binaryData.sendData[i], HEX);
        //   SerialUSB.print(",");
        // }
        SerialUSB.println();
        // Increment the internalIndex count
        internalIndex += 1;
      }
      packetCount++;
    }
    _myFile.close();

    // Let remote know when the last dropped packet has been sent
    fileData.remoteStatus = 0x45;

    memcpy(fileDataBuf, &fileData, sizeof(fileData));

    myDriver.send((uint8_t*)&fileDataBuf, sizeof(fileData));
  }
  //---------------//
  // INITIAL DEBUG //
  //---------------//
  // for (int i = 0; i < index; i++) {
  //   SerialUSB.print("Packet ");
  //   SerialUSB.print(i);
  //   SerialUSB.print(": ");
  //   SerialUSB.println(droppedPackets[i]);
  // }
}

void fileTransfer::packetDataAvailable(const char deviceType[]) {

  if (myDriver.available()) {
    byte buf[myDriver.maxMessageLength()];
    byte len = sizeof(buf);

    if (myDriver.waitAvailableTimeout(200)) {
      while (myDriver.recv(buf, &len)) {

        //copy the incoming data to the incomingBuffer
        memcpy(incomingBuffer, buf, sizeof(buf));

        //--------//
        // SENDER //
        //--------//
        if (incomingBuffer[0] == DROPPED_PACKET) {
          memcpy(&dropped, incomingBuffer, sizeof(dropped));
          //-------------------------------------------------------//
          // [RECEIVING MESSAGES] DROPPED PACKETS/ FILE SEND CONFIR//
          //-------------------------------------------------------//
          if (strcmp(deviceType, "sender") == 0) {
            // Then lets print out the array of the

            // for (int i = 0; i < dropped.packetIndex; i++) {
            //   SerialUSB.print("Packet ");
            //   SerialUSB.print(i);
            //   SerialUSB.print(": ");
            //   SerialUSB.println(dropped.resendPackets[i]);
            // }

            // FUNCTION TO HANDLE THE RESENDING OF THE DROPPED PACKETS
            // PASSING POINTER AS AN ARGUMENT
            _resendDroppedPackets(dropped.resendPackets, dropped.packetIndex);

            // **DEBUG**
            // Reset the droppedPacketFlag otherwise on the receive side it messes with how the data is read/wrote into the file
            binaryData.droppedPacketFlag = false;

            //-----------------//  Example: 49 packet dropped
            //ref. file - exel //  (49 -1) * 200 = 9600 bytes from file start
            //-----------------//  packet 49: start 9600, stop 9799  [200] bytes
            // wipe the memory location just for testing purposes
            memset(dropped.resendPackets, 0, sizeof(dropped.resendPackets));
          }
          //------------------------//
          // SENDER / RECEIVER SIDE //
          //------------------------//
        } else if (incomingBuffer[0] == FILE_DATA) {

          memcpy(&fileData, incomingBuffer, sizeof(fileData));

          //-------------------------------//
          //  REMOTE STATUS CHECK @ SENDER //
          //-------------------------------//
          if (strcmp(deviceType, "sender") == 0) {
            if (fileData.remoteStatus == 0x30) {
              fileData.transferBegin = true;
              fileData.remoteStatus = 0x00;  // RESET THE REMOTE STATUS

              //need to open the file back up if we plan to test sending files back to back
              _myFile = SD.open("file1.bin");
              // AFTER THE FILE IS OPENED LETS GO AHEAD AND GET THE ITERAITON AND LEFTOVER COUNT

              //----------------------------------------------//
              // RESET THE DROPPED PACKET ARRAY/ INDEX COUNT  //
              //----------------------------------------------//
              memset(dropped.resendPackets, 0, sizeof(dropped.resendPackets));
              dropped.packetIndex = 0;

              SerialUSB.println("**RECEIVED CONFIRMATION FROM REMOTE**");
            }
          }
          //--------------------------------//
          //  REMOTE CODE BLOCK @ RECEIVER  //
          //--------------------------------//
          else if (strcmp(deviceType, "receiver") == 0) {
            // reset the packetCounter back to zero
            // because this is the intial status check to send the firmware
            _packetCounter = 0;
            // DEBUG
            //SerialUSB.println(fileData.remoteStatus);

            //---------------------------------//
            // RECEIVE STATUS CHECK @ RECEIVER //
            //---------------------------------//
            if (fileData.remoteStatus == 0x25) {

              SerialUSB.println("**RECEIVE FILE TRANSFER REQUEST**");

              fileData.remoteStatus = 0x30;  // 0X30 = GOOD
              //-------------------//
              // SETUP THE SD CARD //
              //-------------------//
              SerialUSB.println("Initializing SD card...");
              if (!SD.begin(_SDpin)) {
                //   SerialUSB.println("initialization failed!");
                //   Can add extra redundancy to handle if we accidentily remove the SD card before we transmit
                //   while (1);
              }
              if (SD.exists(_file)) {
                SerialUSB.println("File already exists, deleting file");
                SD.remove(_file);
              }
              //myFile = SD.open(fileName, FILE_WRITE);
              _initalizeHashFile("write");


              SerialUSB.println("SD card initialization done.");

              memcpy(fileDataBuf, &fileData, sizeof(fileData));
              myDriver.send((uint8_t*)&fileDataBuf, sizeof(fileData));
              myDriver.waitPacketSent();
              // THis is where the file integrity will be checked with the hash
            } else if (fileData.remoteStatus == 0x45) {
              if (dropped.resendPackets[0] == 0) {
                SerialUSB.println("All dropped packets have been received!");

                // Now since the file should be rebuilt we can then hash it
                if (_checkFileIntegrity("receiver")) {
                  SerialUSB.println("File Integrity Check: PASS");
                } else {
                  SerialUSB.println("file hashing error");
                }
              } else {
                SerialUSB.println("We had an issue receiving all of the dropped packets");

                // This is where we will send another message to send dropped packets
              }
              //-------------------------------------------//
              // FILE TRANSFER COMPLETE / RESET VARIABLES  //
              //-------------------------------------------//
            } else if (fileData.transferComplete == true) {
              // Lets close the file
              _myFile.close();


              SerialUSB.print("Bytes written to file: ");
              SerialUSB.println(_fileByteCount - 1);
              SerialUSB.print("Total Bytes recieved: ");
              SerialUSB.println((binaryData.packetCount - dropped.packetIndex) * 200);

              // Check the first array item to see if there are any dropped packets
              if (dropped.resendPackets[0] != 0) {
                SerialUSB.print("packets dropped ");
                //goes off of index so if we have dropped 2 packets that index[0]
                //and index[1]  but it is then incremented to  index[2] for the
                //next interation so it can also be used as a counter
                SerialUSB.print(dropped.packetIndex);
                SerialUSB.print(" : ");
                //print out the packets that were dropped
                for (int i = 0; i < dropped.packetIndex; i++) {
                  SerialUSB.print(dropped.resendPackets[i]);
                  if (i < dropped.packetIndex - 1) {
                    SerialUSB.print(" , ");
                  }
                }
                //--------------------------------//
                //  SEND DROPPED PACKETS MESSAGE  //
                //--------------------------------//
                //lets send the message to the receiver
                memcpy(droppedPacketBuf, &dropped, sizeof(dropped));

                myDriver.send((uint8_t*)&droppedPacketBuf, sizeof(droppedPacketBuf));
                myDriver.waitPacketSent();

                // wipe the memory location for now because we are testing it..
                // Don't wipe the memory locations.... we will wipe the memory was we receive the packets in
                //memset(dropped.resendPackets, 0, sizeof(dropped.resendPackets));
                SerialUSB.println(" ");
                //reset the index of the resendPackets
                dropped.packetIndex = 0;
                // We have received all of the packets on the first go so we need to check the hash's to make sure that they match
              }
              else {
                if (_checkFileIntegrity("receiver")) {
                  SerialUSB.println("File Integrity Check: PASS");
                } else {
                  SerialUSB.println("file hashing error");
                }
              }
              fileData.transferComplete = false;
              _fileByteCount = 1;
              // Reset the internal counter
              _packetCounter = 0;
            }
          }
          //----------//
          // RECEIVER //
          //----------//
        } else if (incomingBuffer[0] == BINARY_DATA) {
          // only on the receive side so we dont need to nest the deviceType command
          _packetCounter++;

          memcpy(&binaryData, incomingBuffer, sizeof(binaryData));

          // If we are recieving the regular file and not the dropped packets yet
          if (binaryData.droppedPacketFlag != true) {
            //check to see if the incoming packet and the expected packet are the same number
            if (_packetCounter != binaryData.packetCount) {
              _offsetWriteFlag = true;
              //now we need to see how many packets they are off

              //if they are only one packet off
              if ((binaryData.packetCount - 1) == _packetCounter) {
                SerialUSB.print("Have an issue with packets # \t");
                SerialUSB.print("Packet counter internal: ");
                SerialUSB.print(_packetCounter);
                SerialUSB.print("\t Packet counter external: ");
                SerialUSB.println(binaryData.packetCount);

                //Save the packets that are dropped to the array so we can request them later
                dropped.resendPackets[dropped.packetIndex] = _packetCounter;
                //reset the internal packet count to match the external count coming in
                _packetCounter = binaryData.packetCount;
                dropped.packetIndex++;
                _writeOffsetSize++;
                if (dropped.packetIndex > 198) {
                  dropped.packetIndex == 198;
                  SerialUSB.println("Dropped Packet Buffer Index maxed out @ 198");
                }
                //meaning we have more than 1 packet of seperation
              } else {
                uint16_t indexCount, externalCount, internalCount = 0;
                indexCount = dropped.packetIndex;
                externalCount = binaryData.packetCount;
                internalCount = _packetCounter;
                char buffer[100];

                for (int i = indexCount; i < (indexCount + (externalCount - internalCount)); i++) {
                  dropped.resendPackets[i] = _packetCounter;
                  sprintf(buffer, "Have a problem, internal packet count: %d, external packet count: %d", _packetCounter, externalCount);
                  SerialUSB.println(buffer);

                  //save 'i' to the packetIndex variable
                  dropped.packetIndex++;
                  _packetCounter++;
                  _writeOffsetSize++;
                }
                // binaryData.packetCount and packetCounter (internal) should be equal....
                //packetCounter = binaryData.packetCount;
              }
            }

            //-------------------//
            // WRITE TO SD CARD  //
            //                   //
            // WRITE ZERO BLOCKS //
            //-------------------//
            if (_offsetWriteFlag) {
              for (int i = 0; i < _writeOffsetSize; i++) {
                for (int i = 0; i < 200; i++) {
                  // Check the file size against the count to see if we have reached it so we can stop writing to the file
                  if (_fileByteCount <= uint32_t(fileData.hash[1] << 8 | fileData.hash[2])) {
                    _myFile.write((uint8_t)0);
                    _fileByteCount++;
                  } else {
                    // lets break out
                    break;
                  }
                }
              }
              // Reset the flag along with the writeOffsetSize
              _offsetWriteFlag = false;
              _writeOffsetSize = 0;
            }


            for (int i = 0; i < sizeof(binaryData.sendData); i++) {
              // Check the file size against the count to see if we have reached it so we can stop writing to the file
              if (_fileByteCount <= uint32_t(fileData.hash[1] << 8 | fileData.hash[2])) {
                _myFile.write(binaryData.sendData[i]);
                _fileByteCount++;
              } else {
                // lets break out
                break;
              }
              // SerialUSB.print(_fileByteCount);
              // SerialUSB.print(" : ");
              // SerialUSB.println(binaryData.sendData[i]);
              //_fileByteCount++;
            }
          }
          // We are receiving the droppped packets now
          else {

            if (binaryData.droppedPacket == dropped.resendPackets[0]) {
              char buffer[100];
              sprintf(buffer, "Receiving packet %d from transmitter...", binaryData.droppedPacket);
              SerialUSB.println(buffer);

              // Reopen the binary file so we can modify it
              _myFile = SD.open(_file, O_RDWR);

              // need to read in the iteration count and the leftover count to check the dropped packet
              // if droppedpacket < iteration write 200
              // else write leftover size
              uint8_t writeBlockSize = 0;

              // Total packet _fileByteCount = iterationCount + 1 if (leftOver > 0)
              uint8_t totalPacketCount = binaryData.iterationCount;
              totalPacketCount += (binaryData.leftOver > 0) ? 1 : 0;

              // Now that we have the total packet count we can make sure that we don't overshoot the file size
              // Now lets first compare to see if the dropped packet is equal to the overall file size packet count
              // First I need to see if the interationCount and the totalPacketCount are the same


              // Handle the instance that the droppedPacket is equal to the last total packet for the file but there is left over
              // So we only want to write the size of the leftOver segment, no more
              if ((binaryData.droppedPacket == totalPacketCount) && (binaryData.leftOver != 0)) {
                writeBlockSize = binaryData.leftOver;
              } else {
                writeBlockSize = 200;
              }

              // Now we should be able to write to the file
              // First need to move to the location and then write the data...
              //*** DEBUG ***
              //SerialUSB.print("Write block size");
              //SerialUSB.println(writeBlockSize);

              // Lets first print out the dropped packets to make sure we have them correctly
              //SerialUSB.println((binaryData.droppedPacket -1)*200);   // This will be what I use to seek out the correct index within the binary file
              //---------------//
              // PRINT PACKETS //
              //---------------//
              // for (int i = 0; i < sizeof(binaryData.sendData); i++) {
              //   // myfile.seek((packet -1) * 200)
              //   SerialUSB.print(binaryData.sendData[i], HEX);
              //   SerialUSB.print(",");
              // }
              // FIRST NEED TO SET THE POINT IN THE FILE
              _myFile.seek((binaryData.droppedPacket - 1) * 200);

              for (int i = 0; i < writeBlockSize; i++) {
                _myFile.write(binaryData.sendData[i]);
              }
              //SerialUSB.println("");
              // RESET the packet count along with the droppedPacketFlag
              binaryData.droppedPacketFlag = false;
              binaryData.droppedPacket = 0;
              _myFile.close();

              //--------------------------------//
              // SHIFT THE DROPPED PACKET ARRAY //
              //--------------------------------//
              for (int i = 0; i < sizeof(dropped.resendPackets) - 1; i++) {
                dropped.resendPackets[i] = dropped.resendPackets[i + 1];
              }
              // Zero the last index
              dropped.resendPackets[sizeof(dropped.resendPackets) - 1] = 0;
            }
          }
        }
      }
    }
  }
}

bool fileTransfer::_checkFileIntegrity(const char deviceLocation[]) {
  // open the file first
  _myFile = SD.open(_file);

  // First lets has the file stream, this will return _hashValue
  _hashFileStream();
  bool result = false;

  // Secondly need to convert the incoming file hash that is sent within the fileData structure
  char* receivedHash = _getIncomingFileHash(deviceLocation);

  // After we have the two we can compare
  if (strcmp(receivedHash, _hashValue) == 0) {
    result = true;
  }
  _myFile.close();

  return result;
}
