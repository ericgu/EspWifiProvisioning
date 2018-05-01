
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

    bool ProcessMessage(const char* pMessage)
    {
      // crt cccc
      if (*pMessage == 'c')
      {
        _lastMessage = pMessage;
        
        pMessage += 4;
        _steps = atoi(pMessage);  

        return true;
      }

      return false;
    }
      
    HsbColor GetColor()
    {
      _currentStep = (_currentStep + 1) % _steps;

      float hue = (1.0 * _currentStep) / _steps;

      return HsbColor(hue, _saturation, _brightness);
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

