#include <Arduino.h>
#include <Task.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#include <NeoPixelBus.h>

extern "C" {
#include "user_interface.h"
}

#include "PersistentStorage.h"
#include "WifiHandler.h"
#include "PixelHandler.h"
#include "taskProcessMessages.h"
#include "webServer.h"

PersistentStorage persistentStorage;

const uint16_t PixelCount = 33; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266

PixelHandler pixelHandler(&persistentStorage, PixelCount, PixelPin);
WifiHandler wifiHandler;
TaskManager taskManager;

TaskProcessMessages taskProcessMessages(LED_BUILTIN, MsToTaskTime(5), &pixelHandler, &wifiHandler);

WebServer webServer;

void setup() {
  Serial.begin(115200);
  //Serial.setDebugOutput(true);

  //strcpy(persistentStorage._ssid, "junkjunkjunk");  // set bad wifi params for testing...
  //persistentStorage.Save();
  persistentStorage.Load();
  
  Serial.print("Starting: ");
  Serial.println(persistentStorage._ssid);

  pixelHandler.Init();

  taskProcessMessages.Init();

  webServer.SetHandlers(&wifiHandler, &pixelHandler);
  webServer.Init();

  taskManager.StartTask(&taskProcessMessages);

  wifiHandler.LoadConfiguration(&persistentStorage);
  //wifiHandler.setParamsForDebug("ASUS", "Gunnerson", "EDP44");
  wifiHandler.Init();
}

void loop() {
  taskManager.Loop();
}


