#include <WiFi.h>
#include "webpage.h" // <-- Include your webpage

const char *ssid = "welim";
const char *password = "wilywily";

WiFiServer server(80);

// Define output pins
const int out_led_delantero = 4;
const int out_led_misil = 5;
const int out_rotar_helice = 6; 

// Define output states
String color_led_delantero = "#f0f0f080";
String color_led_misil = "#ffff0080";
String clase_rotar_helice = "";

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

String header;

String processor(String page)
{
  // Defining colors
  page.replace("%color_led_delantero%", color_led_delantero);
  page.replace("%color_led_misil%", color_led_misil);
  
  // Defining classes
  page.replace("%rotar_helice%", clase_rotar_helice);

  // Defining urls
  page.replace("%url_led_delantero%", color_led_delantero == "#f0f0f080" ? "/led_delantero/on" : "/led_delantero/off");
  page.replace("%url_led_misil%", color_led_misil == "#ffff0080" ? "/led_misil/on" : "/led_misil/off");
  page.replace("%url_rotar_helice%", clase_rotar_helice == "" ? "/rotar_helice/on" : "/rotar_helice/off");

  return page;
}

void setup()
{
  Serial.begin(115200);

  pinMode(out_led_delantero, OUTPUT);
  pinMode(out_led_misil, OUTPUT);
  pinMode(out_rotar_helice, OUTPUT);

  Serial.print("Pins set as output");

  digitalWrite(out_led_delantero, LOW);
  digitalWrite(out_led_misil, LOW);
  digitalWrite(out_rotar_helice, LOW);
  
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
            if (header.indexOf("GET /led_delantero/on") >= 0)
            {
              Serial.println("url led_delantero on");
              color_led_delantero = "#f0f0f0";
              digitalWrite(out_led_delantero, HIGH);
            }
            else if (header.indexOf("GET /led_delantero/off") >= 0)
            {
              Serial.println("url led_delantero off");
              color_led_delantero = "#f0f0f080";
              digitalWrite(out_led_delantero, LOW);
            }
            else if (header.indexOf("GET /led_misil/on") >= 0)
            {
              Serial.println("url led_misil on");
              color_led_misil = "#ffff00";
              digitalWrite(out_led_misil, HIGH);
            }
            else if (header.indexOf("GET /led_misil/off") >= 0)
            {
              Serial.println("url led_misil off");
              color_led_misil = "#ffff0080";
              digitalWrite(out_led_misil, LOW);
            }
            else if (header.indexOf("GET /rotar_helice/on") >= 0)
            {
              Serial.println("url rotar_helice on");
              clase_rotar_helice = "rotar";
              digitalWrite(out_rotar_helice, HIGH);
            }
            else if (header.indexOf("GET /rotar_helice/off") >= 0)
            {
              Serial.println("url rotar_helice off");
              clase_rotar_helice = "";
              digitalWrite(out_rotar_helice, LOW);
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
