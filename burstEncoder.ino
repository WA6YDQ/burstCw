/* burstEncoder */

/*
*  This is a cw burst encoder based on the AN/GRA-71
* CW burst encoder, specifically the MX-4495 component.
*
* There are 5 buttons and an LED. The output to the 
* transmitter is an NPN transistor as an open collector
* that is tied to the KEY line. 
*
* Button 1:  Play/Record (Short press/Long press)
* Button 2:  DOT
* Button 3:  Space
* Button 4:  DASH
*
* LED: Show power on (3 flashes), Play (continous on),
* Record Start (2 flashes), Record Stop (flashes while 
* writing to EEPROM), Playback buffer empty (8 flashes).
*
*
* k theis <theis.kurt@gmail.com> 11/2018
*
*/

#include <EEPROM.h>

#define MAXBUFLEN 1000  // maximum character buffer length
#define CHARDELAY 3  // length of dit at 300 wpm is 3 msec, 15 wpm is 125 msec.
#define DEBOUNCE 90    // debounce time in ms for switches

/* Global Variables */

// define input buttons
const int PLREC = 4;    // play/record button
const int DOT = 5;      // DOT button
const int SPACE = 6;    // SPACE button
const int DASH = 7;     // DASH button

// define output lines
const int LED = 9;      // Power/Play/Record feedback
const int KEY = 13 ;    // KEY line to transmitter

// EEPROM addresses
const int lastPOS = 1;     // address in EEPROM of last position used
const int startOfBuffer = 10;  // address in EEPROM of beginning of buffer

unsigned char MSG[1000];    // buffer for storage in RAM (makes record process faster)




/* Routines */

/****** Write to EEPROM *******/

void writeEE(int address, int DATA) {    // write a byte to EEPROM
  EEPROM.write(address, DATA);
  return;
}



/******** Read from EEPROM *********/

int readEE(int address) {    // read a byte from EEPROM
  return EEPROM.read(address);
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(KEY, OUTPUT);
  digitalWrite(KEY, LOW);    // make sure keyline is dekeyed at startup
  pinMode(PLREC, INPUT_PULLUP);
  pinMode(DOT, INPUT_PULLUP);
  pinMode(SPACE, INPUT_PULLUP);
  pinMode(DASH, INPUT_PULLUP); 
}

void pulseLed() {
  digitalWrite(LED, HIGH);
  delay(75);
  digitalWrite(LED, LOW);
  delay(75);
  return;
}



void fastPulse() {
  digitalWrite(LED, HIGH);
  delay(25);
  digitalWrite(LED, LOW);
  delay(25);
  return;
}




/************* Play a Message ************/

void play2msg() {  // version 2 - play bit by bit
  unsigned char msgByte;
  int bitnum, ct;
  int curPos;

  // test if buffer is empty
  if (readEE(lastPOS) == startOfBuffer) {    // if empty, flash LED 8 times
    for (ct=0; ct < 8; ct++)
      pulseLed();
    return;
  }
    
  digitalWrite(LED, HIGH);
  for (curPos = startOfBuffer; curPos < readEE(lastPOS); curPos++) {
    msgByte = readEE(curPos);    // get a byte from EEPROM
    for (bitnum = 0; bitnum < 8; bitnum++) {  // cycle thru each bit
      if ((msgByte & (1 << bitnum)) == 0) {    // bit is low
        digitalWrite(KEY, LOW);    // dekey
        delay(CHARDELAY);
      }
      else {    // bit is high
        digitalWrite(KEY, HIGH);  // == 1 so keyup
        delay(CHARDELAY);
      }
    }
  }
  // finished - make sure we dekey
  digitalWrite(KEY, LOW);
  digitalWrite(LED, LOW);
  return;  // done
}
      




/*************** Record a Message ***************/

void record2msg() {    // version 2 - record bit by bit
  unsigned char msgByte;
  int ct;        // used while writing to EEPROM
  int bitnum;    // bit 0-7
  int curPos = startOfBuffer;    // current eeprom byte position
  bitnum = 0;
  while (1) {
    if (digitalRead(DOT) == LOW) {  // save 2 bits: a 1 and a 0
       msgByte |= 1UL << bitnum;  // set a bit
       pulseLed();
       bitnum++;
       if (bitnum == 8) {
         MSG[curPos] = msgByte;    // write to RAM
         curPos++;
         bitnum = 0;
       }
       msgByte &= ~(1UL << bitnum);   // clear a bit
       bitnum++;
       if (bitnum == 8) {
         MSG[curPos] = msgByte;    // write to RAM
         curPos++;
         bitnum = 0;
       }
       while (digitalRead(DOT) == LOW) {  // wait till released
         delay(DEBOUNCE);
         continue;
       }
       continue;
    } 
    
    if (digitalRead(DASH) == LOW) {  // save 4 bits: 3 1's and a 0
       msgByte |= 1UL << bitnum;  // set a bit
       pulseLed();
       bitnum++;
       if (bitnum == 8) {
         MSG[curPos] = msgByte;    // write to RAM
         curPos++;
         bitnum = 0;
       }
       msgByte |= 1UL << bitnum;  // set a bit
       bitnum++;
       if (bitnum == 8) {
         MSG[curPos] = msgByte;    // write to RAM
         curPos++;
         bitnum = 0;
       }
       msgByte |= 1UL << bitnum;  // set a bit
       bitnum++;
       if (bitnum == 8) {
         MSG[curPos] = msgByte;    // write to RAM
         curPos++;
         bitnum = 0;
       }
       msgByte &= ~(1UL << bitnum);   // clear a bit
       bitnum++;
       if (bitnum == 8) {
         MSG[curPos] = msgByte;    // write to RAM
         curPos++;
         bitnum = 0;
       }
       while (digitalRead(DASH) == LOW) {  // wait till released
         delay(DEBOUNCE);
         continue;
       }
       continue;
    }
    
    if (digitalRead(SPACE) == LOW) {  // save 3 bits: 3 0's
       msgByte &= ~(1UL << bitnum);   // clear a bit
       pulseLed();
       bitnum++;
       if (bitnum == 8) {
         MSG[curPos] = msgByte;    // write to RAM
         curPos++;
         bitnum = 0;
       }
       msgByte &= ~(1UL << bitnum);   // clear a bit
       bitnum++;
       if (bitnum == 8) {
         MSG[curPos] = msgByte;    // write to RAM
         curPos++;
         bitnum = 0;
       }
       msgByte &= ~(1UL << bitnum);   // clear a bit
       bitnum++;
       if (bitnum == 8) {
         MSG[curPos] = msgByte;    // write to RAM
         curPos++;
         bitnum = 0;
       }
       while (digitalRead(SPACE) == LOW) {  // wait till released
         delay(DEBOUNCE);
         continue;
       }
       continue;
    }
    
    if (digitalRead(PLREC) == LOW) {   // test end of record 
      digitalWrite(LED, HIGH);      // turn on LED
      while (digitalRead(PLREC) == LOW) {   // keep on until released
        delay(DEBOUNCE);
        continue;
      }
      digitalWrite(LED, LOW);      // turn off LED
      delay(DEBOUNCE);
      
      while (bitnum < 8) {
        msgByte &= ~(1UL << bitnum);   // clear a bit - make sure the rest of the bits in this byte are 0
        bitnum++;
      }
  
      MSG[curPos] = msgByte;    // write to RAM

      // now transfer from RAM to EEPROM
      for (ct=startOfBuffer; ct <= curPos; ct++) {
        writeEE(ct,MSG[ct]);
        fastPulse();
      }
      writeEE(lastPOS,curPos);         // save position location
      return;    // done
    }
  }
}



/*********** MAIN LOOP ************/

void loop() {
  // initial power on, flash the LED
  pulseLed();
  pulseLed();
  pulseLed();
  
  // wait for the Play/Record button to become active
  while (1) {
    if (digitalRead(PLREC) == HIGH)    // wait until pressed
      continue;  
    delay(25);    // 25 msec debounce
    if (digitalRead(PLREC) == HIGH)    // glitch? not currently pressed
      continue;  

    //pulseLed();
    // button pressed - time it to see if Play or Record

    delay(250);
    if (digitalRead(PLREC) == HIGH) {  // released - PLAY selected
      //playMessage();
      play2msg();
      continue;
    }
      
    delay(350);      // long press   
    pulseLed();      // flash LED twice - shows we're in record mode
    pulseLed();
    // now wait until button is released
    while (digitalRead(PLREC) == LOW){
        delay(DEBOUNCE);
        continue;
    }
          
    // go to record mode. Continue recording until PLAYREC is again pressed
    // recordMessage();
    record2msg();

    // back from record - continue with main loop forever
    // (or at least until power is turned off)
   continue;
   
  }
}
