
class AnimationFlashDecay: public IAnimation
{
  private:
    float _hue;
    RgbColor _baseColor;
    int _stepsOn;
    int _stepsWait;
    int _currentStep = 0;

    float _saturation = 1.0;
    float _brightness = 0.25;
  
  public: 

    AnimationFlashDecay(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* pString, int pixelCount) : 
        IAnimation(pString, pixelCount, "Flash decay")
        {}

    bool ProcessMessage(const char* pMessage)
    {
      // fdc cccc
      if (*pMessage == 'f')
      {
        _lastMessage = pMessage;

        pMessage += 4;
        _stepsOn = atoi(pMessage);  

        pMessage += 4;
        _stepsWait = atoi(pMessage);  

        return true;
      }

      return false;
    }

    void SetHue()
    {
      _hue = ((float)ESP8266TrueRandom.random()) / 2147482647;
    }
      
    HsbColor GetColor()
    {
      if (_currentStep == 0)
      {
        SetHue();
      }
      
      _currentStep = (_currentStep + 1) % (_stepsOn + _stepsWait);

      if (_currentStep < _stepsOn)
      {
        float brightness = 1.0 - (1.0 * _currentStep) / _stepsOn;

        return HsbColor(_hue, 1.0, brightness);
      }
      else
      {
        return HsbColor(0, 0, 0);
      }
    }

    void Update()
    {
      HsbColor _setColor = GetColor();

      for (int led = 0; led < _pixelCount; led++)
      {
        _pStrip->SetPixelColor(led, _setColor);
      }
      _pStrip->Show(); 
    }
};

