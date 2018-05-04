
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
      // crt cccc,bbbb
      if (*pMessage == 'c' && pParseNumbers->_count == 2)
      {
        _steps = pParseNumbers->_values[0];
        _brightness = ((float)pParseNumbers->_values[1]) / 255;  

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
      RgbColor color = GetColor();

      SetAllPixelColorWithGammaAndShow(color);
    }
};

