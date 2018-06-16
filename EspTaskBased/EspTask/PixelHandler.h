#include "ParseNumbers.h"

#include "IAnimation.h"
#include "AnimationBlendTo.h"
#include "AnimationAlternate.h"
#include "AnimationIndividual.h"
#include "AnimationColorRotate.h"
#include "AnimationFlashDecay.h"

class PixelHandler
{
  private:
    NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* _pStrip;
    PersistentStorage* _pPersistentStorage;
    int _pixelPin;
    String _lastMessage;

    const int AnimationCount = 5;
    IAnimation** _pAnimations;
    IAnimation* _pCurrentAnimation;
    bool _pixelCountUpdated;

  public:
    PixelHandler(PersistentStorage* pPersistentStorage, int pixelPin) : 
        _pPersistentStorage(pPersistentStorage),
        _pixelPin(pixelPin)
    { 
      _pStrip = 0;
      _pixelCountUpdated = false;
    }

    NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>* GetStrip()
    {
      return _pStrip;
    }

    int GetPixelCount()
    {
      return _pPersistentStorage->_ledCount;
    }

    void CreateAnimations()
    {
      _pAnimations = new IAnimation*[AnimationCount];

      _pAnimations[0] = new AnimationBlendTo(&_pStrip);
      _pAnimations[1] = new AnimationAlternate(&_pStrip);
      _pAnimations[2] = new AnimationIndividual(&_pStrip);
      _pAnimations[3] = new AnimationColorRotate(&_pStrip);
      _pAnimations[4] = new AnimationFlashDecay(&_pStrip);
    }

    void ClearStrip()
    {
      RgbColor black(0, 0, 0);
      for (int led = 0; led < GetPixelCount(); led++)
      {
        Serial.println(led);
        _pStrip->SetPixelColor(led, black);  
      }
      _pStrip->Show();
    }

    void InitStrip()
    {
      Serial.println("A");
      if (_pStrip != 0)
      {
      Serial.println("B");
        RgbColor black(0, 0, 0);
        for (int led = 0; led < GetPixelCount(); led++)
        {
          Serial.println(led);
          _pStrip->SetPixelColor(led, black);  
        }

      Serial.println("C");
        delete _pStrip;
      }
      Serial.println("D");
      _pStrip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(_pPersistentStorage->_ledCount, _pixelPin);
      Serial.println("E");
      _pStrip->Begin();
      Serial.println("F");
    }

    void Init()
    {
      InitStrip();
      CreateAnimations();

      SetUnconnectedAnimation();

      _pCurrentAnimation->Update();
    }

    void SetUnconnectedAnimation()
    {
      Serial.println("Red alternate");
      ProcessMessage("alt 100,000,000,000,000,000,250");
    }

    void SetAccessPointAnimation()
    {
      Serial.println("Yellow alternate");
      ProcessMessage("alt 150,150,000,000,000,000,250");
    }

    void SetProvisionedAnimation()
    {
      Serial.print("Attempting restart using: ");
      Serial.println(_pPersistentStorage->_startingAnimation);
      bool lastAnimationRestarted = ProcessMessage(_pPersistentStorage->_startingAnimation);
      if (!lastAnimationRestarted)
      {
        Serial.println("Green alternate");
        ProcessMessage("alt 0,150,000,000,000,000,250");
      }
    }

    bool ProcessMessage(const char* pMessage)
    {        
      ParseNumbers parseNumbers;
      parseNumbers.Parse(pMessage);
      //parseNumbers.Dump();
      
      for (int i = 0; i < AnimationCount; i++)
      {
        if (_pAnimations[i]->ProcessMessage(pMessage, &parseNumbers))
        {
          if (_pAnimations[i] != _pCurrentAnimation)
          {
            _pCurrentAnimation = _pAnimations[i];
            _lastMessage = pMessage;
            Serial.print("Switched to: ");
            Serial.println(_pCurrentAnimation->getName());
          }
            
          return true;
        }
      }

      if (*pMessage == 's')   // save the last animation to flash
      {
        if (_lastMessage.length() < sizeof(_pPersistentStorage->_startingAnimation))
        {
          _lastMessage.toCharArray(_pPersistentStorage->_startingAnimation, sizeof(_pPersistentStorage->_startingAnimation));

          Serial.println("Last message");
          Serial.println(_lastMessage);
          _pPersistentStorage->Save();
        }
      }
      else if (*pMessage == 'n') // set number of leds
      {
        parseNumbers.Dump();

        _pPersistentStorage->_ledCount = parseNumbers._values[0];
        _pixelCountUpdated = true;
        _pPersistentStorage->Save();
        ClearStrip();
        ESP.restart();
      }
      else
      {
        Serial.print("Unrecognized: ");
        Serial.println(pMessage);
      }

      return false;
    }

    void Update()
    {
      if (_pixelCountUpdated)
      {
        Serial.println("> new pixel count");
        _pixelCountUpdated = false;
        InitStrip();
        Serial.println("< new pixel count");
      }
      
      _pCurrentAnimation->Update();
    }
};
