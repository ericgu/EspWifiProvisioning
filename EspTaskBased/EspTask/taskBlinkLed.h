#define colorSaturation 127

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

class TaskBlinkLed : public Task
{
public:
    TaskBlinkLed(uint8_t pin, uint32_t timeInterval, NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>& strip) : // pass any custom arguments you need
        Task(timeInterval),
        ledPin(pin),
        ledOn(false),
        _strip(strip),
        _pClient(0),
        _messageCount(0)
        // initialize members here
    { 
    };

    void SetClient(WiFiClient* pClient)
    {
      _pClient = pClient;
    }

private:
    // put member variables here that are scoped to this object
    bool ledOn;
    const uint8_t ledPin; // const means can't change other than in constructor
    NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>& _strip;
    WiFiClient* _pClient;
    int _messageCount;
    uint8_t _buffer[1024];

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

    void ProcessMessage(char* pMessage)
    {
      int red;
      int green;
      int blue;

      // rgb rrr,ggg,bbb
      if (strcmp(pMessage, "rgb") == 0)
      {
        pMessage += 4;
        red = atoi(pMessage);  
        
        pMessage += 4;
        green = atoi(pMessage); 
         
        pMessage += 4;
        blue = atoi(pMessage);  
      }

      Serial.println("values ");
      Serial.println(red);
      Serial.println(green);
      Serial.println(blue);
    }

    virtual void OnUpdate(uint32_t deltaTime)
    {
        if (_pClient)
        {
          int bytesAvailable = _pClient->available();
          if (bytesAvailable > 0)
          {
            _pClient->read(_buffer, bytesAvailable);
            _buffer[bytesAvailable] = 0;
            _pClient->write("SetRGB");
            if (_messageCount % 100 == 0)
            {
              Serial.println((char*) _buffer);
            }
            _messageCount++;
          }
        }
      
        if (ledOn)
        {
            digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
            _strip.SetPixelColor(2, red);
            _strip.SetPixelColor(3, green);
            _strip.SetPixelColor(0, blue);
            _strip.SetPixelColor(1, white);
            _strip.RotateRight(4);
            _strip.Show();            
        }
        else
        {
            digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
            _strip.SetPixelColor(0, red);
            _strip.SetPixelColor(1, green);
            _strip.SetPixelColor(2, blue);
            _strip.SetPixelColor(3, white);
            _strip.RotateRight(4);
            _strip.Show();            
        }

        ledOn = !ledOn; // toggle led state
    }
};
