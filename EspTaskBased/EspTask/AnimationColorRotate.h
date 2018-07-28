
class AnimationColorRotate: public IAnimation
{
  private:
    int _steps;
    int _currentStep = 0;

    float _saturation = 1.0;
    float _brightness = 0.25;
  
  public: 

    AnimationColorRotate(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>** ppStrip) : 
        IAnimation(ppStrip, "Color rotate")
        {}

    bool ProcessMessage(const char* pMessage, ParseNumbers* pParseNumbers)
    {
      // col cccc,bbbb
      if (IsMatch(pMessage, pParseNumbers, "col", 2))
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
      //Serial.print(color.R);
      //Serial.print(" ");
      //Serial.print(color.G);
      //Serial.print(" ");
      //Serial.println(color.B);

      SetAllPixelColorWithGammaAndShow(color);
    }
};

