#include "Melt7SegLcd.h"
// If distance between the changed chars is equals or greater then begin a new transmission
// As it would be cheaper than sending unchanged chars
#define DIFF_LEN_TRESHOLD 3

Melt7SegLcd::Melt7SegLcd(uint8_t i2cAddr, uint8_t displayLen, CharMapper *charMapper) {
  this->i2cAddr = i2cAddr;
  this->displayLen = displayLen;
  this->charMapper = charMapper;
  this->activeBuffer = 0;
  this->buffer0 = malloc(displayLen);
  this->buffer1 = malloc(displayLen);
  // The queue array contains pairs: offset, transmission length
  // This is used as transmission plan when sending the data partially
  this->queue = malloc((displayLen / (DIFF_LEN_TRESHOLD + 1) + 1) << 1);
  this->queueLen = 0;
  this->isLastDigitTouched = true;
  this->isWireTrasmitting = false;

  memset(buffer0, 0, displayLen);
  memset(buffer1, 0, displayLen);
}

void Melt7SegLcd::init() {
  Wire.beginTransmission(this->i2cAddr);
  this->isWireTrasmitting = true;

  // See PCF8576 datasheet
  // Table 4 - Definition of  commands
  Wire.write(B11001110); // Command: MODE SET
  Wire.write(B11100000); // Command: DEVICE SELECT
  Wire.write(B11111000); // Command: BANK SELECT
  Wire.write(B11110000); // Command: BLINK

  this->showAll(); // Clear the display
}

void Melt7SegLcd::show() {
  this->prepareTransmissionPlan();
  this->transmit();
}

void Melt7SegLcd::showAll() {
  this->queueLen = 2;
  this->queue[0] = 0;
  this->queue[1] = this->displayLen;
  this->transmit();
}

void Melt7SegLcd::prepareTransmissionPlan() {
  byte *diffBuffer = this->getDiffBuffer();
  byte *buffer = this->getBuffer();
  byte scanCounter = 0;
  boolean skipScanMode = true;
  this->queueLen = 0;

  for (byte i = 0; i < this->displayLen; i++) {
    if (skipScanMode && *diffBuffer == *buffer || !skipScanMode && *diffBuffer != *buffer) {
      scanCounter++;
      diffBuffer++;
      buffer++;
      continue;
    }

    if (skipScanMode) {
      // store skips, a char to print
      if (this->queueLen == 0 || scanCounter >= DIFF_LEN_TRESHOLD) {
        // initial skips or long skips
        this->queue[this->queueLen] = scanCounter;
        this->queueLen++;
        this->queue[this->queueLen] = 0; // Clear previous plan value
      } else {
        // merge the skips
        this->queueLen--;
        this->queue[this->queueLen] += scanCounter;
      }
    } else {
      // store prints, a char to skip
      this->queue[this->queueLen] += scanCounter;
      this->queueLen++;
    }

    // Toggle mode
    scanCounter = 1;
    skipScanMode = !skipScanMode;
    diffBuffer++;
    buffer++;
  }

  if (!skipScanMode) {
    this->queue[this->queueLen] += scanCounter;
  }
};

void Melt7SegLcd::transmit() {
  byte *buffer = this->getBuffer();
  byte scanCounter = 0;
  
  for (byte i = 0; i < this->queueLen; i += 2) {
    byte submitLen = this->queue[i + 1]; // queue[i + 1] is transmission length
    if (submitLen == 0) {
      break;
    }
    
    if (!this->isWireTrasmitting) {
      Wire.beginTransmission(this->i2cAddr);
      this->isWireTrasmitting = true;
    }
    if (this->isLastDigitTouched) {
      // This is needed only if we've touched last digit previously
      // Because PCF8576 automatically selects next device on last digit write
      Wire.write(B11100000); // Command: DEVICE SELECT
      this->isLastDigitTouched = false;
    }

    scanCounter += this->queue[i]; // queue[i] is offset
    Wire.write(scanCounter << 2); // The digit offset is digit offset multiplied by 4
    
    buffer += scanCounter;
    Wire.write(buffer, submitLen);
    
    scanCounter += submitLen;
    
    if (this->isWireTrasmitting) {
      Wire.endTransmission();
      this->isWireTrasmitting = false;
    }
  }

  if (scanCounter >= this->displayLen) {
    this->isLastDigitTouched = true;
  }

  this->toggleActiveBuffer();
}

void Melt7SegLcd::setString(char *str) {
  uint8_t *buffer = this->getBuffer();
  uint8_t i = 0;
  while (i < this->displayLen) {
    if (*str == '.') {
      if (i > 0) {
        buffer[i - 1] = this->charMapper->addDot(buffer[i - 1]);
      }
      str++;
      continue;
    }
    buffer[i] = this->charMapper->map(*str);
    i++;
    if (*str != 0) {
      str++;
    }
  }
}

void Melt7SegLcd::toggleActiveBuffer() {
  memcpy(this->getDiffBuffer(), this->getBuffer(), this->displayLen);
  this->activeBuffer = this->activeBuffer ? 0 : 1;
}

uint8_t *Melt7SegLcd::getBuffer() {
  return this->activeBuffer ? this->buffer1 : this->buffer0;
}

uint8_t *Melt7SegLcd::getDiffBuffer() {
  return this->activeBuffer ? this->buffer0 : this->buffer1;
}
