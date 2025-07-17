/*

      Author: Ethan Dechant
Date created: [(finished) 7-15-2025] October 28th, 2024   


This example is for the Transceiver that is incharge of 
sending the binary file to the remote. It handles the 
serial communication from the computer along with breaking
up the file and sending it as packets, where it also
deals with resending packets that have been dropped after
the inital file transfer. 

** This sketch is intented to work in conjunction with the fileTransferRemote example **
*/


#include "fileTransfer.h"

uint8_t RFM_95CS = 12;
uint8_t RFM_95INT = 6;
const int RFM_95RST = 7;  // NOT USED
uint8_t pinIntSD = 4;
double frequency = 921.2;

unsigned char encryptKey[16] = **REDACTED**;

// Object for the firmware Class [chip select, interrupt] pins
fileTransfer firmwareUpdate(RFM_95CS, RFM_95INT, pinIntSD,"file1.bin");


void setup() {
  SerialUSB.begin(9600);
  pinMode(13, OUTPUT);
  while (!SerialUSB) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
 
  //-------------------//
  //  SETUP THE RADIO  //
  //-------------------//
  if (firmwareUpdate.rf95.init() == false) {
      while(1);
      SerialUSB.println("RF radio not attached, test!");
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
      firmwareUpdate.rf95.setFrequency(921.2);   
	  delay(100);

      firmwareUpdate.rf95.setTxPower(15, false);
      delay(100);

      firmwareUpdate.myCipher.setKey(encryptKey, 16);
      delay(100);
      SerialUSB.println("RF Receiver- Good to Go!");
      digitalWrite(LED_BUILTIN, LOW);
  }

}

void loop() {
  //------------------------------//
  // HANDLES SERIAL COMMUNICATION //
  //------------------------------//
  firmwareUpdate.receiveCommandLocal();
  // --> Serial commands
  // --> block to send the binary file


  //--------------------------//
  // HANDLES RF COMMUNICATION //
  //--------------------------//
  firmwareUpdate.packetDataAvailable_Sender();
  // --> RF communication
  // --> block to send/receive dropped packets
}
