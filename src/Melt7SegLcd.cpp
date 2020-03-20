#include "Melt7SegLcd.h"

Melt7SegLcd::Melt7SegLcd(uint8_t i2cAddr, uint8_t displayLen, CharMapper *charMapper) {
  this->i2cAddr = i2cAddr;
  this->displayLen = displayLen;
  this->charMapper = charMapper;
  this->activeBuffer = 0;
  this->buffer0 = malloc(displayLen);
  this->buffer1 = malloc(displayLen);
  this->isLastDigitTouched = true;
  this->isWireTrasmitting = false;

  for (uint8_t i = 0; i < displayLen; i++) {
    this->buffer0[i] = 0;
  }
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
  byte *diffBuffer = this->getDiffBuffer();
  byte *buffer = this->getBuffer();
  byte queue[] = { 0, 0, 0, 0, 0, 0 }; // {skip, send, ...}
  byte queueLen = 0;
  byte scanCounter = 0;
  boolean skipScanMode = true;

  for (byte i = 0; i < this->displayLen; i++) {
    if (skipScanMode && *diffBuffer == *buffer || !skipScanMode && *diffBuffer != *buffer) {
      scanCounter++;
      diffBuffer++;
      buffer++;
      continue;
    }

    if (skipScanMode) {
      // store skips, a char to print
      if (queueLen == 0 || scanCounter >= 3) {
        // initial skips or long skips
        queue[queueLen] = scanCounter;
        queueLen++;
      } else {
        // merge the skips
        queueLen--;
        queue[queueLen] += scanCounter;
      }
    } else {
      // store prints, a char to skip
      queue[queueLen] += scanCounter;
      queueLen++;
    }

    // Toggle mode
    scanCounter = 1;
    skipScanMode = !skipScanMode;
    diffBuffer++;
    buffer++;
  }

  if (!skipScanMode) {
    queue[queueLen] += scanCounter;
  }
  
  bool isLastDigitWillBeTouched = scanCounter > 0 && !skipScanMode;

  scanCounter = 0;
  buffer = this->getBuffer();
  
  for (byte i = 0; i < queueLen; i += 2) {
    byte submitLen = queue[i + 1];
    if (submitLen == 0) {
      break;
    }
    
    if (!this->isWireTrasmitting) {
      Wire.beginTransmission(this->i2cAddr);
      this->isWireTrasmitting = true;
    }
    if (this->isLastDigitTouched) {
      Wire.write(B11100000); // Command: DEVICE SELECT
      this->isLastDigitTouched = false;
    }

    scanCounter += queue[i];
    Wire.write(scanCounter << 2); // Смещение стартового знака = (N знака * 4)
    
    buffer += scanCounter;
    Wire.write(buffer, submitLen);
    
    scanCounter += submitLen;
    
    if (this->isWireTrasmitting) {
      Wire.endTransmission();
      this->isWireTrasmitting = false;
    }
  }

  if (scanCounter) {
    this->isLastDigitTouched = isLastDigitWillBeTouched;
  }

  this->toggleActiveBuffer();
}

void Melt7SegLcd::showAll() {
  if (!this->isWireTrasmitting) {
    Wire.beginTransmission(this->i2cAddr);
    this->isWireTrasmitting = true;
  }

  if (this->isLastDigitTouched) {
    Wire.write(B11100000); // Command: DEVICE SELECT
  }

  byte *buffer = this->getBuffer();
  Wire.write(0);
  Wire.write(buffer, this->displayLen);

  Wire.endTransmission();
  this->isWireTrasmitting = false;
  this->isLastDigitTouched = true;
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
  this->activeBuffer = this->activeBuffer ? 0 : 1;
}

uint8_t *Melt7SegLcd::getBuffer() {
  return this->activeBuffer ? this->buffer1 : this->buffer0;
}

uint8_t *Melt7SegLcd::getDiffBuffer() {
  return this->activeBuffer ? this->buffer0 : this->buffer1;
}
