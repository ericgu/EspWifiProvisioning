#define MaxValues 16
  
class ParseNumbers
{
  public:
    int _count;
    int _values[MaxValues];
    const char* _pAfterNumbers;

    static bool IsDigit(const char c)
    {
      return c >= '0' && c <= '9';
    }

    static const char* SkipToFirstDigit(const char* pString)
    {
            // skip to first digit
      while (!IsDigit(*pString) && *pString != '\0')
      {
        pString++;    
      }

      return pString;
    }

    static const char* SkipToFirstNonDigit(const char* pString)
    {
            // skip to first digit
      while (IsDigit(*pString) && *pString != '\0')
      {
        pString++;    
      }

      return pString;
    }
    
    void Parse(const char* pValues)
    {
      _count = 0;
      pValues = SkipToFirstDigit(pValues);
      
      while (*pValues != '\0')
      {
        _values[_count] = atoi(pValues);
        _count++;

        pValues = SkipToFirstNonDigit(pValues);

        const char* pNext = SkipToFirstDigit(pValues);

        if (*pNext == '\0')
        {
          _pAfterNumbers = pValues;
        }

        pValues = pNext;
      }

      _pAfterNumbers = pValues;
    }

    void Dump()
    {
      Serial.println("Numbers");
      for (int i = 0; i < _count; i++)
      {
        Serial.print(i);
        Serial.print(" ");
        Serial.println(_values[i]);
      }
    }
};

