#include "Melt7SegCharMapper.h"

// MELT LCD 7-segment map:
//   777
//  5   6
//  5   6
//   444
//  3   2
//  3   2
//   111  00

static const uint8_t charMap[] PROGMEM = {
  B11101110, // "0"  48
  B01000100, // "1"  49
  B11011010, // "2"  50  Same as 'Z'
  B11010110, // "3"  51
  B01110100, // "4"  52
  B10110110, // "5"  53
  B10111110, // "6"  54
  B11000100, // "7"  55
  B11111110, // "8"  56
  B11110110, // "9"  57
  B11111100, // 'A'  65
  B00111110, // 'b'  66
  B10101010, // 'C'  67
  B01011110, // 'd'  68
  B10111010, // 'E'  69
  B10111000, // 'F'  70
  B10101110, // 'G'  71
  B00111100, // 'h'  72
  B00101000, // 'I'  73
  B01000110, // 'J'  74
  B01111010, // 'K'  75
  B00101010, // 'L'  76
  B10011100, // 'M'  77  n^
  B00011100, // 'n'  78
  B11101110, // 'O'  79
  B11111000, // 'P'  80
  B11110100, // 'q'  81
  B00011000, // 'r'  82
  B10110110, // 'S'  83
  B00111010, // 't'  84
  B01101110, // 'U'  85
  B00001110, // 'v'  86  
  B10001110, // 'W'  87  u^
  B01111100, // 'X'  88  
  B01110110, // 'y'  89
  B11011010, // 'Z'  90  Same as '2'
  B00000000, // ' '  32  [36] BLANK
  B00010000, // '-'  45  [37] DASH
  B00000010, // '_'  95  [38] UNDERSCORE
  B00000001, // '.'  46  [39] PERIOD
  B11110000, // '*'  42  [40] DEGREE
  B01100000, // '"'      [41] DBL QUOTE
  B00010010, // '='      [42] EQUAL
  B10000000  // '~'      [43] TILDE
};

uint8_t Melt7SegCharMapper::map(char chr) {
  // Divide ASCII to zones of chars with binary search
  if (chr < 'A') {
    if (chr < '0') {
      // Special chars #1
      if (chr == ' ') {
        return pgm_read_byte_near(charMap + 36);
      }
      if (chr == '-') {
        return pgm_read_byte_near(charMap + 37);
      }
      if (chr == '*') {
        return pgm_read_byte_near(charMap + 40);
      }
      if (chr == '"') {
        return pgm_read_byte_near(charMap + 41);
      }
      if (chr == '.') {
        return pgm_read_byte_near(charMap + 39);
      }
    } else {
      if (chr <= '9') {
        // Digits
        return pgm_read_byte_near(charMap + chr - 48);
      }
      // Special chars #2
      if (chr == '=') {
        return pgm_read_byte_near(charMap + 42);
      }
    }   
  } else {
    if (chr < 'a') {
      if (chr <= 'Z') {
        // Capital letters
        return pgm_read_byte_near(charMap + chr - 'A' + 10);
      }
      // Special chars #3
      if (chr == '_') {
        return pgm_read_byte_near(charMap + 38);
      }
    } else {
      if (chr <= 'z') {
        // Small letters
        return pgm_read_byte_near(charMap + chr - 'a' + 10);
      }
      // Special chars #4
      if (chr == '~') {
        return pgm_read_byte_near(charMap + 43);
      }
    }
  }

  return pgm_read_byte_near(charMap + 36);
}

uint8_t Melt7SegCharMapper::addDot(char chr) {
  return chr | B00000001;
}
