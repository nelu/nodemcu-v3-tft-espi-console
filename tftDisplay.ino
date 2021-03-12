#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>
#include <SysCall.h>

#include <ESP8266WiFi.h>
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include "Logger.h"



//int startMem = freeMemory();

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
Logger cl = Logger(&tft);

void preinit() {
  // Global WiFi constructors are not called yet
  // (global class instances like WiFi, Serial... are not yet initialized)..
  // No global object methods or C++ exceptions can be called in here!
  //The below is a static class method, which is similar to a function, so it's ok.
  ESP8266WiFiClass::preinitWiFiOff();
}

void setup(void) {

  Serial.begin(115200);

  uint16_t time = millis();

  tft.init();
  cl.setDisplay()
    ->setLineDelay(300)
    ->setDelayAfterLines(2)
    ->setTitle(String(F("System setup")).c_str())
    ->println(F("Display Initialized"));

  SdSetup(); // wifi config in storage file 
  WifiSetup();

  
  time = millis() - time;

  cl.print(F("+ Setup done in (ms): "));
  cl.println(time, DEC);
 // cl.printf("Mem start/now: %d/%d\n", startMem, freeMemory());
  delay(2000);
}

void loop()
{
  WifiDisplay();
  delay(3000);
  SdLoop();
  delay(2000);
  tft.invertDisplay(millis()%2 == 0);
 //     cl.printf("Mem start/now: %d/%d\n", startMem, freeMemory());
  delay(2000);


}

// shortcurts
size_t log(const __FlashStringHelper *ifsh) {
  return cl.println(ifsh);
}

