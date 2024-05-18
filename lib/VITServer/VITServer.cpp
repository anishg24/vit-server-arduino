//
// Created by anish on 5/17/24.
//

#include "VITServer.h"

VITServer::VITServer(CytronMotorController controller) :
mc(controller), server(80), numSentences(0), sentenceDifficulty(0), defaultDuration(0) {
    VITServer::initFS();
    VITServer::initSentences();
    VITServer::initRoutes();

    pinMode(RANDOM_PIN, INPUT);
//    randomSeed(analogRead(RANDOM_PIN));
    Serial.println("Successfully started VTI Server");
}

void VITServer::begin() {
    server.begin();
}

void VITServer::initFS() {
    if (!SPIFFS.begin()) {
        Serial.println("An error has occurred while mounting SPIFFS");
    } else {
        Serial.println("SPIFFS mounted successfully");
    }
}

void VITServer::getSentence() {
    long random_line = random(1, this->numSentences+1);
    File sentences = SPIFFS.open("/sentences.csv", "r");
    for (int i = 0; i < random_line; i++) {
        sentences.readStringUntil('\n');
    }
    String line = sentences.readStringUntil('\n');
    sentences.close();

    Serial.println(line);
    int l;
    for (l = (int) line.length() - 1; l >= 0; l--) {
        if (line[l] == ',') {
            break;
        }
    }
    Serial.println(l);

    if (l <= 2 || l == line.length() - 1) return;

    this->sentence = line.substring(1, l-2);
    this->sentenceDifficulty = line.substring(l+1, line.length()-1).toInt();
}

void VITServer::initRoutes() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html", false);
    });

    server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/just_spin.html", "text/html", false);
    });

    server.serveStatic("/", SPIFFS, "/");

    server.on("/api/sentence", HTTP_GET, [this](AsyncWebServerRequest *request) {
        this->getSentence();
        Serial.println(this->sentence);
        request->send(200, "text/plain", this->sentence);
    });

    server.on("/api/rotate", HTTP_POST, [this](AsyncWebServerRequest *request) {
       int speed = request->getHeader("MOTOR_SPEED")->value().toInt();
       int direction = request->getHeader("MOTOR_DIRECTION")->value().toInt();
       if (request->hasHeader("MOTOR_DURATION")) {
           int duration = request->getHeader("MOTOR_DURATION")->value().toInt();
           if (duration > 0 && duration < 300) {
               this->reward(direction, speed, duration * 1000);
           }
       } else {
           this->mc.server_rotate(direction, speed);
       }
       request->send(200);
    });

    server.on("/api/reward", HTTP_POST, [this](AsyncWebServerRequest *request) {
        // Change these values based on some admin settings
        this->reward(LEFT, 40, 10000);
        request->send(200);
    });

    server.on("/api/stop", HTTP_POST, [this](AsyncWebServerRequest *request) {
        this->mc.stop();
        timer.cancel();
        request->send(200);
    });
}

void VITServer::initSentences() {
    File sentences = SPIFFS.open("/sentences.csv", "r");
    if (!sentences) {
        Serial.println("Failed to open sentences.csv for reading");
        return;
    }

    int num_lines = 0;
    while(sentences.available()) {
        num_lines++;
        sentences.readStringUntil('\n');
    }
    sentences.close();
    Serial.printf("Counted %d sentences!", num_lines);

    this->numSentences = num_lines;
}

bool VITServer::endReward() {
    this->mc.stop();
    Serial.println("Ended reward");
    return false;
}

bool VITServer::endRewardCallback(void *argument) {
    auto *server = static_cast<VITServer*>(argument);
    return server->endReward();
}

void VITServer::reward(int direction, uint8_t speed, unsigned long duration) {
    this->mc.server_rotate(direction, speed);
    Serial.println("Started reward");
    timer.in(duration, VITServer::endRewardCallback, this);
}

void VITServer::loop() {
    timer.tick();
}
