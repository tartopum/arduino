#include <Wire.h>
#include "WiFiEsp.h"
#include "rgb_lcd.h"


// LCD
rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

// WiFi
WiFiEspServer server(80);
RingBuffer buf(8);


bool beginAP()
{
  return WiFi.beginAP("Arduino", 10);
}


void setup()
{
  lcd.begin(20, 2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.print("Starting...");
  
  Serial.begin(115200);
  WiFi.init(&Serial);
  if (WiFi.status() == WL_NO_SHIELD) {
    lcd.home();
    lcd.print("No Wifi shield");
    lcd.setCursor(0, 1);
    lcd.print("Retrying...");
  }
  
  int retry = 0;
  while (WiFi.status() == WL_NO_SHIELD)
  {
    lcd.setCursor(12, 1);
    lcd.print(retry);
    retry++;
    WiFi.init(&Serial);
  }
  
  while (!beginAP()) {
    lcd.clear();
    lcd.home();
    lcd.print("Cannot start AP");
    lcd.setCursor(0, 1);
    lcd.print("Retrying...");
    lcd.print(retry);
    retry++;
  }

  IPAddress ip(192, 168, 168, 168);
  //WiFi.configAP(ip);
  server.begin();
  lcd.clear();
}


void loop()
{
  lcd.home();
  lcd.print(WiFi.localIP());
  
  WiFiEspClient client = server.available();
  if (!client) {
    return;
  }

  lcd.home();
  lcd.print("Client connected");

  buf.init();
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      buf.push(c);

      if (buf.endsWith("\r\n\r\n")) {
          sendHttpResponse(client);
          break;
      }

      if (buf.endsWith("GET /on")) {
        lcd.setCursor(0, 1);
        lcd.print("on ");
      }
      else if (buf.endsWith("GET /off")) {
        lcd.setCursor(0, 1);
        lcd.print("off");
      }
    }
  }

  client.stop();
}


void sendHttpResponse(WiFiEspClient client)
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  client.print("Arduino te salue !");

  client.println();
}
