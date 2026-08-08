/*
 * servo_ctrl.cpp
 *
 *  Created on: Aug 8, 2026
 *      Author: evelu
 */

#include "../Inc/servo_ctrl.hpp"

#define PCA9685_MODE1              0x00
#define PCA9685_MODE2              0x01

#define PCA9685_LED0_ON_L          0x06

#define PCA9685_PRESCALE           0xFE

#define PCA9685_MODE1_RESTART      0x80
#define PCA9685_MODE1_SLEEP        0x10
#define PCA9685_MODE1_AI           0x20

#define PCA9685_MODE2_OUTDRV       0x04

#define PCA9685_OSC_CLOCK          25000000UL
#define PCA9685_RESOLUTION         4096UL

#define PCA9685_SERVO_FREQ         50.0f
#define PCA9685_SERVO_PERIOD_US    20000UL

#define PCA9685_I2C_TIMEOUT        5U


PCA9685::PCA9685(
    I2C_HandleTypeDef *hi2c,
    uint8_t address
)
{
    _hi2c = hi2c;

    _address = address << 1;

    _initialized = false;
}

HAL_StatusTypeDef PCA9685::init(
    float frequency
)
{
    if (_hi2c == nullptr)
        return HAL_ERROR;

    HAL_StatusTypeDef status;

    status = write(
        PCA9685_MODE1,
        PCA9685_MODE1_AI
    );

    if (status != HAL_OK)
        return status;

    status = write(
        PCA9685_MODE2,
        PCA9685_MODE2_OUTDRV
    );

    if (status != HAL_OK)
        return status;

    status = setFrequency(
        frequency
    );

    if (status != HAL_OK)
        return status;

    _initialized = true;

    for (uint8_t ch = 0; ch < 16; ch++)
    {
        status = setChannelOff(ch);

        if (status != HAL_OK)
        {
            _initialized = false;
            return status;
        }
    }

    HAL_Delay(1);

    return HAL_OK;
}


HAL_StatusTypeDef PCA9685::write(
    uint8_t reg,
    uint8_t data
)
{
    return HAL_I2C_Mem_Write(
        _hi2c,
        _address,
        reg,
        I2C_MEMADD_SIZE_8BIT,
        &data,
        1,
        PCA9685_I2C_TIMEOUT
    );
}

HAL_StatusTypeDef PCA9685::setFrequency(
    float frequency
)
{
    if (frequency <= 0.0f)
        return HAL_ERROR;

    float prescaleValue =
        (PCA9685_OSC_CLOCK /
        (PCA9685_RESOLUTION * frequency))
        - 1.0f;

    if (prescaleValue < 3.0f)
        prescaleValue = 3.0f;

    if (prescaleValue > 255.0f)
        prescaleValue = 255.0f;

    uint8_t prescale =
        (uint8_t)(prescaleValue + 0.5f);

    uint8_t oldMode = 0;

    HAL_StatusTypeDef status =
        HAL_I2C_Mem_Read(
            _hi2c,
            _address,
            PCA9685_MODE1,
            I2C_MEMADD_SIZE_8BIT,
            &oldMode,
            1,
            PCA9685_I2C_TIMEOUT
        );

    if (status != HAL_OK)
        return status;

    uint8_t sleepMode =
        (oldMode & ~PCA9685_MODE1_RESTART)
        | PCA9685_MODE1_SLEEP;


    status = write(
        PCA9685_MODE1,
        sleepMode
    );

    if (status != HAL_OK)
        return status;

    status = write(
        PCA9685_PRESCALE,
        prescale
    );

    if (status != HAL_OK)
        return status;

    status = write(
        PCA9685_MODE1,
        oldMode
    );

    if (status != HAL_OK)
        return status;

    HAL_Delay(1);

    status = write(
        PCA9685_MODE1,
        oldMode | PCA9685_MODE1_RESTART
    );

    return status;
}

HAL_StatusTypeDef PCA9685::setPWM(
    uint8_t channel,
    uint16_t on,
    uint16_t off
)
{
    if (!_initialized)
        return HAL_ERROR;

    if (channel > 15)
        return HAL_ERROR;

    uint8_t data[4];

    data[0] = on & 0xFF;
    data[1] = (on >> 8) & 0x0F;

    data[2] = off & 0xFF;
    data[3] = (off >> 8) & 0x0F;

    uint8_t reg =
        PCA9685_LED0_ON_L
        + 4 * channel;

    return HAL_I2C_Mem_Write(
        _hi2c,
        _address,
        reg,
        I2C_MEMADD_SIZE_8BIT,
        data,
        4,
        PCA9685_I2C_TIMEOUT
    );
}

uint16_t PCA9685::pulseToCount(
    uint16_t pulse_us
)
{
    uint32_t count =
        ((uint32_t)pulse_us
        * PCA9685_RESOLUTION)
        / PCA9685_SERVO_PERIOD_US;

    if (count > 4095)
        count = 4095;

    return (uint16_t)count;
}

HAL_StatusTypeDef PCA9685::setPulse(
    uint8_t channel,
    uint16_t pulse_us
)
{
    uint16_t count =
        pulseToCount(pulse_us);

    return setPWM(
        channel,
        0,
        count
    );
}

HAL_StatusTypeDef PCA9685::setPulse(
    uint8_t channel,
    uint16_t pulse_us,
    uint16_t minPulse,
    uint16_t maxPulse
)
{
    if (pulse_us < minPulse)
        pulse_us = minPulse;

    if (pulse_us > maxPulse)
        pulse_us = maxPulse;

    return setPulse(
        channel,
        pulse_us
    );
}

HAL_StatusTypeDef PCA9685::setChannelOff(
    uint8_t channel
)
{
    if (!_initialized)
        return HAL_ERROR;

    if (channel > 15)
        return HAL_ERROR;

    uint8_t data[4] =
    {
        0x00,
        0x00,
        0x00,
        0x10
    };

    uint8_t reg =
        PCA9685_LED0_ON_L
        + 4 * channel;

    return HAL_I2C_Mem_Write(
        _hi2c,
        _address,
        reg,
        I2C_MEMADD_SIZE_8BIT,
        data,
        4,
        PCA9685_I2C_TIMEOUT
    );
}

ServoController::ServoController(
    PCA9685 *driver,
    uint8_t channel,
    uint16_t minPulse,
    uint16_t maxPulse
)
{
    _driver = driver;

    _channel = channel;

    _minPulse = minPulse;
    _maxPulse = maxPulse;

    _pulse = 0;

    _initialized = false;
}

void ServoController::init()
{
    if (_driver == nullptr) return;
    _initialized = true;
}

HAL_StatusTypeDef ServoController::setPulse(
    uint16_t pulse
)
{
    if (!_initialized)
        return HAL_ERROR;

    if (pulse < _minPulse)
        pulse = _minPulse;

    if (pulse > _maxPulse)
        pulse = _maxPulse;

    if (pulse == _pulse)
        return HAL_OK;

    HAL_StatusTypeDef status =
        _driver->setPulse(
            _channel,
            pulse
        );

    if (status == HAL_OK)
    {
        _pulse = pulse;
    }

    return status;
}

uint16_t ServoController::getPulse() const{
    return _pulse;
}

HAL_StatusTypeDef ServoController::stop(){
    if (!_initialized)
        return HAL_ERROR;


    return _driver->setChannelOff(
        _channel
    );
}

