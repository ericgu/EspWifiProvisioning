class IAnimation
{
  protected:
    NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* _pStrip;
    int _pixelCount;
    String _name;
    String _lastMessage;
  
  public: 
    IAnimation(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* pStrip, int pixelCount, String name)
    {
      _pStrip = pStrip;
      _pixelCount = pixelCount;
      _name = name;
    }

    String getName() { return _name; }
    String getLastMessage() { return _lastMessage; }

    virtual bool ProcessMessage(const char* pMessage) = 0;
    virtual void Update() = 0;
};

