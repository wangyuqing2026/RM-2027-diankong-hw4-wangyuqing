#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f1xx_hal.h"

/*
 * SG90 (180 度型) 的脉宽范围。
 * 标称 0 度 = 0.5 ms、180 度 = 2.5 ms，但真实舵机行程比标称窄，
 * 打满会顶到机械限位，堵转发热甚至扫齿。
 * 两端各留一点余量：0.55 ms ~ 2.45 ms，对绝大多数 SG90 都安全。
 */
#define SERVO_PULSE_MIN_US   550.0f
#define SERVO_PULSE_MAX_US   2450.0f

uint16_t AngleToCCR(float angle);
void Servo_SetAngle(float angle);

#endif
