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
    int _pixelCount;
    int _pixelPin;
    String _lastMessage;

    const int AnimationCount = 5;
    IAnimation** _pAnimations;
    IAnimation* _pCurrentAnimation;
   
  public:
    PixelHandler(PersistentStorage* pPersistentStorage, int pixelCount, int pixelPin) : 
        _pPersistentStorage(pPersistentStorage),
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
      _pAnimations[3] = new AnimationColorRotate(_pStrip, _pixelCount);
      _pAnimations[4] = new AnimationFlashDecay(_pStrip, _pixelCount);

      Serial.print("Attempting restart using: ");
      Serial.println(_pPersistentStorage->_startingAnimation);
      bool lastAnimationRestarted = ProcessMessage(_pPersistentStorage->_startingAnimation);
      if (!lastAnimationRestarted)
      {
        Serial.println("Fallback to red flash");
        ProcessMessage("alt 032,000,000,000,000,000,250");
      }

      _pCurrentAnimation->Update();
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
            
            return true;
          }
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
      else
      {
        Serial.print("Unrecognized: ");
        Serial.println(pMessage);
      }

      return false;
    }

    void Update()
    {
      _pCurrentAnimation->Update();
    }
};
