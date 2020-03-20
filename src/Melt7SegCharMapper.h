#ifndef Melt7SegCharMapper_h
#define Melt7SegCharMapper_h

#include <Arduino.h>
#include "CharMapper.h"

class Melt7SegCharMapper : public CharMapper {
  public:
    virtual uint8_t map(char chr);
    virtual uint8_t addDot(char chr);
};

#endif
