#include <Arduino.h>
/*******************************************************************
*  An example of bot that echos back any messages received         *
*                                                                  *
*  written by Giacarlo Bacchio (Gianbacchio on Github)             *
*  adapted by Brian Lough                                          *
*******************************************************************/
#include <UniversalTelegramBot.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define _HEART_UTF8 "吉米\U00002764"
#define _ChatID "123xxxxxxxx"

// Initialize Telegram BOT
//#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000;  //mean time between scan messages
long Bot_lasttime;    //last time messages' scan has been done

const int ledPin = 2;
int ledStatus = 0;



//----------------------------------------------------------------------------
//Esp32Cam related
//
#include "esp_camera.h"

camera_fb_t* fb;
uint8_t* fb_buffer;
size_t fb_length;
int currentByte;

bool isMoreDataAvailable() {
    return (fb_length - currentByte);
}

uint8_t photoNextByte() {
    currentByte++;
    return (fb_buffer[currentByte - 1]);
}

void take_send_photo(String chat_id) {
    camera_fb_t* fb = NULL;
    fb = esp_camera_fb_get();
    currentByte = 0;
    fb_length = fb->len;
    fb_buffer = fb->buf;
    bot.sendPhotoByBinary(chat_id, "image/jpeg", fb->len, isMoreDataAvailable, photoNextByte, nullptr, nullptr);
    esp_camera_fb_return(fb);
    fb_length = NULL;
    fb_buffer = NULL;
}


//Telegram Bot HandleNewMessages-----------------------------------------------------
void handleNewMessages(int numNewMessages) {
    Serial.print("handleNewMessages:");
    Serial.println(String(numNewMessages));

    for (int i = 0; i < numNewMessages; i++) {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;

        String from_name = bot.messages[i].from_name;
        if (from_name == "") from_name = "Guest";

        if (text == "/ledon") {
            digitalWrite(ledPin, HIGH);  // turn the LED on (HIGH is the voltage level)
            ledStatus = 1;
            bot.sendMessage(chat_id, "Led is ON" _HEART_UTF8, "");
        }

        if (text == "/ledoff") {
            ledStatus = 0;
            digitalWrite(ledPin, LOW);  // turn the LED off (LOW is the voltage level)
            bot.sendMessage(chat_id, "Led is OFF", "");
        }

        if ( (text == "/jpg") || (text == "/jpeg") ){
            take_send_photo(chat_id);
            }

        if (text == "/status") {
            if (ledStatus) {
                bot.sendMessage(chat_id, "Status:Led is ON" _HEART_UTF8, "");
            } else {
                bot.sendMessage(chat_id, "Status:Led is OFF", "");
            }
        }

        if (text == "/start") {
            String welcome = "Welcome to Telegram Bot, " + from_name + _HEART_UTF8 + ".\n";
            welcome += "Available commands...\n\n";
            welcome += "/ledon  : Led ON\n";
            welcome += "/ledoff : Led OFF\n";
            welcome += "/jpg /jpeg: Takes photo\n";
            welcome += "/status : Returns status\n";
            bot.sendMessage(chat_id, welcome, "Markdown");
        }
    }
}