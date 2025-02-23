
#include "config.h"

#ifdef CFG_WIFI_AP
#include <WiFi.h>
#include <WebServer.h>
#endif

#include "tft_setup.h"
#include <TFT_eSPI.h>

// shared with web serveur for test purpose
uint16_t x, y;
uint32_t t, t_event;

#ifdef CFG_WIFI_AP
const char* ssid = "FAITOUT-AP";
const char* password = "12345678";

WebServer server(80); // Port 80 est le port par défaut pour le web HTTP

void handleRoot() {
  char buffer[128];

  sprintf(buffer, "<h1>Faitout test page</h1><p>Version: 0.1 - 23-Feb-2025<br>uptime: %ld<br>x=%d y=%d</p>", t, x, y);
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
  tft.println("Hello World!");
  tft.setCursor(50, 50);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(micros()/1000, DEC);
  return 0;
}

void setup(void)
{
  t = t_event = 0;

  Serial.begin(115200);

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

  testText();

}
 
void loop() {
  if (tft.getTouch(&x, &y)) {
    t_event = millis()/1000;

    tft.fillCircle(x, y, 2, TFT_YELLOW);

    digitalWrite(TFT_BL, HIGH);

#define TXT_SIZE 3
    tft.fillRect(50, 50, TXT_SIZE*6*7 - TXT_SIZE, TXT_SIZE*7, TFT_BLACK);
    tft.setCursor(50, 50);
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

}
