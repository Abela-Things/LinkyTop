#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>

//#define DEBUG true
// Be sure to use this to print your debug messages. During normal use, the serial configuration is such that you won't be able to easily read it from your computer, even though that can be done with some tinkering.

#ifndef WIFI_SSID
#define WIFI_SSID "XXXXXXXXXXXXXXXXXXXXXXXX"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "XXXXXXXXXXXXXXXXXXXXXXXXXX"
#endif

#ifndef MQTT_SERVER
#define MQTT_SERVER MQTTserver(192, 168, 1, 1); //Replace with your mqtt server IP address
#endif

#ifndef DEFAULT_DELAY
#define DEFAULT_DELAY 30
#endif

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
char *API_URL = "http://usernale:password@serveraddress.com:1880/delay";
//The above string is to be completed with the correct username, password defined in the HttpNodeAuth setting of NodeRed.

char *FAST_DELAY = "fast";
char *SLOW_DELAY = "slow";
IPAddress MQTT_SERVER;
WiFiClient espClient;
PubSubClient mqtt_client(espClient);
int current_delay = DEFAULT_DELAY;

// Connect to Wifi and report IP address
void start_Wifi()
{
#ifdef DEBUG
  Serial.println("Starting Wifi");
#endif
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.begin(ssid, password);
  int counter = 0;
  while ((WiFi.status() != WL_CONNECTED) && (counter < 240)) {
    delay(500);
    counter ++;
#ifdef DEBUG
    Serial.print(".");
#endif
  }
  if (counter >= 240) {
#ifdef DEBUG
    Serial.println("Connection Failed! Rebooting...");
#endif
    ESP.restart();
  }
#ifdef DEBUG
  Serial.println("");
  Serial.println("WiFi connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

void sendTIC(String var_name, String var_value, bool res)
{
  if (res) {
    publish(var_name, var_value);
  } else {
    publish("ERROR", var_name);
  }
}

bool getValue(char *var_name, size_t sz, char *buff)
{
  if (Serial.find(var_name)) {
    Serial.readBytes(buff, sz);
    return true;
  }
  return false;
}

void publish(String topic, String payload)
{
  if (!mqtt_client.connected()) {
    mqtt_client.connect("arduinoClient");
#ifdef DEBUG
    Serial.println("MQTT Connect");
#endif
  }
  mqtt_client.publish(topic.c_str(), payload.c_str());
}

int shallISleep()
{
  HTTPClient http;
  if (http.begin(espClient, API_URL)) {  // HTTP
    int httpCode = http.GET();
    if (httpCode > 0) {
#ifdef DEBUG
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
#endif
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = http.getString();
#ifdef DEBUG
        Serial.println(payload);
#endif
        const char *delay_time = payload.c_str();
        if (!strncmp(delay_time, SLOW_DELAY, 4)) {
          int delay_value = atoi(delay_time + 5); //This is OK, because it is not an user input.
          return delay_value; 
        } else {
          return 0;
        }
      } else {
        return DEFAULT_DELAY;
      }
    } else {
      return DEFAULT_DELAY;
    }
    http.end();
  } else {
    return DEFAULT_DELAY;
  }
}

void setup()
{
#ifdef DEBUG
  Serial.begin(1200);
  Serial.println("starting");
#else
  Serial.begin(1200, SERIAL_7E1);
#endif
  Serial.setTimeout(2000);
  start_Wifi();
  mqtt_client.setServer(MQTTserver, 1883);
  publish("hello", "coucou from esp");
}

void loop()
{
  char hchc[10] = {0};
  bool hchc_res = getValue("HCHC ", 9, hchc);
  char hchp[10] = {0};
  bool hchp_res = getValue("HCHP ", 9, hchp);
  char ptec[3] = {0};
  bool ptec_res = getValue("PTEC ", 2, ptec);
  char iinst[4] = {0};
  bool iinst_res = getValue("IINST ", 3, iinst);
  char papp[6] = {0};
  bool papp_res = getValue("PAPP ", 5, papp);
  sendTIC("HCHC", hchc, hchc_res);
  sendTIC("HCHP", hchp, hchp_res);
  sendTIC("PTEC", ptec, ptec_res);
  sendTIC("IINST", iinst, iinst_res);
  sendTIC("PAPP", papp, papp_res);
  int delay_length = shallISleep();
  char sleep_time[32] = {0};
  if (delay_length != 0) {
    publish("delay", "slow");
    publish("sleep_time", itoa(delay_length, sleep_time, 10));
    delay(700); // It's necessary because the publish function seems to be asynchronous. Clearly not ideal because we're losing precious time that could be spent sleeping, need to find a way to wait for the messages to be sent.
    ESP.deepSleep(1e6 * delay_length);
  } else {
    publish("delay", "fast");
    Serial.flush();
  }
}
