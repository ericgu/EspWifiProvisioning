#include <ESPAsyncWebServer.h>
#include "MainPage.h"
#include "ColorTable.h"

class WebServer
{
  private:
    static WebServer *_pWebServer;
  
    AsyncWebServer* _pServer;
    WifiHandler* _pWifiHandler;
    PixelHandler* _pPixelHandler;
    bool _singleMode;
    String _colorTableHtml;

    static constexpr char* _codeVersionNumber = "V1.0";

    static void OnNotFound(AsyncWebServerRequest* pRequest)
    {
      Serial.println("not found");
      Serial.println(pRequest->url().c_str());

      //pRequest->send(200, "text/html", "<html><body><html>Not found</html></body></html>");
       pRequest->redirect("http://192.168.4.1/Provision");
    }

    static void OnInfo(AsyncWebServerRequest* pRequest)
    {
      String info = "EagleDecorations ESPClient";
      info += _codeVersionNumber;
      info += " ";
      info +=  _pWebServer->_pWifiHandler->HostNameGet();
      pRequest->send(200, "text/plain", info.c_str());
    }

    static void OnToggleMode(AsyncWebServerRequest* pRequest)
    {
      _pWebServer->_singleMode = _pWebServer->_singleMode == 0 ? 1 : 0;

      if (pRequest->hasArg("r"))
      {
        pRequest->redirect("/Color");
      }
      else
      {
        pRequest->redirect("/");
      }
    }

    static void OnMainPage(AsyncWebServerRequest* pRequest)
    {
      Serial.println("> MainPage");

      String response = MainPageHtml1;

      response += "<p>";
        
      if (_pWebServer->_singleMode)
      {
        response += "<b>Mode: Single Mode</b>";
      }
      else
      {
        response += "<b>Mode: Recording Mode </b>";
      }
      response += "&nbsp;&nbsp;<a href=\"/ToggleMode\">Toggle</a></p>";
      response += MainPageHtml2;

      pRequest->send(200, "text/html", response);

      Serial.println("< MainPage");
    }
    
    static void OnProvision(AsyncWebServerRequest* pRequest)
    {
      Serial.println("> Provision");

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
        String response = ProvisionPageHtml1;

        response += "<b>Status: </b>";
        response += _pWebServer->_pWifiHandler->GetStatus();
        response += "</br>";
        
        response += "<b>LEDs: </b>";
        response += _pWebServer->_pPixelHandler->GetPixelCount();
        response += "</br>";
        
        response += ProvisionPageHtml2;

        Networks networks = _pWebServer->_pWifiHandler->getNetworks();

        for (int i = 0; i < networks.count; i += 2)
        {
          String option = "<option value=\"" + networks.pValues[i] + "\">" + networks.pValues[i + 1] + "</option>";
          response += option;
        }

        response += ProvisionPageHtml3;
        pRequest->send(200, "text/html", response);
      }

      Serial.println("< Provision");
    }

    static void OnColorPage(AsyncWebServerRequest* pRequest)
    {
      Serial.println("> ColorPage");

      String response = ColorPageHtml1;

      response += "<p>";
        
      if (_pWebServer->_singleMode)
      {
        response += "<b>Mode: Single Mode</b>";
      }
      else
      {
        response += "<b>Mode: Recording Mode </b>";
      }

      response += "&nbsp;&nbsp;<a href=\"/ToggleMode?r=1\">Toggle</a></p>";

      response += _pWebServer->_colorTableHtml;

      response += ColorPageHtml2;
      pRequest->send(200, "text/html", response);

      Serial.println("< ColorPage");
    }

#if fred
    static String GetHexDigit(int digit)
    {
      static char digitChars[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

      return String(digitChars[digit]);
    }

    static String GetHexValue(int number)
    {
      return GetHexDigit(number / 16) + GetHexDigit(number % 16);
    }

    static String GetIntValue(int number)
    {
      char buffer[16];
      itoa(number, buffer, 10);
      return buffer;      
    }

    static String GetTableCell(int red, int green, int blue)
    {
      String cell = "<td bgColor=\"#";
      cell += GetHexValue(red) + GetHexValue(green) + GetHexValue(blue);
      cell += "\"><a style=\"text-decoration: none;\" href=\"/message?c=1&content=rgbx";

      cell += GetIntValue(red);
      cell += ",";
      cell += GetIntValue(green);
      cell += ",";
      cell += GetIntValue(blue);

      cell += ",100";
      if (!_pWebServer->_singleMode)
      {
        cell += "&period=500";
      }
      cell += "\">&nbsp;&nbsp;&nbsp;&nbsp;</a></td>";

      return cell;
    }

    static String GetTableRow(int intensity)
    {
      String row;

      row += GetTableCell(intensity, intensity, intensity);
      row += GetTableCell(0, 0, 0);
      row += GetTableCell(intensity, 0, 0);
      row += GetTableCell(0, intensity, 0);
      row += GetTableCell(0, 0, intensity);
      row += GetTableCell(intensity, intensity, 0);
      row += GetTableCell(intensity, 0, intensity);
      row += GetTableCell(0, intensity, intensity);
      
      return row;
    }

    static String GetTable()
    {
      String response;
      
      response += "<table>";

      for (int intensity = 255; intensity > 100; intensity -= 50)
      {
        response += "<tr>";

        response += GetTableRow(intensity);

        response += "</tr>";
      }
            
      response += "</table>";

      return response;
    }
#endif

    static void OnMessage(AsyncWebServerRequest* pRequest)
    {
      if (pRequest->hasArg("r"))
      {
        pRequest->redirect("/");
      }
      if (pRequest->hasArg("c"))
      {
        pRequest->redirect("/Color");
      }      
      else if (pRequest->hasArg("ledcount"))
      {
        pRequest->redirect("/Provision");
      }
      else
      {
        pRequest->send(200, "text/plain", "Accepted");      
      }

      if (pRequest->hasArg("content"))
      {
        String content = pRequest->arg("content");

        if (pRequest->hasArg("ledcount"))
        {
          content = "nx" + content; // prepend command...
        }

        Serial.print("content: "); Serial.println(content);

        int period = -1;
        if (pRequest->hasArg("period"))
        {
          period = atoi(pRequest->arg("period").c_str());
        }

        _pWebServer->_pPixelHandler->ProcessMessage(content.c_str(), period);
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
      _singleMode = true;
      _colorTableHtml = ColorTable::GetTable(_pWebServer->_singleMode);
      
      _pServer = new AsyncWebServer(80);

      _pServer->onNotFound(OnNotFound);
      _pServer->on("/", OnMainPage);
      _pServer->on("/Provision", OnProvision);
      _pServer->on("/ToggleMode", OnToggleMode);
      _pServer->on("/Color", OnColorPage);
      _pServer->on("/info", OnInfo);
      _pServer->on("/message", OnMessage);

      _pServer->begin();
    }
  
};

WebServer* WebServer::_pWebServer = 0;

