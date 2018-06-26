class TaskProcessMessages : public Task
{
public:
    TaskProcessMessages(uint8_t ledPin, uint32_t timeInterval, PixelHandler* pPixelHandler, WifiHandler* pWifiHandler) : // pass any custom arguments you need
        Task(timeInterval),
        _ledPin(ledPin),
        _ledOn(false),
        _pWifiHandler(pWifiHandler),
        _pPixelHandler(pPixelHandler)
    { 
      _ledUpdateCountInitialValue = 500 / _timeInterval;
      _ledUpdateCount = _ledUpdateCountInitialValue;
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
    bool _ledOn;
    const uint8_t _ledPin; // const means can't change other than in constructor
    PixelHandler* _pPixelHandler;
    char _buffer[1024];
    int _packetCount= 0;
    int _ledUpdateCountInitialValue;
    int _ledUpdateCount;
    int _updatesWithoutPackets = 0;
    int _updateCount = 0;

    virtual bool OnStart() // optional
    {
        // put code here that will be run when the task starts
        _ledOn = false;
        pinMode(_ledPin, OUTPUT);
        return true;
    }

    virtual void OnStop() // optional
    {
        // put code here that will be run when the task stops
        _ledOn = false;
        digitalWrite(_ledPin, LOW);    // turn the LED off by making the voltage LOW
    }

    void UpdateHandleUdp()
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

        _pPixelHandler->ProcessMessage(_buffer, false);
      }
      else
      {
        _updatesWithoutPackets++;
      }
    }

    void UpdateHandleLed()
    {
      _ledUpdateCount--;
      if (_ledUpdateCount == 0)
      {
        _ledUpdateCount = _ledUpdateCountInitialValue;

        digitalWrite(_ledPin, _ledOn ? LOW : HIGH);
        _ledOn = !_ledOn; // toggle led state
      }

      _updateCount++;
      if (_updateCount % 1000 == 0)
      {
        //Serial.print("Updates without packets: ");
        //Serial.println(_updatesWithoutPackets);
      }
    }

    void UpdatePixelHandler()
    {
      _pPixelHandler->Update();
    }

    static void WifiStatusHandler(int status, void* userData)
    {
      TaskProcessMessages* pTaskProcessMessages = (TaskProcessMessages*) userData;

      if (status == STATE_AP)
      {
        pTaskProcessMessages->_pPixelHandler->SetAccessPointAnimation();
      }
      else if (status == STATE_STA)
      {
        pTaskProcessMessages->_pPixelHandler->SetProvisionedAnimation();
      }
    }

    virtual void OnUpdate(uint32_t deltaTime)
    {
      UpdateHandleUdp();
      
      UpdateHandleLed();

      UpdatePixelHandler();
 
      _pWifiHandler->Init(WifiStatusHandler, this);
    }     
};
