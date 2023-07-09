/**
 * @file servo.h
 * @author Shiki
 * @brief 双路舵机控制模块，适用于舵机云台
 * @version 1.0
 * @date 2023-07-09
 *
 * @note 用于MSP432P401R LaunchPad开发板
 */
#ifndef __SERVO_H__
#define __SERVO_H__

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// Double Servo control group
typedef struct {
    uint16_t freq;              // PWM frequency
    uint16_t ACompensation;     // PWM ccr for 0 degree azimuth
    uint16_t ECompensation;     // PWM ccr for 0 degree elevation
    float AMultiplier;          // PWM ccr multiplier for azimuth
    float EMultiplier;          // PWM ccr multiplier for elevation
    uint32_t TimBase;           // PWM timer base
    uint32_t ATimCh;            // PWM timer channel for azimuth
    uint32_t ETimCh;            // PWM timer channel for elevation
} ServoControl2;

ServoControl2 *ServoControl2_init(uint16_t freq, uint32_t TimBase, uint32_t ACh, uint32_t ECh);
void ServoControl2_setAzimuth(ServoControl2 *sc, float angle);
void ServoControl2_setElevation(ServoControl2 *sc, float angle);
void ServoControl2_calibrate0(ServoControl2 *sc);
void ServoControl2_calibrate90(ServoControl2 *sc);
void ServoControl2_setParams(ServoControl2 *sc, uint16_t ac, uint16_t ec, float am, float em);

#endif
