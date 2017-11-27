#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//------------------------------for local web
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

MDNSResponder mdns;
ESP8266WebServer server(80);
String webPage;
//----------------------------for local web

const char* ssid = "ssid";
const char* password = "password";
int pinout=2;  //for relay, and internal led
int pinsw=0;  //for switch
boolean light_change;
boolean sw_status;

void setup() {
  pinMode(pinout, OUTPUT);
  pinMode(pinsw, INPUT);
  Serial.begin(74880);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //-----------------------------------------add your code here
  webPage += "<center><font size='27'>ESP8266 Web Server<p>Light #1 ";
  webPage += "<a href=\"toggle_light\"><mark>TOGGLE it";
  webPage += "</mark></a></p></font></center>";

  if (mdns.begin("esp8266", WiFi.localIP())) 
    Serial.println("MDNS responder started");
 
  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  server.on("/toggle_light", [](){
    server.send(200, "text/html", webPage);
    // Change status
    light_change = true;
    delay(100);
  });

  server.begin();
  //-------------------------------------------add your code here
}


void loop() {
  ArduinoOTA.handle();
  //-------------------------------------------add your code here
  server.handleClient();
  if(light_change)
  {
    light_change = false;
    if(digitalRead(pinout)) digitalWrite(pinout,LOW);
    else digitalWrite(pinout,HIGH);
  }

  if(sw_status != digitalRead(pinsw))
  {
    sw_status = digitalRead(pinsw);
    light_change = true;
  }
  delay(200);
  
}
