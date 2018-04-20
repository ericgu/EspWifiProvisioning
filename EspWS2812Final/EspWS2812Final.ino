#include <Arduino.h>

#include <FunctionTask.h>
#include <MessageTask.h>
#include <Task.h>

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include <NeoPixelBus.h>

#include "taskHandler.h"

const uint16_t PixelCount = 33; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 128

// three element pixels, in different order and speeds
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
//NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(PixelCount, PixelPin);

const char* ssid = "ASUS";
const char* password = "Gunnerson";

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back

TaskManager taskManager;
#define LedPin 13 // BUILTIN_LED
TaskHandler taskHandler(LedPin, MsToTaskTime(10), strip); // every second it will turn on/off the LED

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

  taskManager.StartTask(&taskHandler);

  strip.Begin();
  for (int i = 0; i < PixelCount; i++)
  {
    strip.SetPixelColor(i, r);
  }
  strip.Show();
}

int packetCount = 0;

    int ProcessMessage(char* pMessage)
    {
      int red = 0;
      int green = 0;
      int blue = 0;

      // rgb rrr,ggg,bbb
      if (*pMessage == 'r')
      {
        pMessage += 4;
        red = atoi(pMessage);  
        
        pMessage += 4;
        green = atoi(pMessage); 
         
        pMessage += 4;
        blue = atoi(pMessage);         
        
        pMessage += 4;
        int serial = atoi(pMessage); 

        //_setColor = RgbColor(red, green, blue);

        return serial;
      }
    }

  int lastSerialNumber = -1;
  int missedMessages = 0;

void loop()
{
  taskManager.Loop();
  
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }

    int serial = ProcessMessage(incomingPacket);
    if (serial != lastSerialNumber + 1)
    {
      Serial.println("Missed");
      missedMessages++;
    }
    lastSerialNumber = serial;
    
    //Serial.printf("UDP packet contents: %s\n", incomingPacket);

    // send back a reply, to the IP address and port we got the packet from
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    //Udp.write(replyPacket);
    //Udp.endPacket();

    if (packetCount % 1000 == 0)
    {
      Serial.print(packetCount);
      Serial.print(" ");
      Serial.println(missedMessages);
    }
    packetCount++;
  }
}

#ifdef oldversion

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
#include <EthernetUdp.h>


#include <AsyncPrinter.h>
#include <async_config.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncTCPbuffer.h>
#include <SyncClient.h>
#include <tcp_axtls.h>


#include <ESPAsyncWebServer.h>

#define FASTLED_INTERNAL
#include <FastLED.h>

extern "C" {
#include "user_interface.h"
}

//AsyncWebServer server(80);
//AsyncWebSocket ws("/rgb");

void onRgbEvent(AsyncWebSocket *server, AsyncWebSocketClient* client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    //client->ping();
  
}

EthernetUDP udp;
unsigned int localUdpPort = 8888;
char incomingPacket[255];
char replyPacket[] = "Hello";

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

  // put your setup code here, to run once:

   Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.hostname("EDP44");
  WiFi.mode(WIFI_STA);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  //WiFi.softAP(hostName);
  WiFi.begin("ASUS", "Gunnerson");
  //while (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //  Serial.println(
  //  delay(250);
  //}
      int timeout = 40;
      while (WiFi.status() != WL_CONNECTED && timeout > 0)
      {
        delay(500);
        Serial.print(WiFi.status());
        timeout--;
      }

  Serial.println("Connected");

  //MDNS.addService("http","tcp",80);

  //ws.onEvent(onRgbEvent);
  //server.addHandler(&ws);

#ifdef test
  server.on("/rgb", [](AsyncWebServerRequest* request){
    request->send(200, "text/plain", "<h1>Hello</h1>");
    //Serial.println("rgb");
    //Serial.println(request->url().c_str());

    if (request->hasParam("red"))
    {
      //Serial.println(request->getParam("red")->value().c_str());
      //Serial.println(request->getParam("green")->value().c_str());
      //Serial.println(request->getParam("blue")->value().c_str());
      Serial.print(request->getParam("id")->value().c_str());
      Serial.print(" ");
      Serial.println(system_get_free_heap_size());


      
    }
   
  });

  server.onNotFound([](AsyncWebServerRequest* request){
    Serial.println("not found");
    Serial.println(request->url().c_str());
  });

  server.begin();
#endif 

  if (!udp.begin(localUdpPort))
  {
    Serial.println("Failed on udp.being()");
  }
  else
  {
    Serial.println("udp success");
  }
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);  
}

void loop() {
  // put your main code here, to run repeatedly:
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    //leds[0] = first;
    //FastLED.show();
  
    delay(500);                       // wait for a second

    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    //leds[0] = CRGB::Green;
    //FastLED.show();
    delay(500);  
    //Serial.println(WiFi.status()); 

    int packetSize = udp.parsePacket();
    if (packetSize)
    {
      Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
      int len = udp.read(incomingPacket, 255);

      if (len > 0)
      {
        incomingPacket[len] = 0;
      }
      Serial.printf("UDP packet contents: %s\n", incomingPacket);

          // send back a reply, to the IP address and port we got the packet from
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write(replyPacket);
      udp.endPacket();
    }
}

#endif
