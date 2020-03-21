#ifndef Melt7SegLcd_h
#define Melt7SegLcd_h

#include <Arduino.h>
#include <Wire.h>
#include "CharMapper.h"

class Melt7SegLcd {
  public:
    Melt7SegLcd(uint8_t i2cAddr, uint8_t displayLen, CharMapper *charMapper);
    void init();
    void show();
    void showAll();
    void setString(char *str);

  protected:
    uint8_t i2cAddr;
    uint8_t displayLen;
    uint8_t activeBuffer;
    uint8_t *buffer0;
    uint8_t *buffer1;
    uint8_t *queue;
    uint8_t queueLen;
    bool isLastDigitTouched;
    bool isWireTrasmitting;
    CharMapper *charMapper;

    void toggleActiveBuffer();
    uint8_t *getBuffer();
    uint8_t *getDiffBuffer();
    void prepareTransmissionPlan();
    void transmit();
};

#endif
