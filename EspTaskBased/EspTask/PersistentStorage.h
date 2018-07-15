#include <EEPROM.h>

class PersistentStorage
{
  public:

    int  _version;
    int _size;
    char _ssid[33];
    char _password[65];
    char _hostName[33];
    char _ledCount;
    char _storedAnimation[3956];

    PersistentStorage()
    {
      memset(this, 0, sizeof(PersistentStorage));
    }

    void Save()
    {
      _version = 1001;
      _size = sizeof(PersistentStorage);
      
      EEPROM.begin(4096);
      byte* pData = (byte*) this;
      for (int i = 0; i < _size; i++)
      {
        EEPROM.write(i, *pData);
        pData++;
      }
      
      EEPROM.put(0, this);
      EEPROM.commit();
      Serial.println("Saved configuration");
    }

    void Load()
    {
      Serial.println("Load");
      EEPROM.begin(4096);

      byte* pData = (byte*) this;
      for (int i = 0; i < sizeof(PersistentStorage); i++)
      {
        *pData = EEPROM.read(i);
        pData++;
      }      
      EEPROM.get(0, this);    

      if (_ledCount == 0)
      {
        _ledCount = 33;
      }

      Serial.println("Loaded configuration");
      Serial.println(_hostName);
      Serial.println(_ssid);
      Serial.println(_password);
      Serial.println(_storedAnimation);
      Serial.println(_ledCount);
    }

    void Reset()
    {
      strcpy(_hostName, "");
      strcpy(_ssid, "");
      strcpy(_password, "");
      strcpy(_storedAnimation, "");
      _ledCount = 10;
      Save();
    }

    void SsidSet(String ssid)
    {
      strcpy(_ssid, ssid.c_str());
    }

    String SsidGet()
    {
      return String(_ssid);
    }
  
    void PasswordSet(String password)
    {
      strcpy(_password, password.c_str());
    }

    String PasswordGet()
    {
      return String(_password);
    }
  
    void HostNameSet(String hostName)
    {
      strcpy(_hostName, hostName.c_str());
    }

    String HostNameGet()
    {
      return String(_hostName);
    }

};

