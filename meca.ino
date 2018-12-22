#include "WiFiEsp.h"
#include "rgb_lcd.h"

rgb_lcd lcd;
WiFiEspServer server(80);

typedef enum {
  HOME,
  PAILLEUSE_ON,
  PAILLEUSE_OFF,
} page_t;


void httpRespHeader(WiFiEspClient client)
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
}

void httpRespFooter(WiFiEspClient client)
{
  client.println("</html>");
}

void cmdPailleuse(const char* ip, bool on)
{
  WiFiEspClient client;

  if (!client.connect(ip, 5000)) return;
  
  lcd.clear();
  lcd.home();
  lcd.print("conn");
  lcd.print(on);

  if (on) {
    client.print("GET /on HTTP/1.1\r\n\r\n");  
  }
  else {
    client.print("GET /off HTTP/1.1\r\n\r\n"); 
  }

  while (client.connected()) {
    if (client.available()) {
      client.read();
    }
  }
  client.stop();
}

void httpResp(WiFiEspClient client, page_t page)
{
  httpRespHeader(client);
  if (page == PAILLEUSE_ON) {
    client.println("<a href='/p/off'>Off</a>");
  }
  else if (page == PAILLEUSE_OFF) {
    client.println("<a href='/p/on'>On</a>");
  }
  else {
    client.println("<a href='/p/on'>On</a> / <a href='/p/off'>Off</a>");
  }
  httpRespFooter(client);
}

void setup()
{
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  
  lcd.begin(20, 2);
  lcd.setRGB(255, 0, 0);
  lcd.print("Initializing...");

  Serial.begin(115200);
  WiFi.init(&Serial);
  
  while (WiFi.status() == WL_NO_SHIELD) {
    lcd.clear();
    lcd.home();
    lcd.print("No Wifi shield");
    lcd.setCursor(0, 1);
    lcd.print("Retrying... ");
    delay(500);
  }
  
  WiFi.beginAP("Meca");
  IPAddress ip(192, 168, 168, 100);
  WiFi.configAP(ip);

  server.begin();
  lcd.clear(); 
}

void loop()
{
  delay(1000);
  if (digitalRead(4) == HIGH) {
    lcd.home();
    lcd.print("4 on ");
    cmdPailleuse("192.168.168.101", true);
    cmdPailleuse("192.168.168.102", true);
  }
  else {
    lcd.home();
    lcd.print("4 off");
    cmdPailleuse("192.168.168.101", false);
    cmdPailleuse("192.168.168.102", false);
  }
  return; // TODO
  
  WiFiEspClient client = server.available();
  if (!client) return;

  page_t page = HOME;
  RingBuffer buf(15);
  buf.init();
  
  while (client.connected()) {
    if (!client.available()) continue;
    
    char c = client.read();
    buf.push(c);

    if (buf.endsWith("GET /p/on")) {
      page = PAILLEUSE_ON;
    }
    else if (buf.endsWith("GET /p/off")) {
      page = PAILLEUSE_OFF;
    }

    if (buf.endsWith("\r\n\r\n")) {
      httpResp(client, page);
      break;
    }
  }

  delay(10);
  client.stop();

  if (page == PAILLEUSE_ON || page == PAILLEUSE_OFF) {
    cmdPailleuse("192.168.168.105", page == PAILLEUSE_ON);
  }
}
