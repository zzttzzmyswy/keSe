/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "crc.h"
#include "dma2d.h"
#include "gpio.h"
#include "ltdc.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "./font/fonts.h"
#include "DrawingBoard.h"
#include "bsp_lcd.h"
#include "bsp_touch_resistance.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

void LCD_DISPLAY_ENABLE(void);
void LCD_DISPLAY_DISABLE(void);

uint32_t fontFlag = 0; /*控制显示字符时是否写入背景颜色的变量*/
char strTouch[10] = { 0 }; /*当前触摸点字符串*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief  板级资源初始化
 * @param  None
 * @retval None
 */
void bsp_init(void) {
	strTouch[9] = '\n'; /* 串口发送数据最后一位是换行 */
	LCD_DISPLAY_ENABLE();          /* 显示屏背光使能 */
	Adc_Init();                    /* 初始化电阻触摸屏ADC */
	HAL_ADC_Start(&hadc3);         /* 启动电阻触摸屏ADC */
	HAL_TIM_Base_Start_IT(&htim6); /* 启动内部定时器6，用于获取触摸信息 */
	HAL_TIM_Base_Start_IT(&htim17); /* 启动内部定时器17，用于闪烁led */
	LCD_Clear(LCD_COLOR565_RED);    /* 显示屏清空 */
	LCD_SetFont(&Font8x8);          /* 设置默认字体为8x8英文字体 */
	DrawingBoard_Init();            /* 画板资源初始化，按钮绘制 */
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */
	int32_t touchP[2];  /* 触摸记录 */
	int32_t touchF = 0; /* 触摸情况记录 */
	/* USER CODE END 1 */
	/* MPU Configuration--------------------------------------------------------*/
	MPU_Config();
	/* Enable I-Cache---------------------------------------------------------*/
	SCB_EnableICache();
	/* Enable D-Cache---------------------------------------------------------*/
	SCB_EnableDCache();
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick.
	 */
	HAL_Init();
	/* USER CODE BEGIN Init */
	/* USER CODE END Init */
	/* Configure the system clock */
	SystemClock_Config();
	/* Configure the peripherals common clocks */
	PeriphCommonClock_Config();
	/* USER CODE BEGIN SysInit */
	/* USER CODE END SysInit */
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_ADC3_Init();
	MX_DMA2D_Init();
	MX_LTDC_Init();
	MX_CRC_Init();
	MX_TIM6_Init();
	MX_ADC1_Init();
	MX_TIM17_Init();
	/* USER CODE BEGIN 2 */
	bsp_init();         /* 板级初始化 */
	/* USER CODE END 2 */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */
		/*考虑到处理时间，实际处理触摸信息频率略小于66Hz*/
		HAL_Delay(15);
		if (port[0] < 0) {
			if (touchF == 1) {
				/* 发生按下到断开的过程 */
				DrawingBoard_Button_Up(touchP[0], touchP[1]);
			}
			touchF = 0;
		}
		else {
			if (touchF == 0) {
				/* 发生断开到按下的过程 */
			}
			/*按钮按下调用画板接口*/
			DrawingBoard_Button_Down(touchP[0] < 0 ? port[0] : touchP[0],
				touchP[1] < 0 ? port[1] : touchP[1], port[0], port[1]);
			touchF = 1;
		}
		/* 记录触摸信息 */
		touchP[0] = port[0];
		touchP[1] = port[1];
		/*每次执行完毕后led2变化一次*/
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); /*LED闪烁*/
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	/** Supply configuration update enable
	 */
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);
	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}
	/** Macro to configure the PLL clock source
	 */
	__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType =
		RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 5;
	RCC_OscInitStruct.PLL.PLLN = 192;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 24;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		Error_Handler();
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
		RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
		RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
		Error_Handler();
	/** Enables the Clock Security System
	 */
	HAL_RCC_EnableCSS();
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void) {
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };
	/** Initializes the peripherals clock
	 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CKPER;
	PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		Error_Handler();
}

/* USER CODE BEGIN 4 */

/**
 * @brief  重写printf底层字符串输出函数
 * @param  底层输出字符
 * @retval 返回输出的字符
 */
int stdout_putchar(int ch) {
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1,
		0xFFFF); // UART输出单个单位数据的函数
	return ch;
}

/**
 * @brief  使能显示屏
 * @param  None
 * @retval None
 */
void LCD_DISPLAY_ENABLE(void) {
	HAL_GPIO_WritePin(LTDC_BK_GPIO_Port, LTDC_BK_Pin, GPIO_PIN_SET);
}

/**
 * @brief  失能显示屏
 * @param  None
 * @retval None
 */
void LCD_DISPLAY_DISABLE(void) {
	HAL_GPIO_WritePin(LTDC_BK_GPIO_Port, LTDC_BK_Pin, GPIO_PIN_RESET);
}

/**
 * @brief  定时器中断回调
 * @param  定时器控制结构体指针
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim6)
		touch_ad(); /*调用电阻触摸点获取函数*/
	else if (htim == &htim17)
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin); /*LED闪烁*/
}

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void) {
	MPU_Region_InitTypeDef MPU_InitStruct = { 0 };
	/* Disables the MPU */
	HAL_MPU_Disable();
	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.BaseAddress = 0x24000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
	MPU_InitStruct.SubRegionDisable = 0x0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Number = MPU_REGION_NUMBER1;
	MPU_InitStruct.BaseAddress = 0x30000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Number = MPU_REGION_NUMBER2;
	MPU_InitStruct.BaseAddress = 0x30020000;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Number = MPU_REGION_NUMBER3;
	MPU_InitStruct.BaseAddress = 0x30040000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Number = MPU_REGION_NUMBER4;
	MPU_InitStruct.BaseAddress = 0x38000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/* Enables the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line
	   number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
	   line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
