#include <WiFi.h>
#include "webpage.h" // <-- Include your webpage

const char *ssid = "wisperPxl";
const char *password = "123456789";

WiFiServer server(80);

// Define output pins
const int out_circ_grande_1 = 4;

// Define output states
String color_circ_grande_1 = COLOR_CIRC_APAGADO;

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

String header;

String processor(String page)
{
  // Defining colors
  page.replace("%color_circ_grande_1%", color_circ_grande_1);

  // Defining classes
  page.replace("%clase_boton_heli%", clase_boton_heli);

  // Defining urls
  page.replace("%url_circ_grande_1%", color_circ_grande_1 == COLOR_CIRC_APAGADO ? "/circ_grande_1/on" : "/circ_grande_1/off");

  return page;
}

void setup()
{
  Serial.begin(115200);

  pinMode(out_circ_grande_1, OUTPUT);

  Serial.print("Pins set as output");

  digitalWrite(out_circ_grande_1, LOW);
  

  digitalWrite(out_letra_h, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop()
{
  WiFiClient client = server.available();

  if (client)
  {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    String currentLine = "";

    while (client.connected() && currentTime - previousTime <= timeoutTime)
    {
      currentTime = millis();

      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n')
        {
          if (currentLine.length() == 0)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Handle button presses
            if (header.indexOf("GET /circ_grande_1/on") >= 0)
            {
              Serial.println("url circ_grande_1 on");
              color_circ_grande_1 = COLOR_CIRC_ENCENDIDO;
              digitalWrite(out_circ_grande_1, HIGH);
            }
            else if (header.indexOf("GET /circ_grande_1/off") >= 0)
            {
              Serial.println("url circ_grande_1 off");
              color_circ_grande_1 = COLOR_CIRC_APAGADO;
              digitalWrite(out_circ_grande_1, LOW);
            }
            

            client.println(processor(String(MAIN_page)));
            break;
          }
          else
          {
            currentLine = "";
          }
        }
        else if (c != '\r')
        {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
