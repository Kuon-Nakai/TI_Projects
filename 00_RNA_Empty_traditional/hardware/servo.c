/**
 * @file servo.c
 * @author Shiki
 * @brief 双路舵机控制模块，适用于舵机云台
 * @version 1.0
 * @date 2023-07-09
 * 
 * @note 用于MSP432P401R LaunchPad开发板
 */
#include "servo.h"
#include <stdlib.h>
#include <stdio.h>

#define DEBUG 0

/**
 * @brief Initialize the servo control group
 * 
 * @param freq      PWM frequency of the servos
 * @param TimBase   PWM timer base for both PWM generations
 * @param ACh       PWM timer channel for azimuth
 * @param ECh       PWM timer channel for elevation
 * @return ServoControl2 type struct
 */
ServoControl2 *ServoControl2_init(uint16_t freq, uint32_t TimBase, uint32_t ACh, uint32_t ECh){
    ServoControl2 *sc = (ServoControl2 *)malloc(sizeof(ServoControl2));
    sc->freq = freq;
    sc->TimBase = TimBase;
    sc->ATimCh = ACh;
    sc->ETimCh = ECh;
    sc->ACompensation = 0;
    sc->ECompensation = 0;
    sc->AMultiplier = 1;
    sc->EMultiplier = 1;

    // Start PWM output
    Timer_A_PWMConfig AConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,      // @48MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_48, // -> 1MHz
        (int)(1000000.0 / freq),
        ACh,
        TIMER_A_OUTPUTMODE_TOGGLE_SET,
        (int)(100000.0 / freq) // Default to 10% duty cycle
    };
    Timer_A_PWMConfig EConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,      // @48MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_48, // -> 1MHz
        (int)(1000000.0 / freq),
        ECh,
        TIMER_A_OUTPUTMODE_TOGGLE_SET,
        (int)(100000.0 / freq) // Default to 10% duty cycle
    };
    Timer_A_generatePWM(TimBase, &AConfig);
    Timer_A_generatePWM(TimBase, &EConfig);
    printf("Servo2> Servo 2 Control Group initialized & running.\n");
    return sc;
}

/**
 * @brief Sets the azimuth angle by manipulating servo No. 1.
 * 
 * @param sc    ServoControl2 type struct
 * @param angle Desired azimuth angle
 */
void ServoControl2_setAzimuth(ServoControl2 *sc, float angle){
    uint16_t ccr = (uint16_t)(sc->ACompensation + sc->AMultiplier * angle);
    Timer_A_setCompareValue(sc->TimBase, sc->ATimCh, ccr);
#if DEBUG
    printf("Azimuth: %f\n", angle);
    printf("Azimuth CCR: %d\n", ccr);
#endif
}

/**
 * @brief Sets the elevation angle by manipulating servo No. 2.
 * 
 * @param sc    ServoControl2 type struct
 * @param angle Desired elevation angle
 */
void ServoControl2_setElevation(ServoControl2 *sc, float angle){
    uint16_t ccr = (uint16_t)(sc->ECompensation + sc->EMultiplier * angle);
    Timer_A_setCompareValue(sc->TimBase, sc->ETimCh, ccr);
#if DEBUG
    printf("Elevation: %f\n", angle);
    printf("Elevation CCR: %d\n", ccr);
#endif
}

/**
 * @brief Sets the azimuth and elevation compensation values. Used for calibration.
 * 
 * @param sc ServoControl2 type struct
 */
void ServoControl2_calibrate0(ServoControl2 *sc){
    sc->ACompensation = Timer_A_getCaptureCompareCount(sc->TimBase, sc->ATimCh);
    sc->ECompensation = Timer_A_getCaptureCompareCount(sc->TimBase, sc->ETimCh);
    printf("Servo 2 Control Group Calibrated to 0deg:\n");
    printf("Azimuth Compensation: %d\n", sc->ACompensation);
    printf("Elevation Compensation: %d\n", sc->ECompensation);
}

/**
 * @brief Sets the azimuth and elevation multiplier values. Used for calibration.
 * 
 * @param sc ServoControl2 type struct
 */
void ServoControl2_calibrate90(ServoControl2 *sc){
    register uint16_t ccr = Timer_A_getCaptureCompareCount(sc->TimBase, sc->ATimCh);
    sc->AMultiplier = (ccr - sc->ACompensation) / 90.0;
    ccr = Timer_A_getCaptureCompareCount(sc->TimBase, sc->ETimCh);
    sc->EMultiplier = (ccr - sc->ECompensation) / 90.0;
    printf("Servo 2 Control Group Calibrated to 90deg:\n");
    printf("Azimuth Multiplier: %f\n", sc->AMultiplier);
    printf("Elevation Multiplier: %f\n", sc->EMultiplier);
}

/**
 * @brief Manually set the compensation and multiplier values. Used for automatic calibration.
 * 
 * @param sc ServoControl2 type struct
 * @param ac Azimuth compensation
 * @param ec Elevation compensation
 * @param am Azimuth multiplier
 * @param em Elevation multiplier
 */
void ServoControl2_setParams(ServoControl2 *sc, uint16_t ac, uint16_t ec, float am, float em){
    sc->ACompensation = ac;
    sc->ECompensation = ec;
    sc->AMultiplier = am;
    sc->EMultiplier = em;
}

