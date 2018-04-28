#include <Arduino.h>
#include <Task.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#include <NeoPixelBus.h>


//#define FASTLED_INTERNAL
//#include <FastLED.h>

extern "C" {
#include "user_interface.h"
}

#include "WifiHandler.h"
#include "taskProcessMessages.h"
#include "webServer.h"

const uint16_t PixelCount = 33; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 128

// three element pixels, in different order and speeds
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

int maxTicks;

RgbColor r(64, 0, 0);

char incomingPacket[255];  // buffer for incoming packets

WifiHandler wifiHandler;

TaskManager taskManager;
TaskProcessMessages taskProcessMessages(LED_BUILTIN, MsToTaskTime(5), &strip, &wifiHandler);

WebServer webServer;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  //Serial.setDebugOutput(true);

  //wifiHandler.setParamsForDebug("ASUS", "Gunnerson", "EDP44");

  wifiHandler.LoadConfiguration();
  wifiHandler.Init();

  taskProcessMessages.Init();

  webServer.SetWifiHandler(&wifiHandler);
  webServer.Init();

  maxTicks = 0;

  taskManager.StartTask(&taskProcessMessages);

  strip.Begin();
  for (int i = 0; i < PixelCount; i++)
  {
    strip.SetPixelColor(i, r);
  }
  strip.Show();
}

void loop() {
  taskManager.Loop();

  //int startTicks = system_get_time();
}


