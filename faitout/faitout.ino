
#include "tft_setup.h"
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();
uint16_t cal[5] = {441, 3477, 244, 3583, 5};

void calibrate_touch() {
  if (!cal[1]) {
    tft.fillScreen(TFT_BLACK);
    tft.calibrateTouch(cal, TFT_YELLOW, TFT_BLACK, 20);
    Serial.printf("cal[5] = {%d, %d, %d, %d, %d};\n",
                  cal[0], cal[1], cal[2], cal[3], cal[4]);
  }
}

unsigned long testText() {
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_GREEN);  tft.setTextSize(4);
  tft.println("Hello World!");
  tft.setCursor(50, 50);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(micros()/1000, DEC);
  return 0;
}

void setup(void)
{
  Serial.begin(115200);

  tft.init();
  tft.setRotation(3);

  calibrate_touch();
  tft.setTouch(cal);

  testText();

}
 
void loop() {
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    tft.fillCircle(x, y, 2, TFT_YELLOW);

#define TXT_SIZE 3
    tft.fillRect(50, 50, TXT_SIZE*6*7 - TXT_SIZE, TXT_SIZE*7, TFT_BLACK);
    tft.setCursor(50, 50);
    tft.setTextColor(ILI9341_RED);    tft.setTextSize(TXT_SIZE);
    tft.print(x, DEC);
    tft.print(" ");
    tft.print(y, DEC);
  }

}
