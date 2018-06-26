class AnimationSegment
{
  public:

    int _period;
    String _message;
};

class AnimationSegments
{
  private:
  
    AnimationSegment _segments[10];
    bool _inTemporary;
    int _currentSegment;
    int _segmentCount;
    int _cycle;
    String _currentMessage;

  public:

    AnimationSegments()
    {
      _inTemporary = true;
      _currentSegment = 0;
      _segmentCount = 0;
      _cycle = 0;
    }

    void Add(String message, int period)
    {
      if (period < 0)   // set temporary animation
      {
        _segmentCount = 1;
        _inTemporary = true;
        _currentSegment = 0;
        _cycle = 0;
        
        _segments[0]._message = message;
        _segments[0]._period = 1000000;  
      }
      else
      {
        if (_inTemporary)
        {
          _segmentCount = 0;
          _inTemporary = false;
        }
      
        _segments[_segmentCount]._message = message;
        _segments[_segmentCount]._period = period;
        _segmentCount++;

          // force switch to the new one on the next cycle. 
        _currentSegment = _segmentCount - 1;
        _cycle = 0;
      }
    }

    String GetNextMessage()
    {
      if (_cycle <= 0)
      {
        _currentSegment = (_currentSegment + 1) % _segmentCount;

        if (_segments[_currentSegment]._message != _currentMessage)
        {
          _currentMessage = _segments[_currentSegment]._message;
          _cycle = _segments[_currentSegment]._period;
          return _currentMessage;
        }
      }
      _cycle--;

      return String("");
    }
};

