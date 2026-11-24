#include "Key.h"

/**
  * @brief  扫描两个按键，20ms 消抖
  * @retval +1 加键，-1 减键，0 无动作
  *
  * 上拉输入：松开读到 1，按下读到 0。
  * 只在 1 -> 0 这一下触发，按住不放只加一次。
  */
int8_t Key_Scan(void)
{
    static uint8_t up_prev = 1;
    static uint8_t dn_prev = 1;

    uint8_t up = HAL_GPIO_ReadPin(KEY_PORT, KEY_UP_PIN);
    uint8_t dn = HAL_GPIO_ReadPin(KEY_PORT, KEY_DOWN_PIN);

    int8_t ret = 0;

    if (up_prev == 1 && up == 0)
    {
        HAL_Delay(20);
        if (HAL_GPIO_ReadPin(KEY_PORT, KEY_UP_PIN) == 0)
        {
            ret = +1;
        }
    }
    else if (dn_prev == 1 && dn == 0)
    {
        HAL_Delay(20);
        if (HAL_GPIO_ReadPin(KEY_PORT, KEY_DOWN_PIN) == 0)
        {
            ret = -1;
        }
    }

    up_prev = up;
    dn_prev = dn;

    return ret;
}