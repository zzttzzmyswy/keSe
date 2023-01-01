#ifndef __BSP_TOUCH_RESISTANCE_H
#define __BSP_TOUCH_RESISTANCE_H

#include "main.h"

void GPIO_AF_Set(GPIO_TypeDef *GPIOx, uint8_t BITx, uint8_t AFx);
void GPIO_Set(GPIO_TypeDef *GPIOx, uint32_t BITx, uint32_t MODE, uint32_t OTYPE,
              uint32_t OSPEED, uint32_t PUPD);
void GPIO_Pin_Set(GPIO_TypeDef *GPIOx, uint16_t pinx, uint8_t status);
uint8_t GPIO_Pin_Get(GPIO_TypeDef *GPIOx, uint16_t pinx);

void Adc_Init(void);     /*触摸资源初始化*/
uint32_t touch_ad(void); /*获取电阻触摸点*/

extern int32_t port[2]; /*电阻触摸点，负数代表没有触摸*/

#endif /* __BSP_TOUCH_RESISTANCE_H */
