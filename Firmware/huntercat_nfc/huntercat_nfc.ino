/**
   Example to detect NFC readers, read a Mifare Card, read a Visa card
   and emulate a Visa MSD card.

   Authors:
          Salvador Mendoza - @Netxing - salmg.net
          For Electronic Cats - electroniccats.com

   March 2021

   This code is beerware; if you see me (or any other collaborator
   member) at the local, and you've found our code helpful,
   please buy us a round!
   Distributed as-is; no warranty is given.

   ---
   
   This code will detect NFC card readers.

   When it starts, the three LEDs will be on, and then will turn off after a second.
   Then a constant LED 1 will start flashing meaning that is scanning for NFC readers.
   When a NFC reader is detected, LED 2 and 3 will be on for a second.
   
*/

#include "Electroniccats_PN7150.h"

#define PN7150_IRQ   (15)
#define PN7150_VEN   (14)
#define PN7150_ADDR  (0x28)

Electroniccats_PN7150 nfc(PN7150_IRQ, PN7150_VEN, PN7150_ADDR);  // creates a global NFC device interface object, attached to pins 7 (IRQ) and 8 (VEN) and using the default I2C address 0x28
RfIntf_t RfInterface;                                            //Intarface to save data for multiple tags

unsigned char STATUSOK[] = {0x90, 0x00}, Cmd[256], CmdSize;

uint8_t mode = 2;  // modes: 1 = Reader/ Writer, 2 = Emulation

void blink(int pin, int msdelay, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(msdelay);
    digitalWrite(pin, LOW);
    delay(msdelay);
  }
}

//Detect NFC Readers
void nfcdetectreader() {
  Serial.println("Looking for card readers...");
  if (nfc.CardModeReceive(Cmd, &CmdSize) == 0) { //Data in buffer?
    if ((CmdSize >= 2) && (Cmd[0] == 0x00)) { //Expect at least two bytes
      switch (Cmd[1]) {
        case 0xA4: //Something tries to select a file, meaning that it is a reader
          Serial.println("Card reader detected!");
          blink(PIN_LED2, 200, 3);
          break;

        default:
          break;
      }
      nfc.CardModeSend(STATUSOK, sizeof(STATUSOK));
    }
  }
}

void setup() {
  Serial.begin(9600);
  //while (!Serial);
  Serial.println("Detecting NFC readers with PN7150");
  
  pinMode(LED_BUILTIN, OUTPUT); //RED
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(PIN_LED2, LOW);
  digitalWrite(PIN_LED3, LOW);

  Serial.println("Initializing...");
  if (nfc.connectNCI()) { //Wake up the board
    Serial.println("Error while setting up the mode, check connections!");
    while (1);
  }

  if (nfc.ConfigureSettings()) {
    Serial.println("The Configure Settings failed!");
    while (1);
  }

  if (nfc.ConfigMode(mode)) { //Set up the configuration mode
    Serial.println("The Configure Mode failed!!");
    while (1);
  }
  nfc.StartDiscovery(mode); //NCI Discovery mode
  blink(LED_BUILTIN, 200, 2);
  blink(PIN_LED2, 200, 2);
  blink(PIN_LED3, 200, 2);
  Serial.println("HunterCat NFC v1.0");
}

// to detect NFC card readers
void loop() {
  Serial.println("Looking for card readers...");
  if (nfc.CardModeReceive(Cmd, &CmdSize) == 0) { //Data in buffer?
    if ((CmdSize >= 2) && (Cmd[0] == 0x00)) { //Expect at least two bytes
      switch (Cmd[1]) {
        case 0xA4: //If tries to select a file, meaning that it is a reader
          Serial.println("Card reader detected!");
          digitalWrite(PIN_LED2, HIGH);
          digitalWrite(PIN_LED3, HIGH);
          delay(1000);
          digitalWrite(PIN_LED2, LOW);
          digitalWrite(PIN_LED3, LOW);
          break;

        default:
          break;
      }
      nfc.CardModeSend(STATUSOK, sizeof(STATUSOK));
    }
  }
  blink(LED_BUILTIN, 10, 1);
}
