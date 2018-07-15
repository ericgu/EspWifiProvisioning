#include "ParseNumbers.h"

#include "AnimationSegment.h"

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
    AnimationSegments _animationSegments;

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
        _pStrip->SetPixelColor(led, black);  
      }
      _pStrip->Show();
    }

    void InitStrip()
    {
      _pStrip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(_pPersistentStorage->_ledCount, _pixelPin);
      _pStrip->Begin();
    }

    void Init()
    {
      InitStrip();
      CreateAnimations();

      SetUnconnectedAnimation();

      //_pCurrentAnimation->Update();
    }

    void SetUnconnectedAnimation()
    {
      Serial.println("Red alternate");
      ProcessMessage("alt 100,000,000,000,000,000,250", -1);
    }

    void SetAccessPointAnimation()
    {
      Serial.println("Yellow alternate");
      ProcessMessage("alt 150,150,000,000,000,000,250", -1);
    }

    void SetProvisionedAnimation()
    {
      Serial.print("Attempting restart using: ");
      Serial.println(_pPersistentStorage->_storedAnimation);
      ProcessMessage(_pPersistentStorage->_storedAnimation, -1);
      //if (!lastAnimationRestarted)
      //{
      //  Serial.println("Green alternate");
      //  ProcessMessage("alt 0,150,000,000,000,000,250", -1);
      //}
    }

    void ProcessMessage(const char* pMessage, int period)
    {
      ParseNumbers parseNumbers;
      parseNumbers.Parse(pMessage);
      
      if (*pMessage == 's')   // save the last animation to flash
      {
        if (_lastMessage.length() < sizeof(_pPersistentStorage->_storedAnimation))
        {
          _lastMessage.toCharArray(_pPersistentStorage->_storedAnimation, sizeof(_pPersistentStorage->_storedAnimation));

          Serial.println("Last message");
          Serial.println(_lastMessage);
          _pPersistentStorage->Save();
        }
      }
      else if (*pMessage == 'n') // set number of leds
      {
        _pPersistentStorage->_ledCount = parseNumbers._values[0];
        _pixelCountUpdated = true;
        _pPersistentStorage->Save();
        ClearStrip();
        ESP.restart();
      }
      else if (strstr(pMessage, "reset") == pMessage)
      {
        _pPersistentStorage->Reset();
        ClearStrip();
        ESP.restart();        
      }
      else
      {
        _animationSegments.Add(pMessage, period);
      }
    }

    bool ProcessMessageInt(const char* pMessage)
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

      Serial.print("Unrecognized: ");
      Serial.println(pMessage);

      return false;
    }

    void Update()
    {
      String message = _animationSegments.GetNextMessage();
      if (message.length() != 0)
      {
        Serial.print("Updated to: ");
        Serial.println(message);
        ProcessMessageInt(message.c_str());
      }
      
      _pCurrentAnimation->Update();
    }
};
