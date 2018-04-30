
class AnimationAlternate: public IAnimation
{
  private:
    RgbColor _firstColor;
    RgbColor _secondColor;
    int _count;
    int _currentCount = 0;

 
  public: 

    AnimationAlternate(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* pString, int pixelCount) : 
        IAnimation(pString, pixelCount)
        {}

    // alt rrr,ggg,bbb,rrr,ggg,bbb,cccc
    //      (first)       (second)  (count)
    bool ProcessMessage(const char* pMessage)
    {
      int red;
      int green;
      int blue;

      if (*pMessage == 'a')
      {
        pMessage += 4;
        red = atoi(pMessage);  
        
        pMessage += 4;
        green = atoi(pMessage); 
         
        pMessage += 4;
        blue = atoi(pMessage);         

        _firstColor = RgbColor(red, green, blue);
        
        pMessage += 4;
        red = atoi(pMessage);  
        
        pMessage += 4;
        green = atoi(pMessage); 
         
        pMessage += 4;
        blue = atoi(pMessage);         

        _secondColor = RgbColor(red, green, blue);
        
        pMessage += 4;
        _count = atoi(pMessage);
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
        _pStrip->SetPixelColor(led, color);
      }
      _pStrip->Show(); 
    }      
};

