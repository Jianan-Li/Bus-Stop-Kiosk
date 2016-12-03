#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <SPI.h>
#include <DMD.h>
#include <TimerOne.h>
#include "SystemFont5x7.h"

// IO
#define WAKE_UP_PIN 2

// Display Initialization
#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 2
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

// Program defines and variables
#define ROUTE_DATA_LENGTH 10
byte previousHeader = 0;
byte previousNumRoutesAvail = 0;

void wakeupInterrupt(void){
  // detach the interrupt to stop it from continuously firing while the interrupt pin is low
  detachInterrupt(0);
}

void enterSleep(void){
  // attachInterrupt before going to sleep
  attachInterrupt(0, wakeupInterrupt, RISING);

  // See http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__avr__sleep.html
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli(); // noInterrupts()
  sleep_enable();
  sei();  // interrupts()
  sleep_cpu();
  sleep_disable();
}

void ScanDMD(){
  dmd.scanDisplayBySPI();
}

void setup(void){
  // Turn off unused peripherals
  ADCSRA &= ~(1<<ADEN); //Disable the ADC
  ACSR = (1<<ACD);      //Disable the analog comparator
  
  pinMode(WAKE_UP_PIN, INPUT);
  Serial.begin(19200);
  dmd.selectFont(System5x7);
  
  // Timer1 interrupt f = 1s/0.002s = 500Hz; display refresh f = 500Hz/4 = 125Hz
  // 1s/0.0004s = 2500Hz; f = 2500Hz/4 = 625Hz
  Timer1.initialize(400);    
  Timer1.attachInterrupt(ScanDMD);
}

void loop(void){
  if (Serial.available() > 0) {
    char dataBuffer[60];
    byte bytesRead = Serial.readBytesUntil('\0', dataBuffer, sizeof(dataBuffer)); 
    byte header = dataBuffer[0]; //1-Data available, 2-Data not available, 3-API request error, 4-Go to sleep
    if (header!=4) {
      byte brightness = dataBuffer[1]; // 0-255
      dmd.setBrightness(brightness);
    }
    
    byte numRoutesAvail = 0;  
    switch (header) {
      case 1:
        numRoutesAvail = (bytesRead-2)/ROUTE_DATA_LENGTH;
        if (previousHeader!=1 || numRoutesAvail < previousNumRoutesAvail) dmd.clearScreen();
        for (int i = 0; i < numRoutesAvail; i++)
          dmd.drawString(2, i*8, dataBuffer+2+ROUTE_DATA_LENGTH*i, ROUTE_DATA_LENGTH, GRAPHICS_NORMAL);
        break;
      case 2:
        if (previousHeader!=2) {
          dmd.clearScreen();
          dmd.drawString(2,  0, "Bus Data",  8, GRAPHICS_NORMAL);
          dmd.drawString(2,  8, "Not",       3, GRAPHICS_NORMAL);
          dmd.drawString(2, 16, "Available", 9, GRAPHICS_NORMAL);
        }
        break;
      case 3:
        if (previousHeader!=3) {
          dmd.clearScreen();
          dmd.drawString(2,  0, "API",     3, GRAPHICS_NORMAL);
          dmd.drawString(2,  8, "Request", 7, GRAPHICS_NORMAL);
          dmd.drawString(2, 16, "Error",   5, GRAPHICS_NORMAL);
        }
        break;
      case 4:
        // Before going to sleep, clear the display and set brightness to 0
        dmd.clearScreen();
        dmd.setBrightness(0);
        enterSleep();
        break;
    }   
    // update the previousHeader to current header
    previousHeader = header;
    previousNumRoutesAvail = numRoutesAvail;
  }
}