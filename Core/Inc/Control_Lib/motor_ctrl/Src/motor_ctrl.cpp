#include "../Inc/motor_ctrl.hpp"
#include "math.h"
#include "motor_config.h"
int times = 0;
uint32_t current_cnt;
float v = 0.5;
float e = 1.0;

JointMotor_polulu::JointMotor_polulu(
TIM_HandleTypeDef *enc,
TIM_HandleTypeDef *pwm,
uint32_t channel,
GPIO_TypeDef *dirPort,
uint16_t dirPin)
{
    _enc=enc;
    _pwm=pwm;
    _channel=channel;

    _dirPort=dirPort;
    _dirPin=dirPin;
}

void JointMotor_polulu::init()
{
    HAL_TIM_Encoder_Start(_enc,TIM_CHANNEL_ALL);
    HAL_TIM_PWM_Start(_pwm,_channel);

    stop();

    __HAL_TIM_SET_COUNTER(_enc,0);

//    _zeroCount=0;
    _absCount = 0;
    _prevCount = 0;
}

void JointMotor_polulu::stop()
{
    __HAL_TIM_SET_COMPARE(_pwm,_channel,0);
}

void JointMotor_polulu::zero()
{
//    _zeroCount=__HAL_TIM_GET_COUNTER(_enc);
	_absCount = 0;
	_prevCount = __HAL_TIM_GET_COUNTER(_enc);
}

int32_t JointMotor_polulu::getCount()
{
//    return (int32_t)__HAL_TIM_GET_COUNTER(_enc)-_zeroCount;
	uint32_t currentCount = __HAL_TIM_GET_COUNTER(_enc);
	    if (_enc->Instance == TIM2 || _enc->Instance == TIM5) {
	        int32_t delta = (int32_t)(currentCount - _prevCount);
	        _absCount += delta;
	    } else {
	        int16_t delta = (int16_t)(currentCount - _prevCount);
	        _absCount += delta;
	    }
	    _prevCount = currentCount;
	    return _absCount;
}

float JointMotor_polulu::getAngle()
{
    return getCount()*360.0f/(ARM_ENCODER_CPR * ARM_RATIO);
}

void JointMotor_polulu::setPWM(int pwm)
{
    if(pwm>1000)pwm=1000;
    if(pwm<-1000)pwm=-1000;

    if(pwm>=0)
    {
        HAL_GPIO_WritePin(_dirPort,_dirPin,GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(_pwm,_channel,pwm);
    }
    else
    {
        HAL_GPIO_WritePin(_dirPort,_dirPin,GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(_pwm,_channel,-pwm);
    }
}

void JointMotor_polulu::setTarget(float target)
{
    _targetDeg = target;
}

void JointMotor_polulu::update()
{
//    float now = getAngle();
//    float err = _targetDeg - now;
//    if(fabs(err) < 0.5f)
//    {
//        stop();
//        _integral = 0;
//        return;
//    }
//    _integral += err;
//    float derivative = err - _lastError;
//    float pwm = _kp * err + _ki * _integral + _kd * derivative;
//    setPWM((int)pwm);
//    _lastError = err;
    float err = _targetDeg - getAngle();
    if(fabs(err) < 1.0f)
    {
        stop();
        return;
    }
    if(err > 0)
        setPWM(1000);
    else
        setPWM(-1000);
}

JointMotor_vnh::JointMotor_vnh(
TIM_HandleTypeDef *enc,
TIM_HandleTypeDef *pwm,
uint32_t channel,
GPIO_TypeDef *dirPortA,
uint16_t dirPinA,
GPIO_TypeDef *dirPortB,
uint16_t dirPinB
)
{
    _enc=enc;
    _pwm=pwm;
    _channel=channel;

    _portA=dirPortA;
    _pinA=dirPinA;

    _portB=dirPortB;
    _pinB=dirPinB;
}

void JointMotor_vnh::init()
{
    HAL_TIM_Encoder_Start(_enc,TIM_CHANNEL_ALL);
    HAL_TIM_PWM_Start(_pwm,_channel);

    stop();

    __HAL_TIM_SET_COUNTER(_enc,0);

//    _zeroCount=0;
    _absCount = 0;
    _prevCount = 0;
}

void JointMotor_vnh::stop()
{
    HAL_GPIO_WritePin(_portA,_pinA,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(_portB,_pinB,GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(_pwm,_channel,0);
}

void JointMotor_vnh::zero()
{
//    _zeroCount=__HAL_TIM_GET_COUNTER(_enc);
	_absCount = 0;
	_prevCount = __HAL_TIM_GET_COUNTER(_enc);
}

int32_t JointMotor_vnh::getCount()
{
//    return (int32_t)__HAL_TIM_GET_COUNTER(_enc)-_zeroCount;
	uint32_t currentCount = __HAL_TIM_GET_COUNTER(_enc);
	    if (_enc->Instance == TIM2 || _enc->Instance == TIM5) {
	        int32_t delta = (int32_t)(currentCount - _prevCount);
	        _absCount += delta;
	    } else {
	        int16_t delta = (int16_t)(currentCount - _prevCount);
	        _absCount += delta;
	    }
	    _prevCount = currentCount;
	    return _absCount;
}

float JointMotor_vnh::getAngle()
{
    return getCount()*360.0f/(ARM_ENCODER_CPR * ARM_RATIO);
}

void JointMotor_vnh::setPWM(int pwm)
{
    if(pwm>1000)pwm=1000;
    if(pwm<-1000)pwm=-1000;

    if(pwm>=0)
    {
        HAL_GPIO_WritePin(_portA,_pinA,GPIO_PIN_SET);
        HAL_GPIO_WritePin(_portB,_pinB,GPIO_PIN_RESET);

        __HAL_TIM_SET_COMPARE(_pwm,_channel,pwm);
    }
    else
    {
        HAL_GPIO_WritePin(_portA,_pinA,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(_portB,_pinB,GPIO_PIN_SET);

        __HAL_TIM_SET_COMPARE(_pwm,_channel,-pwm);
    }
}

void JointMotor_vnh::setTarget(float target)
{
    _targetDeg = target;
}

void JointMotor_vnh::update()
{
//    float now = getAngle();
//    float err = _targetDeg - now;
//    if(fabs(err) < 0.5f)
//    {
//        stop();
//        _integral = 0;
//        return;
//    }
//    _integral += err;
//    float derivative = err - _lastError;
//    float pwm = _kp * err + _ki * _integral + _kd * derivative;
//    setPWM((int)pwm);
//    _lastError = err;
    float err = _targetDeg - getAngle();
    if(fabs(err) < 1.0f)
    {
        stop();
        return;
    }
    if(err > 0)
        setPWM(1000);
    else
        setPWM(-1000);
}
