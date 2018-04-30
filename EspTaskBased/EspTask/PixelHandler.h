
class PixelHandler
{
  private:
    NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* _pStrip;
    int _pixelCount;
    int _pixelPin;

    const int AnimationCount = 3;
    IAnimation** _pAnimations;
    IAnimation* _pCurrentAnimation;
   
  public:
    PixelHandler(int pixelCount, int pixelPin) : 
        _pixelCount(pixelCount),
        _pixelPin(pixelPin)
    { 
    };

    void Init()
    {
      _pStrip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(_pixelCount, _pixelPin);
      _pStrip->Begin();

      _pAnimations = new IAnimation*[AnimationCount];
      _pAnimations[0] = new AnimationBlendTo(_pStrip, _pixelCount);
      _pAnimations[1] = new AnimationAlternate(_pStrip, _pixelCount);
      _pAnimations[2] = new AnimationIndividual(_pStrip, _pixelCount);

      _pAnimations[1]->ProcessMessage("alt 032,000,000,000,000,000,250");
      _pCurrentAnimation = _pAnimations[1];

      _pCurrentAnimation->Update();
    }

    int ProcessMessage(const char* pMessage)
    {
      for (int i = 0; i < AnimationCount; i++)
      {
        if (_pAnimations[i]->ProcessMessage(pMessage))
        {
          _pCurrentAnimation = _pAnimations[i];
          return 0;
        }
      }

      Serial.println(pMessage);
      
      return 0;

    }

    void Update()
    {
      _pCurrentAnimation->Update();
    }
};
