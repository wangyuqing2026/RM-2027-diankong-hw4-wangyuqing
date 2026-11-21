#include "Servo.h"

/* htim2 的定义在 Core/Src/main.c 里（CubeMX 生成的全局变量） */
extern TIM_HandleTypeDef htim2;

/**
  * @brief  角度 -> 比较寄存器 CCR 的值
  * @param  angle 目标角度，单位度
  * @retval CCR 值；因为 TIM2 已被配置成 1 个计数 = 1 us，所以这个值就是脉宽（us）
  *
  * 换算原理：
  *   0 度   -> 550 us
  *   180 度 -> 2450 us
  *   线性插值：ccr = MIN + angle / 180 * (MAX - MIN)
  *   90 度代进去 = 550 + 95/... 即 550 + 950 = 1500 us（正好是标准中位）
  */
uint16_t AngleToCCR(float angle)
{
    /* 限幅：角度超出 0~180 容易被舵机内部程序当成非法值，必须先夹住 */
    if (angle < 0.0f)
    {
        angle = 0.0f;
    }
    if (angle > 180.0f)
    {
        angle = 180.0f;
    }

    return (uint16_t)(SERVO_PULSE_MIN_US
                      + angle / 180.0f * (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US));
}

/**
  * @brief  设置舵机角度
  * @param  angle 目标角度，单位度
  * @retval 无
  */
void Servo_SetAngle(float angle)
{
    uint16_t ccr = AngleToCCR(angle);

    /* 只改比较寄存器，PWM 波形由硬件自动生成，CPU 不用管 */
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ccr);
}
