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

#define STATE_UNKNOWN 0
#define STATE_AP 1
#define STATE_STARTING_STA 2
#define STATE_STA 3
    
class WifiHandler
{
  private:
    // static instance and methods so we can get back to real code...
    static WifiHandler* _pInstance;
    
    static void handleNotFoundStatic() {
      _pInstance->handleNotFound();
    }


    static void handleProvisionStatic() {
      _pInstance->handleProvision();
    }

    ESP8266WebServer* _pServer;
    String _ssid;
    String _password;
    String _hostname;
    int _wifiState;

    void handleNotFound() {
      Serial.println("handleNotFound");
      _pServer->send(200, "text/html", "<h1>notFound</h1>");
    }

    void handleProvision() {
      Serial.println("provision");

      if (_wifiState != STATE_AP)
      {
        return;
      }
      
      if (_pServer->hasArg("ssid"))
      {
        _ssid = _pServer->arg("ssid");
        Serial.println(_ssid);
        _wifiState = STATE_STARTING_STA;
      }
      if (_pServer->hasArg("password"))
      { 
        _password = _pServer->arg("password");
        Serial.println(_password);
      }
    }

    void startAsClient()
    {
      Serial.println("Disconnecting, waiting to finish");
      //WiFi.mode(WIFI_OFF);
      WiFi.disconnect();
      while (WiFi.status() != WL_DISCONNECTED && WiFi.status() != WL_IDLE_STATUS)
      {
        Serial.println(WiFi.status());
        delay(50);
      }

      Serial.println("starting STA");
      Serial.print("Connecting to: ");
      Serial.println(_ssid.c_str());
      Serial.println(_password.c_str());
      Serial.print(WiFi.status());
      
      WiFi.mode(WIFI_STA);
      while (WiFi.getMode() != WIFI_STA)
      {
        Serial.print("M");
        delay(50);
      }
      WiFi.disconnect();
      Serial.println(_hostname.c_str());
      WiFi.hostname(_hostname.c_str());
      WiFi.begin(_ssid.c_str(), _password.c_str());

      //WiFi.hostname(_hostname.c_str());
      //WiFi.mode(WIFI_STA);
      
      //WiFi.begin(_ssid.c_str(), _password.c_str());

      int timeout = 40;
      while (WiFi.status() != WL_CONNECTED && timeout > 0)
      {
        delay(500);
        Serial.print(WiFi.status());
        timeout--;
      }

      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("Connected");
        Serial.println(WiFi.localIP());

        //_pServer->onNotFound(handleNotFoundStatic);
        _pServer->begin();
        Serial.println("HTTP server started");
        _wifiState = STATE_STA;
      }
      else
      {
        Serial.print("Connect failed, status = ");
        Serial.println(WiFi.status());
      }
    }

    void generateRandomName()
    {
      Serial.println("generateRandonName");
       char buf[16];
      ltoa(ESP8266TrueRandom.random(), buf, 10);
      _hostname = "EDP_";
      _hostname.concat(buf);
    }
  
    void startAsServer() {

      Serial.print("Setting soft-AP ... ");
      Serial.println(_hostname);
      WiFi.mode(WIFI_AP);
      boolean result = WiFi.softAP(_hostname.c_str(), _hostname.c_str());
      if(result == true)
      {
        Serial.println("Ready");
        IPAddress myIP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(myIP);
        //_pServer->on("/provision", handleProvisionStatic);
        //_pServer->onNotFound(handleNotFoundStatic);
        _pServer->begin();
        Serial.println("HTTP server started");
        _wifiState = STATE_AP;
      }
      else
      {
        Serial.println("Failed!");
      }
    }    

  public:
    WifiHandler()
    {
      _pInstance = this;
      _wifiState = STATE_UNKNOWN;
    }

    int _lastWifiState = 0;

    void handleServerTasks()
    {
      if (!_pServer)
      {
        _pServer = new ESP8266WebServer(80);
        generateRandomName();
        _pServer->on("/provision", handleProvisionStatic);
        _pServer->onNotFound(handleNotFoundStatic);
      }
      
      if (_wifiState != _lastWifiState)
      {
        Serial.print("handleServerTasks, state = ");
        Serial.println(_wifiState);
        _lastWifiState = _wifiState;
      }
      
        // if we aren't in station mode and we have an SSID, try to start up in station mode:
      if (_wifiState == STATE_STARTING_STA && _ssid.length() != 0)
      {
        startAsClient(); 
      }

      if (_wifiState == STATE_UNKNOWN)
      {
        startAsServer();        
      }
      
      _pServer->handleClient();
    }
};

WifiHandler* WifiHandler::_pInstance = 0;


