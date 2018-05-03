
class AnimationColorRotate: public IAnimation
{
  private:
    int _steps;
    int _currentStep = 0;

    float _saturation = 1.0;
    float _brightness = 0.25;
  
  public: 

    AnimationColorRotate(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* pString, int pixelCount) : 
        IAnimation(pString, pixelCount, "Color rotate")
        {}

    bool ProcessMessage(const char* pMessage, ParseNumbers* pParseNumbers)
    {
      // crt cccc
      if (*pMessage == 'c')
      {
        _lastMessage = pMessage;
        if (pParseNumbers->_count < 1)
        {
          return false;
        }
        
        _steps = pParseNumbers->_values[0];  

        return true;
      }

      return false;
    }
      
    RgbColor GetColor()
    {
      _currentStep = (_currentStep + 1) % _steps;

      float hue = (1.0 * _currentStep) / _steps;

      return RgbColor(HsbColor(hue, _saturation, _brightness));
    }

    void Update()
    {
      RgbColor _setColor = GetColor();

      for (int led = 0; led < _pixelCount; led++)
      {
        SetPixelColorWithGamma(led, _setColor);
      }
      _pStrip->Show(); 
    }
};

