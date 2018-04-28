#include "pixelMessageHandler.h"

class TaskProcessMessages : public Task
{
public:
    TaskProcessMessages(uint8_t pin, uint32_t timeInterval, NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* pStrip, WifiHandler* pWifiHandler) : // pass any custom arguments you need
        Task(timeInterval),
        ledPin(pin),
        ledOn(false),
        _pStrip(pStrip),
        _pWifiHandler(pWifiHandler)
    { 
      _setColor = RgbColor(127, 0, 0);
      _ledUpdateCountInitialValue = 500 / _timeInterval;
      _ledUpdateCount = _ledUpdateCountInitialValue;
      _pPixelMessageHandler = new PixelMessageHandler(pStrip);
    };

    void Init()
    {
      _udp.begin(localUdpPort);
      Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
    }

private:
    const unsigned int localUdpPort = 4210;  // local port to listen on

    WifiHandler* _pWifiHandler;
    WiFiUDP _udp;
    bool ledOn;
    const uint8_t ledPin; // const means can't change other than in constructor
    NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* _pStrip;
    PixelMessageHandler* _pPixelMessageHandler;
    //WiFiUDP* _pUdp;
    char _buffer[1024];
    RgbColor _setColor;
    int _packetCount= 0;
    int _ledUpdateCountInitialValue;
    int _ledUpdateCount;
    int _updatesWithoutPackets = 0;
    int _updateCount = 0;

    virtual bool OnStart() // optional
    {
        // put code here that will be run when the task starts
        ledOn = false;
        pinMode(ledPin, OUTPUT);
        return true;
    }

    virtual void OnStop() // optional
    {
        // put code here that will be run when the task stops
        ledOn = false;
        digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
    }

    virtual void OnUpdate(uint32_t deltaTime)
    {
      int packetSize = _udp.parsePacket();
      if (packetSize)
      {
        // receive incoming UDP packets
        //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, _udp.remoteIP().toString().c_str(), _udp.remotePort());
        int len = _udp.read(_buffer, sizeof(_buffer) - 1);
        if (len > 0)
        {
          _buffer[len] = 0;
        }

        if (_packetCount % 1000 == 0)
        {
          Serial.print(_packetCount);
          Serial.print(" ");
          Serial.println(system_get_free_heap_size());
          //Serial.println(missedMessages);
        }
        _packetCount++;

        int serial = _pPixelMessageHandler->ProcessMessage(_buffer);
      }
      else
      {
        _updatesWithoutPackets++;
      }

      _ledUpdateCount--;
      if (_ledUpdateCount == 0)
      {
        _ledUpdateCount = _ledUpdateCountInitialValue;
        
        if (ledOn)
        {
            digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
        }
        else
        {
            digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
        }

        ledOn = !ledOn; // toggle led state
      }

      _updateCount++;
      if (_updateCount % 1000 == 0)
      {
        //Serial.print("Updates without packets: ");
        //Serial.println(_updatesWithoutPackets);
      }

      _pWifiHandler->Init();
    }     
};
