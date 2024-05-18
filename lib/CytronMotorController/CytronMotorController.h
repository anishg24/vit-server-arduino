//
// Created by anish on 5/16/24.
//

#ifndef ESP32_PWM_WEBSERVER_ARDUINO_CYTRONMOTORCONTROLLER_H
#define ESP32_PWM_WEBSERVER_ARDUINO_CYTRONMOTORCONTROLLER_H

#include "Arduino.h"

enum {
    LEFT,
    RIGHT
};

struct cytron_motor_controller_config_t {
    uint8_t an1_pin;
    uint8_t in1_pin;
    uint8_t toggle_pin;
    uint8_t pot_pin;
    uint8_t dir_pin;
    uint8_t resolution;
    int frequency;
};

struct cytron_motor_controller_output_t {
    uint8_t speed;
    int direction;
};

class CytronMotorController {
public:
    explicit CytronMotorController(cytron_motor_controller_config_t *cfg);
    int stop();
    int manual_rotate();
    int server_rotate(int direction, uint8_t speed);
    bool manual_control();
    void loop();

private:
    cytron_motor_controller_output_t server_output;
    cytron_motor_controller_output_t manual_output;
    bool server_controlled;
    cytron_motor_controller_config_t *cfg;

    int rotate(cytron_motor_controller_output_t *output);
    int rotate();
};


#endif //ESP32_PWM_WEBSERVER_ARDUINO_CYTRONMOTORCONTROLLER_H
