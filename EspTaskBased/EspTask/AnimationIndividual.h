
class AnimationIndividual: public IAnimation
{
  private:
    RgbColor* _pColors;
    bool _rendered;
 
  public: 

    AnimationIndividual(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>** ppStrip) : 
        IAnimation(ppStrip, "Individual pixels")
    {
      _pColors = new RgbColor[GetPixelCount()];      
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
    bool ProcessMessage(const char* pMessage, ParseNumbers* pParseNumbers)
    {
      if (strstr(pMessage, "ind") == pMessage) // individual with specified chunk size.
      {
        pMessage = ParseNumbers::SkipToFirstDigit(pMessage);

        int blockSize = atoi(pMessage);

        pMessage = ParseNumbers::SkipToFirstNonDigit(pMessage);
        pMessage = ParseNumbers::SkipToFirstDigit(pMessage);

        Serial.println(pMessage);

        // two hex bytes for color of each led

        RgbColor* pColor = _pColors;
        for (int led = 0; led < GetPixelCount(); led += blockSize)
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

        for (int led = 0; led < GetPixelCount(); led++)
        {
          SetPixelColor(led, *pColor);
          pColor++;
        }
        (*_ppStrip)->Show(); 
        
        _rendered = true;
      }
    }      
};

