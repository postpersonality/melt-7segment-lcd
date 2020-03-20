#ifndef CharMapper_h
#define CharMapper_h

#include <Arduino.h>

class CharMapper {
  public:
    virtual uint8_t map(char chr) = 0;
    virtual uint8_t addDot(char chr) = 0;
    virtual ~CharMapper() {} 
};

#endif
