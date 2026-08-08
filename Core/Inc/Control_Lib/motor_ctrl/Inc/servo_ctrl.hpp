/*
 * servo_ctrl.hpp
 *
 *  Created on: Aug 8, 2026
 *      Author: evelu
 */

#ifndef SERVO_CTRL_HPP
#define SERVO_CTRL_HPP

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

class PCA9685
{
public:

    PCA9685(
        I2C_HandleTypeDef *hi2c,
        uint8_t address = 0x40
    );

    HAL_StatusTypeDef init(
        float frequency = 50.0f
    );

    HAL_StatusTypeDef setPWM(
        uint8_t channel,
        uint16_t on,
        uint16_t off
    );

    HAL_StatusTypeDef setPulse(
        uint8_t channel,
        uint16_t pulse_us
    );

    HAL_StatusTypeDef setPulse(
        uint8_t channel,
        uint16_t pulse_us,
        uint16_t minPulse,
        uint16_t maxPulse
    );

    HAL_StatusTypeDef setChannelOff(
        uint8_t channel
    );


private:

    HAL_StatusTypeDef write(
        uint8_t reg,
        uint8_t data
    );

    HAL_StatusTypeDef setFrequency(
        float frequency
    );

    uint16_t pulseToCount(
        uint16_t pulse_us
    );


private:

    I2C_HandleTypeDef *_hi2c;

    uint8_t _address;

    bool _initialized;
};


class ServoController
{
public:

    ServoController(
        PCA9685 *driver,
        uint8_t channel,
        uint16_t minPulse = 500,
        uint16_t maxPulse = 2500
    );

    void init();

    HAL_StatusTypeDef setPulse(
        uint16_t pulse
    );

    uint16_t getPulse() const;

    HAL_StatusTypeDef stop();


private:

    PCA9685 *_driver;

    uint8_t _channel;

    uint16_t _minPulse;
    uint16_t _maxPulse;

    uint16_t _pulse;

    bool _initialized;
};


#endif
