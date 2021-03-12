
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <AddrList.h>

int n;

void WifiSetup() {
    //Serial.begin(115200);
    cl.println("- Wifi setup. sleep 5s");

    // during this period, a simple amp meter shows
    // an average of 20mA with a Wemos D1 mini
    // a DSO is needed to check #2111
    delay(5000);

    WiFi.forceSleepWake();
    WiFi.mode(WIFI_STA);
    // WiFi.disconnect(); 
    // amp meter raises to 75mA
    delay(5000);

    // amp meter cycles within 75-80 mA
    cl.println("+ WIFI Initialized");

}

void WifiDisplay() {
  WifiScreen();
  cl.println(" ...scanning... ");

  
  n = WiFi.scanNetworks();

  if (n == 0) {
    cl.print("(no networks found)");
      }
   else if (n == WIFI_SCAN_RUNNING)
  {
    

      // lcd.setCursor(0, 0);
      cl.println("\nScan already running"); // Start Print text to Line 1
      cl.println("--------"); // Start Print Test to Line 2

  } else {
    cl.print("(");
   // tft.setTextColor(CYAN);
    cl.print(n);
   // tft.setTextColor(WHITE);
    cl.println(" networks found)");
    int16_t x, y;
    
    for (int i = 0; i < n; ++i)
    {


      cl.print(i + 1);
      cl.print(" ");
      cl.print(WiFi.RSSI(i));
      cl.print(" ");
      if (WiFi.encryptionType(i) == ENC_TYPE_NONE) {
            cl.print("*");
      }
      cl.println(WiFi.SSID(i));

    }
  }

}

void WifiScreen() {

  cl.setTitle(String(F("WIFI: Networks")).c_str())
  ->clearContent();

}