
class AnimationBlendTo: public IAnimation
{
  private:
    RgbColor _baseColor;
    RgbColor _targetColor;
    int _steps;
    int _currentStep = 0;

    float _deltaRed;
    float _deltaGreen;
    float _deltaBlue;
  
  public: 

    AnimationBlendTo(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>** ppStrip) : 
        IAnimation(ppStrip, "Blend")
        {}

    bool ProcessMessage(const char* pMessage, ParseNumbers* pParseNumbers)
    {
      // rgb rrr,ggg,bbb,cccc
      if (*pMessage == 'r' && pParseNumbers->_count == 4)
      {
        RgbColor color = RgbColor(pParseNumbers->_values[0], pParseNumbers->_values[1], pParseNumbers->_values[2]);
        int count = pParseNumbers->_values[3];

        DoBlend(color, count);
        
        return true;
      }

      return false;
    }
      
    void DoBlend(RgbColor targetColor, int steps)
    {
      _baseColor = _targetColor; // the old target color...
      _targetColor = targetColor;
      _steps = steps;
      _currentStep = 0;
    }
  
    RgbColor GetColor()
    {
      if (_currentStep < _steps)
      {
        _currentStep++;
      }

      float factor = _currentStep / (float) _steps;
      int red = _baseColor.R + (int) ((_targetColor.R - _baseColor.R) * factor);
      int green = _baseColor.G + (int) ((_targetColor.G - _baseColor.G) * factor);
      int blue = _baseColor.B + (int) ((_targetColor.B - _baseColor.B) * factor);

      return RgbColor(red, green, blue);
    }

    void Update()
    {
      RgbColor color = GetColor();

      SetAllPixelColorWithGammaAndShow(color);
    }
};

