
class PixelMessageHandler
{
public:
    PixelMessageHandler(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* pStrip) : 
        _pStrip(pStrip)
        // initialize members here
    { 
    };

    int GetHexCharacter(char ch)
    {
      static char * Hex = "0123456789abcdef";
      char * found = strchr(Hex, tolower(ch));
      return found ? found - Hex: 0;
    }

    RgbColor GetRgbColorFromBytes(char* pMessage)
    {
      int red   = GetHexCharacter(*pMessage++) * 16 + GetHexCharacter(*pMessage++);
      int green = GetHexCharacter(*pMessage++) * 16 + GetHexCharacter(*pMessage++);
      int blue  = GetHexCharacter(*pMessage++) * 16 + GetHexCharacter(*pMessage++);

      return RgbColor(red, green, blue);
    }
    
    int ProcessMessage(char* pMessage)
    {
      int red = 0;
      int green = 0;
      int blue = 0;

      // rgb rrr,ggg,bbb
      if (*pMessage == 'r')
      {
        pMessage += 4;
        red = atoi(pMessage);  
        
        pMessage += 4;
        green = atoi(pMessage); 
         
        pMessage += 4;
        blue = atoi(pMessage);         
        
        pMessage += 4;
        int serial = atoi(pMessage); 

        RgbColor _setColor = RgbColor(red, green, blue);

        for (int led = 0; led < 33; led++)
        {
          _pStrip->SetPixelColor(led, _setColor);
        }
        _pStrip->Show();          

        return serial;
      }
      else if (*pMessage == 'a') // all
      {
        pMessage += 4;  // skip all and space

        // two hex bytes for color of each led
        for (int led = 0; led < 33; led++)
        {
          _pStrip->SetPixelColor(led, GetRgbColorFromBytes(pMessage));
          pMessage += 6;
        }
        
        _pStrip->Show();          

        return 0;
      }
      else if (*pMessage == 'b') // all LEDs with specified chunk size.
      {
        pMessage += 4;
        int blockSize = atoi(pMessage); 

        pMessage += 4;  // skip block size

        // two hex bytes for color of each led
        for (int led = 0; led < 33; led += blockSize)
        {
          RgbColor rgbColor = GetRgbColorFromBytes(pMessage);
          pMessage += 6;

          for (int block = 0; block < blockSize; block++)
          {
            _pStrip->SetPixelColor(led + block, rgbColor);
          }
        }
        
        _pStrip->Show();          

        return 0;
      }
      else
      {
        Serial.println(pMessage);
      }
    }

private:
    NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* _pStrip;
};
