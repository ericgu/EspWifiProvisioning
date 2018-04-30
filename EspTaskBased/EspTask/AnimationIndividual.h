
class AnimationIndividual: public IAnimation
{
  private:
    RgbColor* _pColors;
    bool _rendered;
 
  public: 

    AnimationIndividual(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* pString, int pixelCount) : 
        IAnimation(pString, pixelCount)
    {
      _pColors = new RgbColor[pixelCount];      
    }

    int GetHexCharacter(const char ch)
    {
      static char * Hex = "0123456789abcdef";
      char * found = strchr(Hex, tolower(ch));
      return found ? found - Hex: 0;
    }

    RgbColor GetRgbColorFromBytes(const char* pMessage)
    {
      int red   = GetHexCharacter(*pMessage++) * 16 + GetHexCharacter(*pMessage++);
      int green = GetHexCharacter(*pMessage++) * 16 + GetHexCharacter(*pMessage++);
      int blue  = GetHexCharacter(*pMessage++) * 16 + GetHexCharacter(*pMessage++);

      return RgbColor(red, green, blue);
    }
    
    // ind bbb,RRGGBBRRGGBB...
    // block size (number for each color), followed by 2 digit Hex color value
    bool ProcessMessage(const char* pMessage)
    {
      if (*pMessage == 'i') // individual with specified chunk size.
      {
        pMessage += 4;
        int blockSize = atoi(pMessage); 

        pMessage += 4;  // skip block size

        // two hex bytes for color of each led

        RgbColor* pColor = _pColors;
        for (int led = 0; led < _pixelCount; led += blockSize)
        {
          RgbColor rgbColor = GetRgbColorFromBytes(pMessage);
          pMessage += 6;

          for (int block = 0; block < blockSize; block++)
          {
            *pColor = rgbColor;
            pColor++;
          }
        }
        
        _rendered = false;
        
        return true;
      }

      return false;
    }  

    void Update()
    {
      if (!_rendered)
      {
        RgbColor* pColor = _pColors;

        for (int led = 0; led < _pixelCount; led++)
        {
          _pStrip->SetPixelColor(led, *pColor);
          pColor++;
        }
        _pStrip->Show(); 
        
        _rendered = true;
      }
    }      
};

