
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
 
#define TFT_SCK    18
#define TFT_MOSI   23
#define TFT_MISO   19
#define TFT_CS     5
#define TFT_DC     16
#define TFT_RESET  17
 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RESET, TFT_MISO);

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
  tft.begin();
}
 
void loop() {
    testText();
    delay(5000);
  
  }
