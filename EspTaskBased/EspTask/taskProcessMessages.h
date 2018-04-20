#define colorSaturation 127

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

class TaskBlinkLed : public Task
{
public:
    TaskBlinkLed(uint8_t pin, uint32_t timeInterval, NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>& strip, WiFiUDP* pUdp) : // pass any custom arguments you need
        Task(timeInterval),
        ledPin(pin),
        ledOn(false),
        _strip(strip),
        _pUdp(pUdp),
        _messageCount(0)
        // initialize members here
    { 
      _setColor = RgbColor(127, 0, 0);
      _ledUpdateCountInitialValue = 500 / _timeInterval;
      _ledUpdateCount = _ledUpdateCountInitialValue;
    };

private:
    // put member variables here that are scoped to this object
    bool ledOn;
    const uint8_t ledPin; // const means can't change other than in constructor
    NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>& _strip;
    WiFiUDP* _pUdp;
    int _messageCount;
    uint8_t _buffer[1024];
    RgbColor _setColor;
    int _packetCount= 0;
    int _ledUpdateCountInitialValue;
    int _ledUpdateCount;

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

        _setColor = RgbColor(red, green, blue);

        for (int led = 0; led < 33; led++)
        {
          _strip.SetPixelColor(led, _setColor);
        }
        _strip.Show();          

        return serial;
      }
      else (*pMessage == 'a')
      {
        _strip.Show();          
        

        return 0;
      }
      
    }

    virtual void OnUpdate(uint32_t deltaTime)
    {
      char incomingPacket[255];  // buffer for incoming packets

      int packetSize = _pUdp->parsePacket();
      if (packetSize)
      {
        // receive incoming UDP packets
        //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, _pUdp->remoteIP().toString().c_str(), _pUdp->remotePort());
        int len = _pUdp->read(incomingPacket, 255);
        if (len > 0)
        {
          incomingPacket[len] = 0;
        }

        if (_packetCount % 1000 == 0)
        {
          Serial.print(_packetCount);
          Serial.println(" ");
          //Serial.println(missedMessages);
        }
        _packetCount++;

        int serial = ProcessMessage(incomingPacket);


        if (_messageCount % 100 == 0)
        {
          //Serial.println(_messageCount);
        }
        
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
    }     
    
#ifdef fred
        if (_pClient)
        {
          int bytesAvailable = _pClient->available();
          if (bytesAvailable > 0)
          {
            _pClient->read(_buffer, bytesAvailable < sizeof(_buffer) ? bytesAvailable : sizeof(_buffer));
            _buffer[bytesAvailable] = 0;
            _messageCount++;
            int serial = ProcessMessage((char*) _buffer);

            char buffer2[16];
            itoa(serial, buffer2, 10);

            char buffer[16];
            buffer[0] = 0;
            strcpy(buffer, "RGB");
            strcat(buffer, buffer2);
            _pClient->write(&buffer[0], sizeof(buffer));
            //Serial.println(buffer);

            if (serial % 1000 == 0)
            {
              Serial.println(system_get_free_heap_size());
              Serial.println(_pClient->status());
            }
          }
          else
          {
            Serial.println(_pClient->status());
          }
        }

        _strip.SetPixelColor(0, _setColor);
        _strip.SetPixelColor(1, _setColor);
        _strip.SetPixelColor(2, _setColor);
        _strip.RotateRight(3);
        _strip.Show();  

        if (_messageCount % 100 == 0)
        {
          //Serial.println(_messageCount);
        }
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
#endif
};
