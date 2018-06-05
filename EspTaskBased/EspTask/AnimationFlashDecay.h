
class AnimationFlashDecay: public IAnimation
{
  private:
    float _hue;
    RgbColor _baseColor;
    int _stepsOn;
    int _stepsWaitMin;
    int _stepsWaitMax;
    int _stepsWait;
    int _currentStep = 0;

    float _saturation = 1.0;
    float _brightness = 0.25;
  
  public: 

    AnimationFlashDecay(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* pString, int pixelCount) : 
        IAnimation(pString, pixelCount, "Flash decay")
        {}

    bool ProcessMessage(const char* pMessage, ParseNumbers* pParseNumbers)
    {
      // fdc cccc
      if (*pMessage == 'f' && pParseNumbers->_count == 3)
      {
        _stepsOn = pParseNumbers->_values[0];
        _stepsWaitMin = pParseNumbers->_values[1];
        _stepsWaitMax = pParseNumbers->_values[2];

        return true;
      }

      return false;
    }

    void Next()
    {
      _hue = ((float)ESP8266TrueRandom.random()) / 2147482647;
      _stepsWait = _stepsWaitMin + ESP8266TrueRandom.random(_stepsWaitMax - _stepsWaitMin);
      //Serial.println(_hue);
      //Serial.println(_stepsWait);
    }
      
    HsbColor GetColor()
    {
      if (_currentStep == 0)
      {
        Next();
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
      HsbColor color = GetColor();

      SetAllPixelColorWithGammaAndShow(color);
    }
};

