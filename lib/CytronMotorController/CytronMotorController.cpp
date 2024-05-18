//
// Created by anish on 5/16/24.
//

#include "CytronMotorController.h"

CytronMotorController::CytronMotorController(cytron_motor_controller_config_t *cfg) {
    this->cfg = cfg;
    this->server_controlled = false;

    pinMode(this->cfg->an1_pin, OUTPUT);
    pinMode(this->cfg->in1_pin, OUTPUT);

    pinMode(this->cfg->toggle_pin, INPUT);
    pinMode(this->cfg->pot_pin, INPUT);
    pinMode(this->cfg->dir_pin, INPUT);

    analogWriteResolution(this->cfg->resolution);
    analogWriteFrequency(this->cfg->frequency);

    Serial.println("Successfully initialized motor controller");
}

// 0 -> success
// 1 -> requested speed too high
int CytronMotorController::rotate(cytron_motor_controller_output_t *output) {
    if (output->speed > 255 || output->speed < 0) return 1;

    analogWrite(this->cfg->an1_pin, output->speed);
    digitalWrite(this->cfg->in1_pin, output->direction);

    return 0;
}

int CytronMotorController::rotate() {
    if (this->manual_control()) {
        if (this->server_controlled) {
            Serial.println("Switched to manual output control!");
            this->stop();
            this->server_controlled = false;
        }
        this->manual_rotate();
        return this->rotate(&this->manual_output);
    } else {
        if (!this->server_controlled) {
            Serial.println("Switched to server output control!");
            this->stop();
            this->server_controlled = true;
        }
        return 0;
    }
}

int CytronMotorController::stop() {
    this->server_output.speed = 0;
    this->manual_output.speed = 0;
    analogWrite(this->cfg->an1_pin, 0);
    return 0;
}

bool CytronMotorController::manual_control() {
    return !digitalRead(this->cfg->toggle_pin);
}

int CytronMotorController::server_rotate(int direction, uint8_t speed) {
    this->server_output.speed = speed;
    this->server_output.direction = direction;
    Serial.printf("Started rotating motor in direction %d at %d%% speed\n", direction, speed);
    this->rotate(&this->server_output);
    return 0;
}

int CytronMotorController::manual_rotate() {
    uint16_t pot_reading = analogRead(this->cfg->pot_pin);
    pot_reading = pot_reading >> 4; // The pin gives 12 bits of precision, we only want 8.
    Serial.println(pot_reading);
    this->manual_output.speed = pot_reading;
    this->manual_output.direction = digitalRead(this->cfg->dir_pin);
    return 0;
}

void CytronMotorController::loop() {
    this->rotate();
}
