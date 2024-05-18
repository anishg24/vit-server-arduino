//
// Created by anish on 5/17/24.
//

#ifndef ESP32_PWM_WEBSERVER_ARDUINO_VITSERVER_H
#define ESP32_PWM_WEBSERVER_ARDUINO_VITSERVER_H
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <arduino-timer.h>
#include "CytronMotorController.h"
#include "SPIFFS.h"

#define RANDOM_PIN 7

class VITServer {
public:
    explicit VITServer(CytronMotorController controller);
    void begin();
    void loop();

private:
    CytronMotorController mc;
    AsyncWebServer server;
    int numSentences;
    String sentence;
    int sentenceDifficulty;
    int defaultDuration;
    Timer<1> timer;

    static void initFS();
    void initSentences();
    void initRoutes();
    void getSentence();
    bool endReward();
    static bool endRewardCallback(void *argument);
    void reward(int direction, uint8_t speed, unsigned long duration);
};


#endif //ESP32_PWM_WEBSERVER_ARDUINO_VITSERVER_H
