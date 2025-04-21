
#include "config.h"

#ifdef CFG_WIFI_AP
#include <WiFi.h>
#include <WebServer.h>
#endif

#ifdef CFG_SD_CARD
#include <SPI.h>
#include "FS.h"
#include "SD.h"
#endif

#include "tft_setup.h"
#include <TFT_eSPI.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#include "max6675.h"

OneWire oneWire(DS18B20_PIN);
DallasTemperature DS18B20(&oneWire);

MAX6675 thermocouple(MAX6675_SCLK, MAX6675_CS, MAX6675_MISO);

// shared with web serveur for test purpose
uint16_t x, y;
uint32_t t, t_event;

char sd_status[32];
char sd_size[32];

float temperature_0 = 0;
float temperature_1 = 0;
float temperature_k = 0;
uint8_t probe_nb = 0;

long optic_counter = 0;
long b1_counter = 0;
long b2_counter = 0;

int relay1 = 0;
int relay2 = 0;
int relay3 = 0;
int relay4 = 0;

#ifdef CFG_WIFI_AP
const char* ssid = "FAITOUT-AP";
const char* password = "12345678";

WebServer server(80); // Port 80 est le port par défaut pour le web HTTP

void handleRoot() {
  char buffer[512];

  sprintf(buffer, "<h1>Faitout test page</h1><p>Version: %s<br>uptime: %ld<br>x=%d y=%d<br>%s<br>%s<br>\
          Temperature 0 = %3.3f<br>Temperature 1 = %3.3f<br>Sonde K = %4.2f<br>Capteur Optique = %d<br>Bouton 1 = %d<br>Bouton 2 = %d</p>",\
          FAITOUT_VERSION, t, x, y, sd_status, sd_size, temperature_0, temperature_1, temperature_k, optic_counter, b1_counter, b2_counter);
  //Serial.println(buffer);
  server.send(200, "text/html", buffer);
}
#endif

TFT_eSPI tft = TFT_eSPI();
// Ecran 2019
// uint16_t cal[5] = {441, 3477, 244, 3583, 5};
// Ecran 2025
uint16_t cal[5] = {268, 3632, 360, 3433, 2};

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
  tft.fillRect(0, 0, 80, 30, ILI9341_RED);
  tft.fillRect(80, 0, 80, 30, ILI9341_ORANGE);
  tft.fillRect(160, 0, 80, 30, ILI9341_BLUE);
  tft.fillRect(240, 0, 80, 30, ILI9341_GREEN);
  tft.setCursor(0, 35);
  tft.setTextColor(ILI9341_GREEN);  tft.setTextSize(4);
  tft.println("   Faitout");
  tft.setTextColor(ILI9341_BLUE);  tft.setTextSize(3);
  tft.println(FAITOUT_VERSION);
  tft.setCursor(110, 110);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(micros()/1000, DEC);
  return 0;
}

void IRAM_ATTR Ext_IntO_ISR()
{
  // Increment counter
  optic_counter++;
}

void IRAM_ATTR Ext_IntB1_ISR()
{
  // Increment counter
  b1_counter++;
}

void IRAM_ATTR Ext_IntB2_ISR()
{
  // Increment counter
  b2_counter++;
}

void setup(void)
{
  t = t_event = 0;

  Serial.begin(115200);

  // Set all chip selects high to avoid bus contention during initialisation of each peripheral
  digitalWrite(TOUCH_CS, HIGH); // Touch controller chip select (if used)
  digitalWrite(  TFT_CS, HIGH); // TFT screen chip select
  digitalWrite(   SD_CS, HIGH); // SD card chips select, must use GPIO 5 (ESP32 SS)

  tft.init();
  tft.setRotation(3);

  // set the digital pin as output and switch on the LED:
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  calibrate_touch();
  tft.setTouch(cal);

#ifdef CFG_WIFI_AP
  // Configurer l’ESP32 en mode AP (Point d’accès)
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: " );
  Serial.println(IP);

  // Routeur pour la racine (« / »)
  server.on("/", handleRoot);

  // Démarrer le serveur
  server.begin();
  Serial.println("HTTP server started");
#endif

#ifdef CFG_SD_CARD
  if (!SD.begin(5, tft.getSPIinstance())) {
    Serial.println("Card Mount Failed");
    sprintf(sd_status, "Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    sprintf(sd_status, "No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
    sprintf(sd_status, "SD Card Type: MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
    sprintf(sd_status, "SD Card Type: SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
    sprintf(sd_status, "SD Card Type: SDHC");
  } else {
    Serial.println("UNKNOWN");
    sprintf(sd_status, "SD Card Type: UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  sprintf(sd_size, "SD Card Size: %lluMB", cardSize);
#endif

  DS18B20.begin();    // initialize the DS18B20 sensor
  DS18B20.setWaitForConversion(false);

  testText();

  pinMode(INPUT_PIN_1, INPUT);
  attachInterrupt(INPUT_PIN_1, Ext_IntO_ISR, FALLING);
  pinMode(INPUT_PIN_3, INPUT);
  attachInterrupt(INPUT_PIN_3, Ext_IntB1_ISR, FALLING);
  pinMode(INPUT_PIN_4, INPUT);
  attachInterrupt(INPUT_PIN_4, Ext_IntB2_ISR, FALLING);

  pinMode(OUTPUT_PIN_1, OUTPUT);
  digitalWrite(OUTPUT_PIN_1, HIGH);
  pinMode(OUTPUT_PIN_2, OUTPUT);
  digitalWrite(OUTPUT_PIN_2, HIGH);
  pinMode(OUTPUT_PIN_3, OUTPUT);
  digitalWrite(OUTPUT_PIN_3, HIGH);
  pinMode(OUTPUT_PIN_4, OUTPUT);
  digitalWrite(OUTPUT_PIN_4, HIGH);
}

bool T_requested = false;
 
void loop() {
  if (tft.getTouch(&x, &y)) {
    char tempText[7];

    t_event = millis()/1000;

    tft.fillCircle(x, y, 2, TFT_YELLOW);

    digitalWrite(TFT_BL, HIGH);

    Serial.print(x, DEC);
    Serial.print(" ");
    Serial.println(y, DEC);

#define TXT_SIZE 3
    tft.fillRect(110, 110, TXT_SIZE*6*7 - TXT_SIZE, TXT_SIZE*7, TFT_BLACK);
    tft.setCursor(110, 110);
    tft.setTextColor(ILI9341_RED, TFT_BLACK);    tft.setTextSize(TXT_SIZE);
    tft.print(x, DEC);
    tft.print(" ");
    tft.print(y, DEC);

    if (temperature_0 != 0) {
      tft.fillRect(10, 140, TXT_SIZE*6*7 - TXT_SIZE, TXT_SIZE*7, TFT_BLACK);
      tft.setCursor(10, 140);
      tft.setTextColor(ILI9341_RED, TFT_BLACK);    tft.setTextSize(TXT_SIZE);
      sprintf(tempText, "%3.2f",temperature_0);
      tft.print(tempText);
    }

    if (temperature_1 != 0) {
      tft.fillRect(140, 140, TXT_SIZE*6*7 - TXT_SIZE, TXT_SIZE*7, TFT_BLACK);
      tft.setCursor(140, 140);
      tft.setTextColor(ILI9341_RED, TFT_BLACK);    tft.setTextSize(TXT_SIZE);
      sprintf(tempText, "%3.2f",temperature_1);
      tft.print(tempText);
    }

    tft.fillRect(10, 170, TXT_SIZE*6*7 - TXT_SIZE, TXT_SIZE*7, TFT_BLACK);
    tft.setCursor(10, 170);
    tft.setTextColor(ILI9341_RED, TFT_BLACK);    tft.setTextSize(TXT_SIZE);
    sprintf(tempText, "%3.2f",temperature_k);
    tft.print(tempText);

    tft.fillRect(10, 200, TXT_SIZE*6*15 - TXT_SIZE, TXT_SIZE*15, TFT_BLACK);
    tft.setCursor(10, 200);
    tft.print(optic_counter, DEC);
    tft.print(" ");
    tft.print(b1_counter, DEC);
    tft.print(" ");
    tft.print(b2_counter, DEC);

    if ( y < 30) {
      if (x < 80) {
        digitalWrite(OUTPUT_PIN_1, LOW);
        relay1=60;
      }
      else if (x < 160) {
        digitalWrite(OUTPUT_PIN_2, LOW);
        relay2=60;
      }
      else if (x < 240) {
        digitalWrite(OUTPUT_PIN_3, LOW);
        relay3=60;
      }
      else {
        digitalWrite(OUTPUT_PIN_4, LOW);
        relay4=60;
      }
    }
  }

  t = millis()/1000;

  if ( t - t_event > 30 ) digitalWrite(TFT_BL, LOW);

#ifdef CFG_WIFI_AP
  server.handleClient(); // Gérer les clients du serveur web
#endif

  if ( ! T_requested ) {
    probe_nb = DS18B20.getDeviceCount();
    DS18B20.requestTemperatures();                  // send the command to get temperatures
    T_requested = true;
  }
  else
  {
    if (DS18B20.isConversionComplete() ) {
      if (probe_nb > 0 ) temperature_0 = DS18B20.getTempCByIndex(0); // read temperature in Celsius
      if (probe_nb > 1 ) temperature_1 = DS18B20.getTempCByIndex(1); // read temperature in Celsius
      T_requested = false;

      temperature_k = thermocouple.readCelsius();
    }
  }

  if (relay1 > 0) {
    if (--relay1 == 0 ) digitalWrite(OUTPUT_PIN_1, HIGH);
  }
  if (relay2 > 0) {
    if (--relay2 == 0 ) digitalWrite(OUTPUT_PIN_2, HIGH);
  }
  if (relay3 > 0) {
    if (--relay3 == 0 ) digitalWrite(OUTPUT_PIN_3, HIGH);
  }
  if (relay4 > 0) {
    if (--relay4 == 0 ) digitalWrite(OUTPUT_PIN_4, HIGH);
  }

  delay(50);

}
