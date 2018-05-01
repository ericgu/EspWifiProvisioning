
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

    AnimationBlendTo(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* pString, int pixelCount) : 
        IAnimation(pString, pixelCount, "Blend")
        {}

    bool ProcessMessage(const char* pMessage)
    {
      int red = 0;
      int green = 0;
      int blue = 0;
      int count = 1;

      // rgb rrr,ggg,bbb,cccc
      if (*pMessage == 'r')
      {
        _lastMessage = pMessage;
        
        pMessage += 4;
        red = atoi(pMessage);  
        
        pMessage += 4;
        green = atoi(pMessage); 
         
        pMessage += 4;
        blue = atoi(pMessage);         
        
        pMessage += 3;
        if (*pMessage != 0)
        {
          pMessage++;
          count = atoi(pMessage);
        }

        DoBlend(RgbColor(red, green, blue), count);
        
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
      RgbColor _setColor = GetColor();

      for (int led = 0; led < _pixelCount; led++)
      {
        _pStrip->SetPixelColor(led, _setColor);
      }
      _pStrip->Show(); 
    }
};

