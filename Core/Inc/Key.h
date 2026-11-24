#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx_hal.h"

/* 引脚集中定义，以后换脚只改这里 */
#define KEY_PORT      GPIOA
#define KEY_UP_PIN    GPIO_PIN_1
#define KEY_DOWN_PIN  GPIO_PIN_2

/* +1 按了加，-1 按了减，0 没动 */
int8_t Key_Scan(void);

#endif