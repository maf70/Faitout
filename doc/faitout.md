# Projet FAITOUT

## Goal 
Build a general purpose platform based on an ESP32-VROOM-32 + ILI9341:
- Color display and touchscreen of ili9341
- Use SD slot card of ili9641 to store any value and history
- ESP32 WiFi (as AP or maybe station)
- 4 GPIO to pilot some relay
- At least 2 GPIO for HW input buttons
- 1 GPIO for one-wired protocol (DS18B20 probes)
- some Input GPIO (e.g. opto module)
- manage a MAX6675 (thermocouple K) or equivalent
- manage linky probe

Design a PCB with :
- ESP32
- ILI9341
- buttons
- 4 relay card plugging
- Connectors for input/ouput
- I2C connector
- Uart 0 connector
- Connector to plug "hat"
- Optional: linky probe
- Optional/TBC: probe K

Usefull link:

Display and touch screen examples;
https://embeddedxpress.com/blog/2024/11/27/getting-started-with-esp32-ili9341-tft-display/
https://www.makerguides.com/interface-tft-ili9341-touch-display-with-esp32/

github TFTP_eSPI library to manage ili9341 display and touch screen on the same SPI bus:
https://github.com/Bodmer/TFT_eSPI/tree/master

ESP32 doc:
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html

Simple server to command GPIO:
https://www.electronicshub.org/esp32-web-server/

Demo of ili9341 Display + touchscreen + SD:
https://www.youtube.com/watch?v=rq5yPJbX_uk&ab_channel=XTronical

## ESP32





## ILI9341

### Screen pinout
Missing original link of this image.
![](ili9341-pinout.png)

### Touch pinout


### SD pinout



## Wiring

### TFTP + Touch + SD
|ILI93941|ESP32|Comment            |
|3.3     |3.3  |Rouge              |
|GND     |GND  |Noir               |
|CS      |D15  |Blanc              |
|Reset   |D17  |Jaune / TX2        |
|DC      |D16  |Bleu  / RX2        |
|MOSI    |D23  |Orange / VSPI      |
|SCK     |D18  |Vert               |
|LED     |D4   |Blanc              |
|MISO    | -   |Not connected      |
|T_CLK   |D18  |Vert               |
|T_CS    |D0   |Noir               |
|T_DIN   |D23  |Orange             |
|T_DO    |D19  |Bleu               |
|T_IRQ   | -   |Not connected      |
|SD_CS   |D5   |Blanc              | must be used for SD according to sample ESP32_SDcard_jpeg
|SD_MOSI |D23  |Vert clair         |
|SD_MISO |D19  |Jaune              |
|SD_CLK  |D18  |Violet             |

### DS18B20

DS18b20 => D32

### MAX66775
It does not work on the same SPI bus than previous devices.
(SPI mode is not the same)
|MAX6675|ESP32 |Comment            |
|SCK    |D2    |Jaune              |
|CS     |D13   |Vert               |
|DO     |D12   |Rouge              |

To do : try to change SPI setting before MAX6675 access, then set original setting after read.

### Physical button + optical counter
|Input    |ESP32 | Comment |
|Button 2 |D36   |         |
|Button 1 |D39   |         |
|Empty    |D34   |         |
|Optical  |D35   |         |



