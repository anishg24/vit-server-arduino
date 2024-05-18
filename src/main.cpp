#include <Arduino.h>
#include <WiFi.h>
#include "CytronMotorController.h"
#include "VITServer.h"

void initWiFi() {
    WiFiClass::mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi...");
    while (WiFiClass::status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

cytron_motor_controller_config_t mc_cfg {
    .an1_pin = AN1,
    .in1_pin = IN1,
    .toggle_pin = MANUAL_SWITCH,
    .pot_pin = SPEED_CONTROL,
    .dir_pin = DIRECTION_SWITCH,
    .resolution = RESOLUTION_BITS,
    .frequency = PWM_FREQUENCY,
};
CytronMotorController controller(&mc_cfg);
VITServer server(controller);

void setup() {
    Serial.begin(115200);
    initWiFi();

    server.begin();
}

void loop() {
    controller.loop();
    server.loop();
}