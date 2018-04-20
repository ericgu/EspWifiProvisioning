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

#include "taskBlinkLed.h"

const uint16_t PixelCount = 33; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 128

// three element pixels, in different order and speeds
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
//NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(PixelCount, PixelPin);

//WiFiServer server(5045);
//WiFiClient client;
int maxTicks;

RgbColor r(64, 0, 0);

WiFiUDP udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets

char  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back
TaskManager taskManager;
TaskBlinkLed taskBlinkLed(LED_BUILTIN, MsToTaskTime(5), strip, &udp);


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

  //server.begin();
  udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  maxTicks = 0;

  taskManager.StartTask(&taskBlinkLed);

  strip.Begin();
  for (int i = 0; i < PixelCount; i++)
  {
    strip.SetPixelColor(i, r);
  }
  strip.Show();
}

int count;
int packetCount = 0;

void loop() {
  taskManager.Loop();

  //int startTicks = system_get_time();

#ifdef fred
  int packetSize = udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    int len = udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
  }


    int serial = ProcessMessage(incomingPacket);
    if (serial != lastSerialNumber + 1)
    {
      Serial.println("Missed");
      missedMessages++;
    }
    lastSerialNumber = serial;
#endif
    
    //Serial.printf("UDP packet contents: %s\n", incomingPacket);

    // send back a reply, to the IP address and port we got the packet from
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    //Udp.write(replyPacket);
    //Udp.endPacket();

    //if (packetCount % 1000 == 0)
#ifdef fred
    {
      Serial.print(packetCount);
      Serial.print(" ");
      //Serial.println(missedMessages);
    }
    packetCount++;
#endif
  }

#ifdef fred
  if (!client.connected()) {
    client = server.available();
    if (client)
    {
      taskBlinkLed.SetClient(&client);
    }
  }
#endif

