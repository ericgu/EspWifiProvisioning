class AnimationAlternate: public IAnimation
{
  private:
    RgbColor _firstColor;
    RgbColor _secondColor;
    int _count;
    int _currentCount = 0;
 
  public: 

    AnimationAlternate(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* pString, int pixelCount) : 
        IAnimation(pString, pixelCount, "Alternate")
        {}

    // alt rrr,ggg,bbb,rrr,ggg,bbb,cccc
    //      (first)       (second)  (count)
    bool ProcessMessage(const char* pMessage, ParseNumbers* pParseNumbers)
    {
      int red;
      int green;
      int blue;

      if (*pMessage == 'a')
      {
        _lastMessage = pMessage;

        pParseNumbers->Dump();
        if (pParseNumbers->_count < 7)
        {
          return false;
        }
        
        _firstColor = RgbColor(pParseNumbers->_values[0], pParseNumbers->_values[1], pParseNumbers->_values[2]);
        _secondColor = RgbColor(pParseNumbers->_values[3], pParseNumbers->_values[4], pParseNumbers->_values[5]);
        _count = pParseNumbers->_values[6];
        _currentCount = 0;

        return true;
      }

      return false;
    }  

    void Update()
    {
      _currentCount = (_currentCount + 1) % (_count + _count);
      
      RgbColor color = _currentCount < _count ? _firstColor : _secondColor;
      
      for (int led = 0; led < _pixelCount; led++)
      {
        SetPixelColorWithGamma(led, color);
      }
      _pStrip->Show(); 
    }      
};

