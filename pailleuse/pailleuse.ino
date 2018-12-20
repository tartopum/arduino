#include "WiFiEsp.h"
#include "multi_channel_relay.h"

// WiFi
WiFiEspServer server(80);
RingBuffer buf(20);

// Debug
Multi_Channel_Relay relay;


void debug(int i)
{
  if (1 <= i && i <= 4) {
    relay.turn_on_channel(i);
    return;
  }
  for (int k = 1; k < 5; k++) {
    relay.turn_off_channel(k);
  }
}


int connectAP()
{
  return WiFi.begin("Meca", "");
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


void setup()
{
  // Debug
  relay.begin(0x21);
  debug(0);
  
  Serial.begin(115200);
  WiFi.init(&Serial);
  
  while (WiFi.status() == WL_NO_SHIELD) {
    delay(1000);
  }
  
  while (connectAP() != WL_CONNECTED);

  IPAddress ip(192, 168, 168, 101);
  WiFi.config(ip);
  server.begin();
  debug(1);
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
        client.print("Pailleuse");
        htmlFooter(client);
        break;
      }

      if (buf.endsWith("GET /on")) {
        debug(2);
      }
      else if (buf.endsWith("GET /off")) {
        debug(3);
      }
    }
  }

  // Give the web browser time to receive the data
  delay(10);
  client.stop();
}
