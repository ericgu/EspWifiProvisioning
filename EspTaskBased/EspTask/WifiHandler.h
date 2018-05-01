
#include "esp8266truerandom.h"

#define STATE_UNKNOWN 0
#define STATE_AP 1
#define STATE_STARTING_STA 2
#define STATE_STA 3

class WifiHandler
{
  private:
    PersistentStorage* _pConfiguration;
    int _wifiState; 
    int _lastWifiState = -1;

  
  public:

    void LoadConfiguration(PersistentStorage* pPersistentStorage)
    {
      _pConfiguration = pPersistentStorage;
      _wifiState = STATE_STARTING_STA;
    }

    void setParamsForDebug(String ssid, String password, String hostname)
    {
      _pConfiguration->SsidSet(ssid);
      _pConfiguration->PasswordSet(password);
      _pConfiguration->HostNameSet(hostname);
      _wifiState = STATE_STARTING_STA;
    }

    void setWirelessParametersAndRestart(String ssid, String password)
    {
      Serial.println("setWirelessParametersAndRestart");
      
      _pConfiguration->SsidSet(ssid);
      _pConfiguration->PasswordSet(password);
      _wifiState = STATE_STARTING_STA;
    }

    void PrintStatus()
    {
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
    }

    void startAsClient()
    {
      Serial.println("SAC: Disconnecting, waiting to finish");
      //WiFi.mode(WIFI_OFF);
      WiFi.disconnect();
      while (WiFi.status() != WL_DISCONNECTED && WiFi.status() != WL_IDLE_STATUS)
      {
        Serial.println(WiFi.status());
        delay(50);
      }

      Serial.println("SAC: starting STA");
      Serial.print("SAC: Connecting to: ");
      Serial.println(_pConfiguration->_ssid);
      Serial.println(_pConfiguration->_password);
      Serial.print(WiFi.status());
      
      WiFi.mode(WIFI_STA);
      while (WiFi.getMode() != WIFI_STA)
      {
        Serial.print("M");
        delay(50);
      }
      WiFi.disconnect();
      Serial.println(_pConfiguration->HostNameGet());
      WiFi.hostname(_pConfiguration->HostNameGet());
      WiFi.begin(_pConfiguration->_ssid, _pConfiguration->_password);

      int timeout = 30;
      while (WiFi.status() != WL_CONNECTED && timeout > 0)
      {
        Serial.print(".");
        delay(500);
 
        //PrintStatus();
        //Serial.print(WiFi.status());
        timeout--;
      }
      Serial.println();

      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.print("SAC: Connected: ");
        Serial.println(WiFi.localIP());

        _wifiState = STATE_STA;
        _pConfiguration->Save();
      }
      else
      {
        Serial.print("SAC: Connect failed, status = ");
        Serial.println(WiFi.status());
        _wifiState = STATE_UNKNOWN;
      }
    }

    void startAsAccessPoint() {
      generateRandomName();
      
      Serial.print("SAAP: Setting soft-AP:  ");
      Serial.println(_pConfiguration->_hostName);
      WiFi.mode(WIFI_AP);
      boolean result = WiFi.softAP(_pConfiguration->_hostName, _pConfiguration->_hostName);
      if(result == true)
      {
        Serial.println("SAAP: Ready");
        IPAddress myIP = WiFi.softAPIP();
        Serial.print("SAAP: AP IP address: ");
        Serial.println(myIP);
        Serial.println("SAAP: HTTP server started");
        _wifiState = STATE_AP;
      }
      else
      {
        Serial.println("SAAP: Failed!");
      }
    }  

    void generateRandomName()
    {
      Serial.println("generateRandonName");
       char buf[16];
      ltoa(ESP8266TrueRandom.random(), buf, 10);
      String hostName = String("EDP_") + buf;
      
      _pConfiguration->HostNameSet(hostName);
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
      if (_wifiState == STATE_STARTING_STA && _pConfiguration->SsidGet().length() != 0)
      {
        startAsClient(); 
      }

      if (_wifiState == STATE_UNKNOWN)
      {
        startAsAccessPoint();        
      }
    }    
};

