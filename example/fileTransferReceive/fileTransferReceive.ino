/*

      Author: Ethan Dechant
Date created: [(finished) 7-28-2025] October 28th, 2024   


This example sketch is for the Receiver side as its only 
job is to receive file data commands and binary file data
in order to build the file to the correct size and re-send
dropped packet information to the Sender in the event that 
some are dropped, it will write-over the dropped packet blocks
with '0' in order to maintain the correct block size for when
data is re-written.

** This sketch is intented to work in conjunction with the fileTransferSend example **
*/


#include "fileTransfer.h"

uint8_t RFM_95CS = 12;
uint8_t RFM_95INT = 6;
const int RFM_95RST = 7;  // NOT USED
uint8_t pinIntSD = 4;
double frequency = 921.2;

unsigned char encryptKey[16] = **REDACTED**;

// Object for the firmware Class [chip select, interrupt] pins
fileTransfer firmwareUpdate(RFM_95CS, RFM_95INT, pinIntSD,"runBin.bin");


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
  //--------------------------//
  // HANDLES RF COMMUNICATION //
  //--------------------------//
  firmwareUpdate.packetDataAvailable("receiver");
  // --> RF communication
  // --> block to send/receive binary data along with dropped packets
}
