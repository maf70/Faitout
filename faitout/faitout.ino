
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


OneWire oneWire(DS18B20_PIN);
DallasTemperature DS18B20(&oneWire);

// shared with web serveur for test purpose
uint16_t x, y;
uint32_t t, t_event;

char sd_status[32];
char sd_size[32];

float temperature = 0;

#ifdef CFG_WIFI_AP
const char* ssid = "FAITOUT-AP";
const char* password = "12345678";

WebServer server(80); // Port 80 est le port par défaut pour le web HTTP

void handleRoot() {
  char buffer[256];

  sprintf(buffer, "<h1>Faitout test page</h1><p>Version: %s<br>uptime: %ld<br>x=%d y=%d<br>%s%sTemperature = %f</p>",\
          FAITOUT_VERSION, t, x, y, sd_status, sd_size, temperature);
  //Serial.println(buffer);
  server.send(200, "text/html", buffer);
}
#endif

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
  tft.println("   Faitout");
  tft.setTextColor(ILI9341_BLUE);  tft.setTextSize(3);
  tft.println(FAITOUT_VERSION);
  tft.setCursor(110, 110);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(micros()/1000, DEC);
  return 0;
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
    sprintf(sd_status, "Card Mount Failed<br>");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    sprintf(sd_status, "No SD card attached <br>");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
    sprintf(sd_status, "SD Card Type: MMC<br>");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
    sprintf(sd_status, "SD Card Type: SDSC<br>");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
    sprintf(sd_status, "SD Card Type: SDHC<br>");
  } else {
    Serial.println("UNKNOWN");
    sprintf(sd_status, "SD Card Type: UNKNOWN<br>");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  sprintf(sd_size, "SD Card Size: %lluMB<br>", cardSize);
#endif

  DS18B20.begin();    // initialize the DS18B20 sensor
  DS18B20.setWaitForConversion(false);

  testText();

}

bool T_requested = false;
 
void loop() {
  if (tft.getTouch(&x, &y)) {
    t_event = millis()/1000;

    tft.fillCircle(x, y, 2, TFT_YELLOW);

    digitalWrite(TFT_BL, HIGH);

#define TXT_SIZE 3
    tft.fillRect(110, 110, TXT_SIZE*6*7 - TXT_SIZE, TXT_SIZE*7, TFT_BLACK);
    tft.setCursor(110, 110);
    tft.setTextColor(ILI9341_RED);    tft.setTextSize(TXT_SIZE);
    tft.print(x, DEC);
    tft.print(" ");
    tft.print(y, DEC);
  }

  t = millis()/1000;

  if ( t - t_event > 10 ) digitalWrite(TFT_BL, LOW);

#ifdef CFG_WIFI_AP
  server.handleClient(); // Gérer les clients du serveur web
#endif

  if ( ! T_requested ) {
    DS18B20.requestTemperatures();                  // send the command to get temperatures
    T_requested = true;
  }
  else
  {
    if (DS18B20.isConversionComplete() ) {
      temperature = DS18B20.getTempCByIndex(0); // read temperature in Celsius
      T_requested = false;
    }
  }

  delay(50);
}
