# RM-2027 电控培训 第四次作业

**王语清 / 20263001319 / 电控组**

> 仓库名：作业要求的 `RM-2027-电控培训-作业-王语清` 含中文，而 GitHub 仓库名只允许 ASCII 字母、数字和 `. - _`，中文会被替换成 `-`，故使用拼音 `RM-2027-diankong-hw4-wangyuqing`。

用 TIM2 的 PWM 驱动 SG90 舵机按 0° → 45° → 90° → 135° → 180° 定角转动，每点停 1 秒；OLED 显示「目标角度 / CCR / 脉宽」三行，同时由 USART1 打印到串口。

## 1. 接线

| 器件 | 引脚 |
|---|---|
| SG90 橙线（信号） | PA0（TIM2_CH1） |
| SG90 红线（电源） | 独立 5 V / ≥1 A，不接板子 5 V |
| SG90 棕线（地） | 独立电源 GND，并与板子 GND 共地 |
| OLED SCL / SDA | PB8 / PB9（软件 I²C） |
| OLED VCC / GND | 3.3 V / GND |
| CH340 TXD / RXD | PA10 / PA9（交叉接） |
| ST-Link | 3.3V / SWDIO(PA13) / SWCLK(PA14) / GND |

舵机堵转电流约 700 mA，从板子取电会拉低电压导致复位，所以独立供电；PWM 是相对地的电压，所以必须共地。

## 2. 时钟与定时器配置

时钟树：HSE 8 MHz × PLL9 → SYSCLK 72 MHz，AHB /1 → HCLK 72 MHz，APB1 /2 → PCLK1 36 MHz，**APB1 定时器时钟 72 MHz**（APB1 分频不为 1 时定时器时钟自动 ×2）。

**PSC = 71**：让 1 个计数等于 1 µs

```
计数频率 = 定时器时钟 ÷ (PSC + 1) = 72 MHz ÷ (71 + 1) = 1 MHz   →   1 计数 = 1 µs
```

**ARR = 19999**：让周期等于 20 ms（50 Hz）

```
(ARR + 1) × 1 µs = 20000 µs   →   ARR = 19999
验算：72 000 000 ÷ (71 + 1) ÷ (19999 + 1) = 50 Hz
```

| 参数 | 值 |
|---|---|
| Prescaler (PSC) | 71 |
| Counter Mode | Up |
| Counter Period (ARR) | 19999 |
| auto-reload preload | Enable |
| CH Polarity | High |

PSC 取 71 后 CCR 的数值直接就是脉宽微秒数，后面算角度不用再乘系数。

## 3. 角度换算与两端取值

```c
uint16_t AngleToCCR(float angle)
{
    if (angle < 0.0f)   angle = 0.0f;
    if (angle > 180.0f) angle = 180.0f;

    return (uint16_t)(SERVO_PULSE_MIN_US
                      + angle / 180.0f * (SERVO_PULSE_MAX_US - SERVO_PULSE_MIN_US));
}
```

`SERVO_PULSE_MIN_US = 550`、`SERVO_PULSE_MAX_US = 2450`。

| 角度 | 0° | 45° | 90° | 135° | 180° |
|---|---|---|---|---|---|
| CCR (µs) | 550 | 1025 | 1500 | 1975 | 2450 |
| 脉宽 | 0.550 ms | 1.025 ms | 1.500 ms | 1.975 ms | 2.450 ms |

**两端不取 500 / 2500 的理由**：0.5~2.5 ms 只是 SG90 的标称范围，真实机械行程比它窄。打满到标称端点会顶住内部限位，电机仍在出力就是堵转，电流上升、发热、扫齿。两端各留约 2° 余量取 550 / 2450，90° 代入得 1500 µs 正好是标准中位脉宽，可作自检基准。

`main()` 里先 `Servo_SetAngle()` 设好 CCR 再 `HAL_TIM_PWM_Start()`，上电瞬间就停在 0°，不会先撞到限位。

## 4. 运行结果

串口输出（COM5，115200 8N1）：

```
Target=  0  CCR= 550  Pulse=0.550ms
Target= 45  CCR=1025  Pulse=1.025ms
Target= 90  CCR=1500  Pulse=1.500ms
Target=135  CCR=1975  Pulse=1.975ms
Target=180  CCR=2450  Pulse=2.450ms
```

串口助手截图（`docs/串口输出.png`，端口 COM5、波特率 115200、8 数据位、1 停止位、无校验、**文本模式**）：

![串口输出](docs/串口输出.png)

说明：每行是一条记录，`Target` 为目标角度，`CCR` 为比较寄存器值——因为 PSC=71 使 1 个计数等于 1 µs，这个值同时就是脉宽的微秒数，`Pulse` 是换算后的毫秒值。五个角度一组循环，每个角度停 1 秒，与 OLED 上显示的三行同步一致。

演示视频：`docs/演示视频.mp4`
