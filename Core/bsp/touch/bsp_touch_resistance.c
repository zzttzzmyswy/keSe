#include "bsp_touch_resistance.h"

uint16_t value_t[4] = {0}; /*ADC��ȡֵ*/
uint32_t value_touch = 0;  /*�ж��Ƿ���ֵ����������ֵ��*/
int32_t port[2] = {0};     /*���µ�����ֵ����������û�а���*/

/**
 * @brief  GPIO��������
			 AF0~15�������(��������г����õ�,��ϸ�����STM32H743xx�����ֲ�,Table 9~19):
			 AF0:MCO/SWD/SWCLK/RTC;        AF1:TIM1/2/TIM16/17/LPTIM1;
			 AF2:TIM3~5/TIM12/HRTIM1/SAI1;   AF3:TIM8/LPTIM2~5/HRTIM1/LPUART1;
			 AF4:I2C1~I2C4/TIM15/USART1;   AF5:SPI1~SPI6/CEC; AF6:SPI3/SAI1~3/UART4/I2C4;
			 AF7:SPI2/3/6/USART1~3/6/UART7/SDIO1; AF8:USART4/5/8/SPDIF/SAI2/4;
			 AF9;FDCAN1~2/TIM13/14/LCD/QSPI; AF10:USB_OTG1/2/SAI2/4/QSPI;
			 AF11:ETH/UART7/SDIO2/I2C4/COMP1/2; AF12:FMC/SDIO1/OTG2/LCD;
			 AF13:DCIM/DSI/LCD/COMP1/2;      AF14:LCD/UART5; AF15:EVENTOUT;
 * @param  
			GPIOx:GPIOA~GPIOK.				
			BITx:0~15,����IO���ű��.
			AFx:0~15,����AF0~AF15.
 * @retval None
 */
void GPIO_AF_Set(GPIO_TypeDef *GPIOx, uint8_t BITx, uint8_t AFx) {
  GPIOx->AFR[BITx >> 3] &= ~(0X0F << ((BITx & 0X07) * 4));
  GPIOx->AFR[BITx >> 3] |= (uint32_t)AFx << ((BITx & 0X07) * 4);
}

/**
 * @brief  GPIOͨ������
 * 				ע��:������ģʽ(��ͨ����/ģ������)��,OTYPE��OSPEED������Ч!!
 * @param  
				GPIOx:GPIOA~GPIOK.
				BITx:0X0000~0XFFFF,λ����,ÿ��λ����һ��IO,��0λ����Px0,��1λ����Px1,��������.����0X0101,����ͬʱ����Px0��Px8.
				MODE:0~3;ģʽѡ��,0,����(ϵͳ��λĬ��״̬);1,��ͨ���;2,���ù���;3,ģ������.
				OTYPE:0/1;�������ѡ��,0,�������;1,��©���.
				OSPEED:0~3;����ٶ�����,0,����;1,����;2,����;3,����.
				PUPD:0~3:����������,0,����������;1,����;2,����;3,����.
 * @retval None
 */
void GPIO_Set(GPIO_TypeDef *GPIOx, uint32_t BITx, uint32_t MODE, uint32_t OTYPE,
              uint32_t OSPEED, uint32_t PUPD) {
  uint32_t pinpos = 0, pos = 0, curpin = 0;
  for (pinpos = 0; pinpos < 16; pinpos++) {
    pos = 1 << pinpos;                      // һ����λ���
    curpin = BITx & pos;                    // ��������Ƿ�Ҫ����
    if (curpin == pos) {                    // ��Ҫ����
      GPIOx->MODER &= ~(3 << (pinpos * 2)); // �����ԭ��������
      GPIOx->MODER |= MODE << (pinpos * 2); // �����µ�ģʽ
      if ((MODE == 0X01) || (MODE == 0X02)) { // ��������ģʽ/���ù���ģʽ
        GPIOx->OSPEEDR &= ~(3 << (pinpos * 2));     // ���ԭ��������
        GPIOx->OSPEEDR |= (OSPEED << (pinpos * 2)); // �����µ��ٶ�ֵ
        GPIOx->OTYPER &= ~(1 << pinpos);            // ���ԭ��������
        GPIOx->OTYPER |= OTYPE << pinpos;           // �����µ����ģʽ
      }
      GPIOx->PUPDR &= ~(3 << (pinpos * 2)); // �����ԭ��������
      GPIOx->PUPDR |= PUPD << (pinpos * 2); // �����µ�������
    }
  }
}

/**
 * @brief  ����GPIOĳ�����ŵ����״̬
 * @param  GPIOx:GPIOA~GPIOK.
 * @param  pinx:����λ��,��Χ:1<<0 ~ 1<<15
 * @param  status:����״̬(�����λ��Ч),0,����͵�ƽ;1,����ߵ�ƽ
 * @retval None
 */
void GPIO_Pin_Set(GPIO_TypeDef *GPIOx, uint16_t pinx, uint8_t status) {
  if (status & 0X01)
    GPIOx->BSRR = pinx; // ����GPIOx��pinxΪ1
  else
    GPIOx->BSRR = pinx << 16; // ����GPIOx��pinxΪ0
}

/**
 * @brief  ��ʼ��ADC
	��ȡGPIOĳ�����ŵ�״̬
	GPIOx:GPIOA~GPIOK.
	pinx:����λ��,��Χ:1<<0 ~ 1<<15
	����ֵ:����״̬,0,���ŵ͵�ƽ;1,���Ÿߵ�ƽ
 * @param  pinx:����λ��,��Χ:1<<0 ~ 1<<15
 * @retval ����״̬,0,���ŵ͵�ƽ;1,���Ÿߵ�ƽ
 */
uint8_t GPIO_Pin_Get(GPIO_TypeDef *GPIOx, uint16_t pinx) {
  if (GPIOx->IDR & pinx)
    return 1; // pinx��״̬Ϊ1
  else
    return 0; // pinx��״̬Ϊ0
}

/**
 * @brief  ��ʼ��ADC
 * @param  None
 * @retval None
 */
void Adc_Init(void) {
  // �ȳ�ʼ��IO��
  //   RCC->AHB1ENR|=1<<5;         //ʹ��ADC1/2ʱ��
  //   RCC->AHB4ENR|=1<<0;         //ʹ��PORTAʱ��
  //   GPIO_Set(GPIOA,PIN5,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,ģ������,����������
  RCC->AHB1RSTR |= 1 << 5;    // ADC1/2��λ
  RCC->AHB1RSTR &= ~(1 << 5); // ��λ����
  RCC->D3CCIPR &= ~(3 << 16); // ADCSEL[1:0]����
  RCC->D3CCIPR |=
      2
      << 16; // ADCSEL[1:0]=2,per_ck��ΪADCʱ��Դ,Ĭ��ѡ��hsi_ker_ck��Ϊper_ck,Ƶ��:64Mhz
  ADC12_COMMON->CCR |=
      1
      << 18; // PRESC[3:0]=1,����ʱ��2��Ƶ,��adc_ker_ck=per_ck/2=64/2=32Mhz(���ܳ���36Mhz)
  ADC1->CR = 0;        // CR�Ĵ�������,DEEPPWD����,�����˯�߻���.
  ADC1->CR |= 1 << 28; // ADVREGEN=1,ʹ��ADC��ѹ��
  HAL_Delay(1); // �ȴ���ѹ���������,Լ10us,������ʱ��һ��,û��ϵ.
  ADC1->CR |=
      1
      << 8; // BOOST=1,ADC������boostģʽ(ADCƵ�ʴ���20M��ʱ��,��������boostλ)
  ADC1->CFGR &= ~(1 << 13); // CONT=0,����ת��ģʽ
  ADC1->CFGR |= 1 << 12;    // OVRMOD=1,��дģʽ(DR�Ĵ����ɱ���д)
  ADC1->CFGR &= ~(3 << 10); // EXTEN[1:0]=0,�������
  ADC1->CFGR &= ~(7 << 2);  // RES[2:0]λ����
  ADC1->CFGR |=
      0 << 2; // RES[2:0]=0,16λ�ֱ���(0,16λ;1,14λ;2,12λ;3,10λ;4,8λ.)
  ADC1->CFGR2 &= ~((uint32_t)15 << 28); // LSHIFT[3:0]=0,������,�����Ҷ���.
  ADC1->CFGR2 &= ~((uint32_t)0X3FF << 16); // OSR[9:0]=0,��ʹ�ù�����
  ADC1->CR &= ~((uint32_t)1 << 30); // ADCALDIF=0,У׼����ת��ͨ��
  ADC1->CR |= 1 << 16;              // ADCALLIN=1,����У׼
  ADC1->CR |= (uint32_t)1 << 31;    // ����У׼
  while (ADC1->CR & ((uint32_t)1 << 31))
    ;                        // �ȴ�У׼���
  ADC1->SQR1 &= ~(0XF << 0); // L[3:0]����
  ADC1->SQR1 |= 0 << 0; // L[3:0]=0,1��ת���ڹ��������� Ҳ����ֻת����������1
  //  //����ͨ��19�Ĳ���ʱ��
  //  ADC1->SMPR2&=~(7<<(3*9));   //ͨ��19����ʱ�����
  //  ADC1->SMPR2|=7<<(3*9);      //ͨ��19
  //  810.5������,��߲���ʱ�������߾�ȷ��
  // ���ô���ͨ��4,5,7,8�Ĳ���ʱ��
  ADC1->SMPR1 &= !(7 << (3 * 4));
  ADC1->SMPR1 |= 7 << (3 * 4);
  ADC1->SMPR1 &= !(7 << (3 * 5));
  ADC1->SMPR1 |= 7 << (3 * 5);
  ADC1->SMPR1 &= !(7 << (3 * 7));
  ADC1->SMPR1 |= 7 << (3 * 7);
  ADC1->SMPR1 &= !(7 << (3 * 8));
  ADC1->SMPR1 |= 7 << (3 * 8);
  ADC1->CR |= 1 << 0; // ����ADת����
}
// ���ADCֵ
// ch:ͨ��ֵ 0~19
// ����ֵ:ת�����
/**
 * @brief  ���ADCֵ
 * @param  ch:ͨ��ֵ 0~19
 * @retval ת�����
 */
uint16_t Get_Adc(uint8_t ch) {
  ADC1->PCSEL |= 1 << ch; // ADCת��ͨ��Ԥѡ��
  // ����ת������
  ADC1->SQR1 &= ~(0X1F << 6 * 1); // ��������1ͨ������
  ADC1->SQR1 |= ch << 6 * 1;      // ���ù�������1��ת��ͨ��Ϊch
  ADC1->CR |= 1 << 2;             // ��������ת��ͨ��
  while (!(ADC1->ISR & 1 << 2))
    ;              // �ȴ�ת������
  return ADC1->DR; // ����adcֵ
}

/**
 * @brief  ��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ��
 * @param  
			ch:ͨ�����
			times:��ȡ����
 * @retval ͨ��ch��times��ת�����ƽ��ֵ
 */
uint16_t Get_Adc_Average(uint8_t ch, uint8_t times) {
  uint32_t temp_val = 0;
  uint8_t t;
  for (t = 0; t < times; t++)
    temp_val += Get_Adc(ch);
  return temp_val / times;
}

/**
 * @brief  ����һ�δ�������
 * ���Ŷ��壺
 *  PB1_X+_ADC1-5
 *  PC4_X-_ADC1-4
 *  PC5_Y+_ADC1-8
 *  PA7_Y-_ADC1-7
 * @param  None
 * @retval ����ֵΪ�Ƿ�����Ч����
 */
uint32_t touch_ad(void) {
  // uint16_t value_t[4]={0};
  uint16_t temp = 0;
  // ������һ��
  // ����PC4-GND PB1-3V3 PC5-ADC PA7-INPUT
  GPIO_Set(GPIOC, PIN4, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PD);
  GPIO_Set(GPIOB, PIN1, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PU);
  GPIO_Set(GPIOC, PIN5, GPIO_MODE_AIN, 0, 0, GPIO_PUPD_NONE);
  GPIO_Set(GPIOA, PIN7, GPIO_MODE_IN, 0, 0, GPIO_PUPD_NONE);
  GPIO_Pin_Set(GPIOC, PIN4, 0);
  GPIO_Pin_Set(GPIOB, PIN1, 1);
  value_t[0] = Get_Adc_Average(8, 5);
  // �����ڶ���
  // ����PC4-GND PC5-3V3 PB1-ADC PA7-INPUT
  GPIO_Set(GPIOC, PIN4, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PD);
  GPIO_Set(GPIOC, PIN5, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PU);
  GPIO_Set(GPIOB, PIN1, GPIO_MODE_AIN, 0, 0, GPIO_PUPD_NONE);
  GPIO_Set(GPIOA, PIN7, GPIO_MODE_IN, 0, 0, GPIO_PUPD_NONE);
  GPIO_Pin_Set(GPIOC, PIN4, 0);
  GPIO_Pin_Set(GPIOC, PIN5, 1);
  temp = Get_Adc_Average(5, 5);
  value_t[1] = temp < 0x100 ? 0X100 : temp;
  // ����������
  // ����PC4-GND PC5-3V3 PA7-ADC PB1-INPUT
  GPIO_Set(GPIOC, PIN4, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PD);
  GPIO_Set(GPIOC, PIN5, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PU);
  GPIO_Set(GPIOA, PIN7, GPIO_MODE_AIN, 0, 0, GPIO_PUPD_NONE);
  GPIO_Set(GPIOB, PIN1, GPIO_MODE_IN, 0, 0, GPIO_PUPD_NONE);
  GPIO_Pin_Set(GPIOC, PIN4, 0);
  GPIO_Pin_Set(GPIOC, PIN5, 1);
  value_t[2] = Get_Adc_Average(7, 5);
  // �������Ĳ�
  // ����PA7-GND PC5-3V3 PB1-ADC PC4-INPUT
  GPIO_Set(GPIOA, PIN7, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PD);
  GPIO_Set(GPIOC, PIN5, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PU);
  GPIO_Set(GPIOB, PIN1, GPIO_MODE_AIN, 0, 0, GPIO_PUPD_NONE);
  GPIO_Set(GPIOC, PIN4, GPIO_MODE_IN, 0, 0, GPIO_PUPD_NONE);
  GPIO_Pin_Set(GPIOA, PIN7, 0);
  GPIO_Pin_Set(GPIOC, PIN5, 1);
  value_t[3] = Get_Adc_Average(5, 5);
  // �ж��Ƿ�Ϊ��Ч���� value_t[2]�����˴���3.3V���ĵ�ѹ
  // value_t[1]�����˴���GND���ĵ�ѹ�����Ӵ�ʱ��ֵӦ�����
  value_touch = value_t[2] / value_t[1];
  if (value_touch < 20 && value_t[0] > 0xD00 && value_t[3] > 0x1000) {
    port[0] = (double)(value_t[0] - 0xE00) / (0xF300 - 0xE00) * 480;
    port[0] = port[0] <= 0 ? 1 : port[0] >= 479 ? 479 : port[0];
    port[1] = (double)(value_t[3] - 0x1B00) / (0xD600 - 0x1B00) * 272;
    port[1] = port[1] <= 0 ? 1 : port[1] >= 271 ? 271 : port[1];
    return 1;
  } else {
    port[0] = -1;
    port[1] = -1;
    return 0;
  }
}
