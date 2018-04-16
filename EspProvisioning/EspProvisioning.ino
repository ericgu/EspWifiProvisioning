
extern "C" {
#include <user_interface.h>
}

#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <ESP8266WebServer.h>

#include "esp8266TrueRandom.h"
#define FASTLED_INTERNAL

#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"

#include "WifiHandler.h"

WifiHandler wifiHandler;

#define DATA_PIN    2
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    33
CRGB leds[NUM_LEDS];

CRGB first;
int rgbUpdated = 0;

void rgbChanged(int red, int green, int blue)
{
  first = CRGB(red, green, blue);
  rgbUpdated = 1;

  uint32_t free = system_get_free_heap_size();  
  Serial.println(free);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS);
  wifiHandler.setRgbHandler(rgbChanged);

  first = CRGB::Red;
}

void loop()
{
  wifiHandler.handleServerTasks();

  if (!rgbUpdated)
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    leds[0] = first;
    FastLED.show();
  
    delay(500);                       // wait for a second

    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    leds[0] = CRGB::Green;
    FastLED.show();
    delay(500);   
  }
  else
  {
    for (int i = 0; i < 33; i++)
    {
      leds[i] = first;
    }
    FastLED.show();
  }
}
