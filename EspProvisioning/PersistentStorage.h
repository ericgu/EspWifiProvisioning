#include <EEPROM.h>

class PersistentStorage
{
  public:

    int  _version;
    int _size;
    char _ssid[33];
    char _password[65];
    char _hostname[33];

    PersistentStorage()
    {
      Serial.println("clearing...");
      memset(this, 0, sizeof(PersistentStorage));
    }

    void save()
    {
      _version = 1;
      _size = sizeof(PersistentStorage);
      
      EEPROM.begin(512);
      byte* pData = (byte*) this;
      for (int i = 0; i < _size; i++)
      {
        EEPROM.write(i, *pData);
        pData++;
      }
      
      EEPROM.put(0, this);
      EEPROM.commit();
    }

    void load()
    {
      EEPROM.begin(512);

      byte* pData = (byte*) this;
      for (int i = 0; i < sizeof(PersistentStorage); i++)
      {
        *pData = EEPROM.read(i);
        pData++;
      }      
      EEPROM.get(0, this);      
    }
  
};

