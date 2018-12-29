#include "WiFi.h"

WiFiEspServer _server(80);
RingBuffer _httpBuf(50);

// Declare a static string
#ifdef __AVR__
    #define P(name)   static const char name[] __attribute__(( section(".progmem." #name) ))
#else
    #define P(name)   static const char name[]
#endif

#define CRLF "\r\n\r\n"

P(httpHeaderPrefix) =
    "HTTP/1.1 200 OK" CRLF
    "Content-Type: text/html" CRLF
    "Connection: close" CRLF
    "Content-Length: ";

P(htmlHeader) =
    "<!DOCTYPE HTML>"
    "<html>"
    "<head></head>"
    "<body>";

P(htmlFooter) =
    "</body>"
    "</html>";

void _httpRespHeader(WiFiEspClient client, unsigned int contentLen)
{
    client.print(httpHeaderPrefix);
    client.println(contentLen + strlen(htmlHeader) + strlen(htmlFooter));
    client.println("");
    client.println(htmlHeader);
}

void _httpRespFooter(WiFiEspClient client)
{
    client.println(htmlFooter);
}

void _httpResp(WiFiEspClient client)
{
    _httpRespHeader(client, 4);
    // TODO: in program memory?
    client.print("TODO");
    _httpRespFooter(client);
}

void setupWiFi()
{
    Serial.begin(115200);
    WiFi.init(&Serial);
    
    while (WiFi.status() == WL_NO_SHIELD) {
        delay(500);
    }
    
    delay(100);
    WiFi.beginAP("Meca");
    IPAddress ip(192, 168, 170, 1);
    WiFi.configAP(ip);

    delay(100);
    _server.begin();
}

void handleWiFi()
{
    WiFiEspClient client = _server.available();
    if (!client) return;

    _httpBuf.init();
    
    while (client.connected()) {
        if (!client.available()) continue;
        char c = client.read();
        _httpBuf.push(c);

        if (_httpBuf.endsWith("\r\n\r\n")) {
            _httpResp(client);
            break;
        }
    }

    delay(10);
    client.stop();
}
