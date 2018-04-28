#include <ESPAsyncWebServer.h>


class WebServer
{
  private:
    static WebServer *_pWebServer;
  
    AsyncWebServer* _pServer;
    WifiHandler* _pWifiHandler;

    static void OnNotFound(AsyncWebServerRequest* pRequest)
    {
      Serial.println("not found");
      Serial.println(pRequest->url().c_str());

      pRequest->send(200, "text/plain", "<h1>Hello</h1>");
    }

    static void OnInfo(AsyncWebServerRequest* pRequest)
    {
      pRequest->send(200, "text/plain", "EagleDecorations ESPClient V1.0");
    }

    static void OnProvision(AsyncWebServerRequest* pRequest) {
      if (pRequest->hasArg("ssid"))
      {
        String ssid = pRequest->arg("ssid");
        String password = pRequest->arg("password");

        Serial.print("ssid: "); Serial.println(ssid);
        Serial.print("password: "); Serial.println(password);
        _pWebServer->_pWifiHandler->setWirelessParametersAndRestart(ssid, password);
      }      

      pRequest->send(200, "text/plain", "Starting provisioning");      
    }

  public:
    void SetWifiHandler(WifiHandler* pWifiHandler)
    {
      _pWifiHandler = pWifiHandler;  
    }
    
    void Init()
    {
      _pWebServer = this;
      
      _pServer = new AsyncWebServer(80);

      _pServer->onNotFound(OnNotFound);
      _pServer->on("/info", OnInfo);
      _pServer->on("/provision", OnProvision);

      _pServer->begin();
    }
  
};

WebServer* WebServer::_pWebServer = 0;

