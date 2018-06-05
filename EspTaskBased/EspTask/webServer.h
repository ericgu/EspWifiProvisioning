#include <ESPAsyncWebServer.h>
#include "MainPage.h"

class WebServer
{
  private:
    static WebServer *_pWebServer;
  
    AsyncWebServer* _pServer;
    WifiHandler* _pWifiHandler;
    PixelHandler* _pPixelHandler;

    static constexpr char* _codeVersionNumber = "V1.0";

    static void OnNotFound(AsyncWebServerRequest* pRequest)
    {
      Serial.println("not found");
      Serial.println(pRequest->url().c_str());

      pRequest->send(200, "text/plain", "<h1>Hello</h1>");
    }

    static void OnInfo(AsyncWebServerRequest* pRequest)
    {
      String info = "EagleDecorations ESPClient";
      info += _codeVersionNumber;
      info += " ";
      info +=  _pWebServer->_pWifiHandler->HostNameGet();
      pRequest->send(200, "text/plain", info.c_str());
    }

    static void OnMainPage(AsyncWebServerRequest* pRequest) {
      Serial.println("> MainPage");

      if (pRequest->hasArg("ssid"))
      {
        String ssid = pRequest->arg("ssid");
        String password = pRequest->arg("password");

        Serial.print("  ssid: "); Serial.println(ssid);
        Serial.print("  password: "); Serial.println(password);
        _pWebServer->_pWifiHandler->setWirelessParametersAndRestart(ssid, password, true);

        pRequest->send(200, "text/html", "<h1>Provisioning...</h1><html>Disconnect from this wireless network and reconnect to your main network.</html>");
      }      
      else
      {
        String response = MainPageStartHtml;

        Networks networks = _pWebServer->_pWifiHandler->getNetworks();

        for (int i = 0; i < networks.count; i += 2)
        {
          String option = "<option value=\"" + networks.pValues[i] + "\">" + networks.pValues[i + 1] + "</option>";
          Serial.println(option);
          response += option;
        }

        response += MainPageEndHtml;
        pRequest->send(200, "text/html", response);
      }

      Serial.println("< MainPage");
    }

    static void OnProvision(AsyncWebServerRequest* pRequest) {
      Serial.println("> Provision");
      if (pRequest->hasArg("ssid"))
      {
        String ssid = pRequest->arg("ssid");
        String password = pRequest->arg("password");
        bool provisionOtherNodes = !pRequest->hasArg("SkipOtherNodes");

        Serial.print("  ssid: "); Serial.println(ssid);
        Serial.print("  password: "); Serial.println(password);
        Serial.print("  provisionOtherNodes: "); Serial.println(provisionOtherNodes);
        _pWebServer->_pWifiHandler->setWirelessParametersAndRestart(ssid, password, provisionOtherNodes);
      }      

      pRequest->send(200, "text/plain", "Starting provisioning");    
      Serial.println("< Provision");
    }
  
    static void OnMessage(AsyncWebServerRequest* pRequest) {
      if (pRequest->hasArg("content"))
      {
        String content = pRequest->arg("content");

        Serial.print("content: "); Serial.println(content);

        _pWebServer->_pPixelHandler->ProcessMessage(content.c_str());
      }      

      if (pRequest->hasArg("r"))
      {
        pRequest->redirect("/");
      }
      else
      {
        pRequest->send(200, "text/plain", "Accepted");      
      }
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
      _pServer->on("/", OnMainPage);
      _pServer->on("/info", OnInfo);
      _pServer->on("/provision", OnProvision);
      _pServer->on("/message", OnMessage);

      _pServer->begin();
    }
  
};

WebServer* WebServer::_pWebServer = 0;

