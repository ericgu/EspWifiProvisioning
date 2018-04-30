class IAnimation
{
  protected:
    NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* _pStrip;
    int _pixelCount;
  
  public: 
    IAnimation(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* pStrip, int pixelCount)
    {
      _pStrip = pStrip;
      _pixelCount = pixelCount;
    }

    virtual bool ProcessMessage(const char* pMessage) = 0;
    virtual void Update() = 0;
};

