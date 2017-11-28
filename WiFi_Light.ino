#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//------------------------------for local web
#include <ArduinoJson.h>
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
boolean light_status;
boolean sw_status;

void setup() {
  pinMode(pinout, OUTPUT);
  pinMode(pinsw, INPUT);
  Serial.begin(115200);
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
  webPage += "<center><font size='27'>ESP8266 Web Server Light Switch<p>Light #1 ";
  webPage += "<a href=\"/light/toggle\"><mark>TOGGLE it";
  webPage += "</mark></a></p></font></center>";

  if (mdns.begin("esp8266", WiFi.localIP())) 
    Serial.println("MDNS responder started");
 
  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  server.on("/light/toggle", [](){
    light_toggle();
    server.send(200, "text/html", webPage);
  });
  server.on("/light", HTTP_GET, [](){
    server.send(200, "text/json", response_json());
  });
  server.on("/light", HTTP_POST, [](){
    StaticJsonBuffer<200> request_buffer;
    JsonObject& request = request_buffer.parseObject(server.arg("plain"));
    boolean desired_light_state = request["active"];
    if (desired_light_state)
      light_on();
    else
      light_off();
    server.send(200, "text/json", response_json());
  });

  server.begin();
  //-------------------------------------------add your code here
}

String response_json() {
    String is_active = light_status ? "true" : "false";
    return "{\"is_active\": \"" + is_active + "\"}";
}

void light_toggle() {
    Serial.println("Toggling light...");
    if (light_status) 
      light_off();
    else
      light_on();
}

void light_off() {
    Serial.println("Turning light off");
    light_status = false;
    digitalWrite(pinout, LOW);
}

void light_on() {
    Serial.println("Turning light on");
    light_status = true;
    digitalWrite(pinout, HIGH);
}

void loop() {
  ArduinoOTA.handle();
  //-------------------------------------------add your code here
  server.handleClient();

  if(sw_status != digitalRead(pinsw))
  {
    sw_status = digitalRead(pinsw);
  }
  delay(200);
  
}
