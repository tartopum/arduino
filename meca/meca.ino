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
WiFiEspClient clientOut;
RingBuffer buf(20);
const char* pailleuseId = "192.168.168.101";


bool beginAP()
{
  return WiFi.beginAP("Meca", 10, "", 0, false);
}


void htmlHeader(WiFiEspClient client)
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
}


void htmlFooter(WiFiEspClient client)
{
  client.println();
  client.println();
}


bool get(const char* ip, const char* route)
{
  clientOut.stop();
  
  if (!clientOut.connect(ip, 80)) {
    lcd.clear();
    lcd.home();
    lcd.print(ip);
    return false;
  }

  clientOut.print("GET ");
  clientOut.print(route);
  clientOut.println(" HTTP/1.1");
  clientOut.print("Host: ");
  clientOut.println(ip);
  clientOut.println("Connection: close");
  clientOut.println();
  return true;
}


void setup()
{
  lcd.begin(20, 2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.print("Initializing...");
  
  Serial.begin(115200);
  WiFi.init(&Serial);
  
  while (WiFi.status() == WL_NO_SHIELD) {
    lcd.clear();
    lcd.home();
    lcd.print("No Wifi shield");
    lcd.setCursor(0, 1);
    lcd.print("Retrying... ");
    delay(1000);
  }
  
  while (!beginAP()) {
    lcd.clear();
    lcd.home();
    lcd.print("Cannot start AP");
    lcd.setCursor(0, 1);
    lcd.print("Retrying...");
  }

  IPAddress ip(192, 168, 168, 100);
  WiFi.configAP(ip);
  server.begin();
  
  lcd.clear();
  lcd.home();
  lcd.print(WiFi.localIP());
}


void loop()
{
  WiFiEspClient client = server.available();
  if (!client) {
    return;
  }

  buf.init();
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      buf.push(c);

      if (buf.endsWith("\r\n\r\n")) {
        htmlHeader(client);
        client.print("<a href='/p/on'>On</a> / <a href='/p/off'>Off</a>");
        htmlFooter(client);
        break;
      }

      if (buf.endsWith("GET /p/on")) {
        lcd.setCursor(0, 1);
        lcd.print("on ");
        get("192.168.168.101", "/on");
      }
      else if (buf.endsWith("GET /p/off")) {
        lcd.setCursor(0, 1);
        lcd.print("off ");
        get("192.168.168.101", "/off");
      }
    }
  }

  delay(10);
  client.stop();
}
