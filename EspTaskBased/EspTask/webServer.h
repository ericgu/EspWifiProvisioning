#include <ESPAsyncWebServer.h>

class WebServer
{
  private:
    static WebServer *_pWebServer;
  
    AsyncWebServer* _pServer;
    WifiHandler* _pWifiHandler;
    PixelHandler* _pPixelHandler;

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

    static void OnMessage(AsyncWebServerRequest* pRequest) {
      if (pRequest->hasArg("content"))
      {
        String content = pRequest->arg("content");

        Serial.print("content: "); Serial.println(content);

        _pWebServer->_pPixelHandler->ProcessMessage(content.c_str());
      }      

      pRequest->send(200, "text/plain", "Accepted");      
    }

  public:
    void SetHandlers(WifiHandler* pWifiHandler, PixelHandler* pPixelHandler)
    {
      _pWifiHandler = pWifiHandler;  
      _pPixelHandler = pPixelHandler;
    }
    
    void Init()
    {
      _pWebServer = this;
      
      _pServer = new AsyncWebServer(80);

      _pServer->onNotFound(OnNotFound);
      _pServer->on("/info", OnInfo);
      _pServer->on("/provision", OnProvision);
      _pServer->on("/message", OnMessage);

      _pServer->begin();
    }
  
};

WebServer* WebServer::_pWebServer = 0;

