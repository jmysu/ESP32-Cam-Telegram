#include <Arduino.h>
#include <WiFi.h>

#include "esp_camera.h"
#include <UniversalTelegramBot.h>


const char* ssid = CONFIG_WIFI_SSID;
const char* password = CONFIG_WIFI_PASSWORD;

void startCameraServer();
void setupEsp32Cam();

extern const int ledPin = 2;
extern int ledStatus;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  setupEsp32Cam();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
  log_i("Streaming with http://%s:9601/stream", WiFi.localIP().toString().c_str());
  

  pinMode(ledPin, OUTPUT);  // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, HIGH);  // initialize pin as off
  ledStatus = 1;
}

extern UniversalTelegramBot bot;
extern int Bot_mtbs;  //mean time between scan messages
extern long Bot_lasttime;    //last time messages' scan has been done
extern void handleNewMessages(int numNewMessages);
void loop() {
  // put your main code here, to run repeatedly:
  
    //delay(10000);
    if (millis() > Bot_lasttime + Bot_mtbs) {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while (numNewMessages) {
            //Serial.println("got response:");
            //for (int i=0; i<numNewMessages; i++) {
            //  String msg = bot.messages[i].text +"\U00002764";
            //  bot.sendMessage(bot.messages[i].chat_id, msg , "");
            //  Serial.println(msg);
            //  }
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
            }
        Bot_lasttime = millis();
    }
}