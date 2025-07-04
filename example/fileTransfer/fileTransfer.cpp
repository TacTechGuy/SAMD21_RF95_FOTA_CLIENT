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
use the function hashFileStream() to extract the hash from the newly created file.   

STEPS:
1. make an object for the firmware class
2. initalize SD File [file name, read or write]   #file name CANNOT exceed 8 characters
3. reinitalize the SD file for the opposite [file name, read or write] opposite of before
4. hash the file and or get the incoming hash

STEPS:
1. firmware firmwareUpdate
2. firmwareUpdate.initalizeHashFile("myFile.bin", "write")
3. firmwareUpdate.initalizeHashFile("myFile.bin", "read")
4. [Internal] firmwareUpdate.hashFileStream()  [External] firmwareUpdate.getIncomingFileHash()


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



 firmware::firmware(uint8_t csPin, uint8_t interruptPin):rf95(csPin,interruptPin),myDriver(rf95, myCipher){

 }


//Single function to connect and either open the file or make the file READ/WRITE
//convert from String to char
bool firmware::initalizeHashFile(const char file[], const char readWrite[]) {
  resetFileWriteVariables();

  _file = file;

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
  if (!SD.begin(4)) {
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

void firmware::resetFileWriteVariables() {
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
void firmware::receiveCommandLocal() {
  //char incomingCommand[5] = "";


  if (_commandMain == 0) {
    while (SerialUSB.available() > 0) {

      char c = SerialUSB.read();
      _command += c;

      // TEST - Change from string to C-string
      // create a temporary char array
      // char temp[2];
      // temp[0] = c;
      // temp[1] = '\0';
      _inValue = (uint8_t)c;
      //SerialUSB.println(_inValue);
      // strcat(incomingCommand, temp);
    }
    if (_command.length() > 0) {
      SerialUSB.println("main");
      //SerialUSB.println(command);
      //String commandToInt = _command;  ** Old way of doing it with char that is concatenated to a string and then converted to an int
      //_inValue = commandToInt.toInt();

      //TEST
      // if (strcmp(incomingCommand,"file transfer") == 0) {

      // }

      //no line ending
      if (_command == "file transfer") {
        _pushBinaryRemote = false;
        //SerialUSB.println("this is working correctly");

        //let initalize the file first... "file1.bin"
        initalizeHashFile("file1.bin", "write");

        _commandMain = 1;
        // transferFlag = true;
        SerialUSB.println("file transfer block");
        _previousTime = millis();
      }
      //---------------------//
      // SENDING FILE REMOTE //
      //---------------------//
      if ((_command == "upload remote") && (_pushBinaryRemote == true)) {
        SerialUSB.println("remote transfer block");  // used for the visual basic code so we can't remove
        fileData.remoteStatus = 0x25;
        SerialUSB.println("inside of the upload command");

        // Check remote status
        if (fileData.remoteStatus == 0x25) {
          fileData.size = sizeof(fileData);

          memcpy(fileDataBuf, &fileData, sizeof(fileData));
          myDriver.send((uint8_t*)&fileDataBuf, sizeof(fileData));
          //sendDataRF((uint8_t*)&fileDataBuf); **
          myDriver.waitPacketSent();
          //waitToSendDataRF(); **
        }
      }
      if (_command == "get state") {
        SerialUSB.println("main");
      }
      _command = "";
    }
  } else if (_commandMain == 1) {
    if (SerialUSB) {
      SerialUSB.println("file transfer block");
    }
    writeFile();
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

  if (transferComplete()) {
    _fileTransferComplete = false;
    _pushBinaryRemote = true;

    SerialUSB.println("file transfer complete");

    //Read from the file we just wrote to in order to get the hash
    initalizeHashFile("file1.bin", "read");
    hashFileStream();

    //lets print the incoming file hash
    SerialUSB.println(getIncomingFileHash());

    SerialUSB.println(getFileHash());

    if (getIncomingFileHash() == getFileHash()) {
      SerialUSB.println("match");
      //SerialUSB.print("ptr for _arrayHash: ");
      // uint8_t fileData.hash[35] = {0};

      memcpy(fileData.hash, getFileHashPtr(), sizeof(fileData.hash));
      // *** Printing out the address takes up to much time and messes up the array printing ***
      // SerialUSB.println((unsigned long)firmwareUpdate.getFileHashPtr());
      // OPEN THE FILE
      _myFile = SD.open(_file);

      binaryData.iterationCount = _myFile.size() / sizeof(binaryData.sendData);
      binaryData.leftOver = _myFile.size() % sizeof(binaryData.sendData);

      SerialUSB.println(binaryData.iterationCount);
      SerialUSB.println(binaryData.leftOver);

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

        myDriver.waitPacketSent();
        //waitToSendDataRF(); **
        //delay();
      }
      _myFile.close();
      fileData.size = sizeof(fileData);

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

      SerialUSB.println("data sent...");
      // Need to reset the transferComplete flag as it will print the 'total Amout of Received Bytes' on the receive side if I don't
      fileData.transferComplete = false;
    }
  }
}

// **NOT IN USE**
// void firmware::_prepareForFileUpload() {
//   // first thing i need to do is append the hashdata to an array

//   struct {
//     bool firstSend = true;
//     char hashArray[64] = "";
//     uint8_t fileArray[20000];
//   } fileHashData;

//   // fileHashData dataToSend;
//   _hashValue.toCharArray(fileHashData.hashArray, _hashValue.length() + 1);
//   //This prints correctly so we can comment out
//   //SerialUSB.println(fileHashData.hashArray);
// }


// BufferSize is the amount of data that we are execpting to pad the incoming array
void firmware::writeFile() {
  char buffer[8];

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
      digitalWrite(13, HIGH);
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
bool firmware::transferComplete() {

  return _fileTransferComplete;
}

String firmware::getIncomingFileHash() {
  String incomingHash = "";
  String _arrayHashLength = "";

  for (int i = 3; i < sizeof(_arrayHash); i++) {
    _arrayHashLength = String(_arrayHash[i], HEX);
    if (_arrayHashLength.length() == 1) {
      String modified = "0";
      modified += _arrayHashLength;
      incomingHash += modified;
    } else {
      incomingHash += _arrayHashLength;
    }
    //incomingHash += String(_arrayHash[i], HEX);
  }
  return incomingHash;
}

void firmware::readFile() {
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

String firmware::getFileHash() {
  return _hashValue;
}

//
uint8_t* firmware::getFileHashPtr() {
  uint8_t* Ptr = _arrayHash;
  return Ptr;
}


// Hashes the file and then returns the hash
void firmware::hashFileStream() {
  unsigned int _count = 0;
  uint8_t _bitShiftCount = 0;
  uint32_t _hashBank = 0;
  unsigned int _hashBankCount = 0;
  uint32_t _hashArray[9] = { 0, 0xB9D1FBFD, 0xFF206483, 0xC829A0B9, 0x832459EC, 0xEFE83F42, 0xEFB7A415, 0xDF4BD379, 0xE21B5202 };
  uint8_t convertedHashArray[32];

  if (_myFile) {
    _hashValue = "";  //reset the hash
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
      switch (j) {
        case 1:
          convertedHashArray[start] = _hashArray[i] >> 24 & 0xFF;
          start++;
          break;
        case 2:
          convertedHashArray[start] = _hashArray[i] >> 16 & 0xFF;
          start++;
          break;
        case 3:
          convertedHashArray[start] = _hashArray[i] >> 8 & 0xFF;
          start++;
          break;
        case 4:
          convertedHashArray[start] = _hashArray[i] & 0xFF;
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
  convertHashArray2String(convertedHashArray, sizeof(convertedHashArray));

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


// This will take in the array and convert it to a String
void firmware::convertHashArray2String(uint8_t* arrayToConvert, uint8_t size) {
  String arrayHashLength = "";

  for (int i = 0; i < size; i++) {
    arrayHashLength = String(arrayToConvert[i], HEX);

    if (arrayHashLength.length() == 1) {
      String modified = "0";
      modified += arrayHashLength;
      _hashValue += modified;
    } else {
      _hashValue += arrayHashLength;
    }
  }
}



//return a 32 unsigned value
uint32_t firmware::fileSize() {

  return _myFile.size();
}

// ADDED 6-29-2025
//------------------------//
// RESEND DROPPED PACKETS //
//------------------------//
void firmware::_resendDroppedPackets(uint8_t* droppedPackets, uint8_t index) {
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
        for (int i = 0; i < 200; i++) {
          SerialUSB.print(binaryData.sendData[i], HEX);
          SerialUSB.print(",");
        }
        SerialUSB.println();
        // Increment the internalIndex count
        internalIndex += 1;
      }

      packetCount++;
    }
    _myFile.close();
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

void firmware::packetDataAvailable_Sender(){

    if (myDriver.available()) {
    byte buf[myDriver.maxMessageLength()];
    byte len = sizeof(buf);

    if (myDriver.waitAvailableTimeout(200)) {
      while (myDriver.recv(buf, &len)) {

        //copy the incoming data to the incomingBuffer
        memcpy(incomingBuffer, buf, sizeof(buf));

        //--------------------------------------------------------//
        // [RECEIVING MESSAGES] DROPPED PACKETS/ FILE SEND CONFIR.//
        //--------------------------------------------------------//
        if (incomingBuffer[0] == sizeof(dropped)) {

          memcpy(&dropped, incomingBuffer, sizeof(dropped));

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
          SerialUSB.println("Done sending droppped packets");
          // Reset the droppedPacketFlag otherwise on the receive side it messes with how the data is read/wrote into the file
          binaryData.droppedPacketFlag = false;  

          //-----------------//  Example: 49 packet dropped
          //ref. file - exel //  (49 -1) * 200 = 9600 bytes from file start
          //-----------------//  packet 49: start 9600, stop 9799  [200] bytes
          // wipe the memory location just for testing purposes
          memset(dropped.resendPackets, 0, sizeof(dropped.resendPackets));
        } else if (incomingBuffer[0] == sizeof(fileData)) {

          memcpy(&fileData, incomingBuffer, sizeof(fileData));

          //-----------------------//
          //  REMOTE STATUS CHECK  //
          //-----------------------//
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
      }
    }
  }
}
