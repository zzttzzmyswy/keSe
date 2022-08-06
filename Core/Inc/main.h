/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "bsp_lcd.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_3
#define LED1_GPIO_Port GPIOE
#define SPI_FLASH_CS_Pin GPIO_PIN_4
#define SPI_FLASH_CS_GPIO_Port GPIOE
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOC
#define K1_Pin GPIO_PIN_0
#define K1_GPIO_Port GPIOA
#define K1_EXTI_IRQn EXTI0_IRQn
#define LTDC_BK_Pin GPIO_PIN_2
#define LTDC_BK_GPIO_Port GPIOA
#define TOUCH_Y__Pin GPIO_PIN_7
#define TOUCH_Y__GPIO_Port GPIOA
#define TOUCH_X__Pin GPIO_PIN_4
#define TOUCH_X__GPIO_Port GPIOC
#define TOUCH_Y_C5_Pin GPIO_PIN_5
#define TOUCH_Y_C5_GPIO_Port GPIOC
#define TOUCH_X_B1_Pin GPIO_PIN_1
#define TOUCH_X_B1_GPIO_Port GPIOB
#define W5500_CS_Pin GPIO_PIN_15
#define W5500_CS_GPIO_Port GPIOA
#define W5500_INT_Pin GPIO_PIN_4
#define W5500_INT_GPIO_Port GPIOD
#define W5500_RST_Pin GPIO_PIN_7
#define W5500_RST_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */


#define IN_SRAM_DT __attribute__((section (".RAM_DT")))
#define IN_SRAM_D1 __attribute__((section (".RAM_D1")))
#define IN_SRAM_D2 __attribute__((section (".RAM_D2")))
#define IN_SRAM_D3 __attribute__((section (".RAM_D3")))
#define IN_SRAM_D4 __attribute__((section (".RAM_D4")))
#define IN_FLASH_EXT __attribute__((section ("EXTFLASH")))

//GPIO引脚位置定义
#define PIN0                1<<0
#define PIN1                1<<1
#define PIN2                1<<2
#define PIN3                1<<3
#define PIN4                1<<4
#define PIN5                1<<5
#define PIN6                1<<6
#define PIN7                1<<7
#define PIN8                1<<8
#define PIN9                1<<9
#define PIN10               1<<10
#define PIN11               1<<11
#define PIN12               1<<12
#define PIN13               1<<13
#define PIN14               1<<14
#define PIN15               1<<15

//GPIO设置专用宏定义
#define GPIO_MODE_IN        0       //普通输入模式
#define GPIO_MODE_OUT       1       //普通输出模式
#define GPIO_MODE_AF        2       //AF功能模式
#define GPIO_MODE_AIN       3       //模拟输入模式

#define GPIO_PUPD_NONE      0       //不带上下拉
#define GPIO_PUPD_PU        1       //上拉
#define GPIO_PUPD_PD        2       //下拉
#define GPIO_PUPD_RES       3       //保留 

#define GPIO_OTYPE_PP       0       //推挽输出
#define GPIO_OTYPE_OD       1       //开漏输出 

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
