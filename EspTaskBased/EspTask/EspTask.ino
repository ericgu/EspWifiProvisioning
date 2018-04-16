#include <Arduino.h>
#include <Task.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include <NeoPixelBus.h>

//#define FASTLED_INTERNAL
//#include <FastLED.h>

extern "C" {
#include "user_interface.h"
}

#include "taskBlinkLed.h"

const uint16_t PixelCount = 33; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 128

// three element pixels, in different order and speeds
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
//NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(PixelCount, PixelPin);

WiFiServer server(5045);
WiFiClient client;
int maxTicks;

TaskManager taskManager;
TaskBlinkLed taskBlinkLed(LED_BUILTIN, MsToTaskTime(100), strip);

RgbColor r(colorSaturation, 0, 0);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

  // put your setup code here, to run once:

   Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.hostname("EDP44");
  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.begin("ASUS", "Gunnerson");
      int timeout = 40;
      while (WiFi.status() != WL_CONNECTED && timeout > 0)
      {
        delay(500);
        Serial.print(WiFi.status());
        timeout--;
      }

  Serial.println("Connected");
  Serial.printf("Now listening at IP %s\n", WiFi.localIP().toString().c_str());  

  server.begin();

  maxTicks = 0;

  taskManager.StartTask(&taskBlinkLed);

  strip.Begin();
  for (int i = 0; i < PixelCount; i++)
  {
    strip.SetPixelColor(i, r);
  }
  strip.Show();
}

void loop() {
  taskManager.Loop();

  Serial.println(".");

    int startTicks = system_get_time();

  if (!client.connected()) {
    client = server.available();
    taskBlinkLed.SetClient(&client);
  }
 
}
