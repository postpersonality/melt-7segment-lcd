#include <Melt7SegCharMapper.h>
#include <Melt7SegLcd.h>

#define LCD_ADDR 0x38
#define LCD_LEN 10
Melt7SegCharMapper charMapper;
Melt7SegLcd melt7SegLcd(LCD_ADDR, LCD_LEN, &charMapper);

const static byte* testStr = "0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ -=_* \"o.k. 3.14\"";

void setup() {
  Wire.begin();
  melt7SegLcd.init();
}

byte i = 0;

void loop() {
  melt7SegLcd.setString(testStr + i++);
  melt7SegLcd.show();
  delay(500);
}
