
#include "esp8266truerandom.h"
#include "PersistentStorage.h"

#define STATE_UNKNOWN 0
#define STATE_AP 1
#define STATE_STARTING_STA 2
#define STATE_STA 3

class WifiHandler
{
  private:
    PersistentStorage _configuration;
    int _wifiState; 
    int _lastWifiState = -1;

  
  public:

    void LoadConfiguration()
    {
      _configuration.Load();
      _wifiState = STATE_STARTING_STA;
    }

    void setParamsForDebug(String ssid, String password, String hostname)
    {
      _configuration.SsidSet(ssid);
      _configuration.PasswordSet(password);
      _configuration.HostNameSet(hostname);
      _wifiState = STATE_STARTING_STA;
    }

    void setWirelessParametersAndRestart(String ssid, String password)
    {
      Serial.println("setWirelessParametersAndRestart");
      
      _configuration.SsidSet(ssid);
      _configuration.PasswordSet(password);
      _wifiState = STATE_STARTING_STA;
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
      Serial.println(_configuration._ssid);
      Serial.println(_configuration._password);
      Serial.print(WiFi.status());
      
      WiFi.mode(WIFI_STA);
      while (WiFi.getMode() != WIFI_STA)
      {
        Serial.print("M");
        delay(50);
      }
      WiFi.disconnect();
      Serial.println(_configuration.HostNameGet());
      WiFi.hostname(_configuration.HostNameGet());
      WiFi.begin(_configuration._ssid, _configuration._password);

      int timeout = 80000;
      while (WiFi.status() != WL_CONNECTED && timeout > 0)
      {
        delay(500);
        switch (WiFi.status())
        {
          case WL_CONNECTED:
            Serial.println("WL_CONNECTED");
            break;
          case WL_NO_SHIELD:
            Serial.println("WL_NO_SHIELD");
            break;
          case WL_IDLE_STATUS:
            Serial.println("WL_IDLE_STATUS");
            break;
          case WL_NO_SSID_AVAIL:
            Serial.println("WL_NO_SSID_AVAIL");
            break;
          case WL_SCAN_COMPLETED:
            Serial.println("WL_SCAN_COMPLETED");
            break;
          case WL_CONNECT_FAILED:
            Serial.println("WL_CONNECT_FAILED");
            break;
          case WL_CONNECTION_LOST:
            Serial.println("WL_CONNECTION_LOST");
            break;
          case WL_DISCONNECTED:
            Serial.println("WL_DISCONNECTED");
            break;
        }
     
        Serial.print(WiFi.status());
        timeout--;
      }

      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.print("Connected: ");
        Serial.println(WiFi.localIP());

        _wifiState = STATE_STA;
        _configuration.Save();
      }
      else
      {
        Serial.print("Connect failed, status = ");
        Serial.println(WiFi.status());
      }
    }

    void startAsAccessPoint() {
      generateRandomName();
      
      Serial.print("Setting soft-AP:  ");
      Serial.println(_configuration._hostName);
      WiFi.mode(WIFI_AP);
      boolean result = WiFi.softAP(_configuration._hostName, _configuration._hostName);
      if(result == true)
      {
        Serial.println("Ready");
        IPAddress myIP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(myIP);
        Serial.println("HTTP server started");
        _wifiState = STATE_AP;
      }
      else
      {
        Serial.println("Failed!");
      }
    }  

    void generateRandomName()
    {
      Serial.println("generateRandonName");
       char buf[16];
      ltoa(ESP8266TrueRandom.random(), buf, 10);
      String hostName = String("EDP_") + buf;
      
      _configuration.HostNameSet(hostName);
    }

    void Init()
    {
      if (_wifiState != _lastWifiState)
      {
        Serial.print("handleServerTasks, state = ");
        Serial.println(_wifiState);
        _lastWifiState = _wifiState;
      }
      
        // if we aren't in station mode and we have an SSID, try to start up in station mode:
      if (_wifiState == STATE_STARTING_STA && _configuration.SsidGet().length() != 0)
      {
        startAsClient(); 
      }

      if (_wifiState == STATE_UNKNOWN)
      {
        startAsAccessPoint();        
      }
    }    
};

