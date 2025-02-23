// tft_setup.h
// 2.8" TFT Touch Display
// 240x 320, Driver: ILI9341 

#define ILI9341_DRIVER      
//#define ILI9341_2_DRIVER  

#define TFT_WIDTH  240
#define TFT_HEIGHT 320
#define TFT_RGB_ORDER TFT_BGR  

#define TFT_BACKLIGHT_ON HIGH

#define TFT_SCLK   18
#define TFT_MOSI   23
#define TFT_MISO   19
#define TFT_CS     5
#define TFT_DC     16
#define TFT_RST    17

#define TFT_BL    22  // LED back-light

#define TOUCH_CS  0
#define TOUCH_CLK TFT_SCLK
#define TOUCH_DIN TFT_MOSI
#define TOUCH_DO  TFT_MISO

#define LOAD_GLCD   
#define LOAD_FONT2  
#define LOAD_FONT4  
#define LOAD_FONT6  
#define LOAD_FONT7  
#define LOAD_FONT8  
#define LOAD_GFXFF
#define SMOOTH_FONT 

#define SPI_FREQUENCY       27000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
