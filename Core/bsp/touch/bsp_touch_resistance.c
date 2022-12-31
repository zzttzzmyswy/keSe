#include "bsp_touch_resistance.h"

//GPIO复用设置
//GPIOx:GPIOA~GPIOK.
//BITx:0~15,代表IO引脚编号.
//AFx:0~15,代表AF0~AF15.
//AF0~15设置情况(这里仅是列出常用的,详细的请见STM32H743xx数据手册,Table 9~19):
//AF0:MCO/SWD/SWCLK/RTC;        AF1:TIM1/2/TIM16/17/LPTIM1;     AF2:TIM3~5/TIM12/HRTIM1/SAI1;   AF3:TIM8/LPTIM2~5/HRTIM1/LPUART1;
//AF4:I2C1~I2C4/TIM15/USART1;   AF5:SPI1~SPI6/CEC;              AF6:SPI3/SAI1~3/UART4/I2C4;     AF7:SPI2/3/6/USART1~3/6/UART7/SDIO1;
//AF8:USART4/5/8/SPDIF/SAI2/4;  AF9;FDCAN1~2/TIM13/14/LCD/QSPI; AF10:USB_OTG1/2/SAI2/4/QSPI;    AF11:ETH/UART7/SDIO2/I2C4/COMP1/2;
//AF12:FMC/SDIO1/OTG2/LCD;      AF13:DCIM/DSI/LCD/COMP1/2;      AF14:LCD/UART5;                 AF15:EVENTOUT;
void GPIO_AF_Set(GPIO_TypeDef *GPIOx, uint8_t BITx, uint8_t AFx) {
	GPIOx->AFR[BITx >> 3] &= ~(0X0F << ((BITx & 0X07) * 4));
	GPIOx->AFR[BITx >> 3] |= (uint32_t)AFx << ((BITx & 0X07) * 4);
}

//GPIO通用设置
//GPIOx:GPIOA~GPIOK.
//BITx:0X0000~0XFFFF,位设置,每个位代表一个IO,第0位代表Px0,第1位代表Px1,依次类推.比如0X0101,代表同时设置Px0和Px8.
//MODE:0~3;模式选择,0,输入(系统复位默认状态);1,普通输出;2,复用功能;3,模拟输入.
//OTYPE:0/1;输出类型选择,0,推挽输出;1,开漏输出.
//OSPEED:0~3;输出速度设置,0,低速;1,中速;2,快速;3,高速.
//PUPD:0~3:上下拉设置,0,不带上下拉;1,上拉;2,下拉;3,保留.
//注意:在输入模式(普通输入/模拟输入)下,OTYPE和OSPEED参数无效!!
void GPIO_Set(GPIO_TypeDef *GPIOx, uint32_t BITx, uint32_t MODE, uint32_t OTYPE,
	uint32_t OSPEED, uint32_t PUPD) {
	uint32_t pinpos = 0, pos = 0, curpin = 0;
	for (pinpos = 0; pinpos < 16; pinpos++) {
		pos = 1 << pinpos;   //一个个位检查
		curpin = BITx & pos; //检查引脚是否要设置
		if (curpin == pos) { //需要设置
			GPIOx->MODER &= ~(3 << (pinpos * 2)); //先清除原来的设置
			GPIOx->MODER |= MODE << (pinpos * 2); //设置新的模式
			if ((MODE == 0X01)
				|| (MODE == 0X02)) { //如果是输出模式/复用功能模式
				GPIOx->OSPEEDR &= ~(3 << (pinpos * 2));     //清除原来的设置
				GPIOx->OSPEEDR |= (OSPEED << (pinpos * 2)); //设置新的速度值
				GPIOx->OTYPER &= ~(1 << pinpos);            //清除原来的设置
				GPIOx->OTYPER |= OTYPE << pinpos;           //设置新的输出模式
			}
			GPIOx->PUPDR &= ~(3 << (pinpos * 2)); //先清除原来的设置
			GPIOx->PUPDR |= PUPD << (pinpos * 2); //设置新的上下拉
		}
	}
}

//设置GPIO某个引脚的输出状态
//GPIOx:GPIOA~GPIOK.
//pinx:引脚位置,范围:1<<0 ~ 1<<15
//status:引脚状态(仅最低位有效),0,输出低电平;1,输出高电平
void GPIO_Pin_Set(GPIO_TypeDef *GPIOx, uint16_t pinx, uint8_t status) {
	if (status & 0X01)
		GPIOx->BSRR = pinx; //设置GPIOx的pinx为1
	else
		GPIOx->BSRR = pinx << 16; //设置GPIOx的pinx为0
}
//读取GPIO某个引脚的状态
//GPIOx:GPIOA~GPIOK.
//pinx:引脚位置,范围:1<<0 ~ 1<<15
//返回值:引脚状态,0,引脚低电平;1,引脚高电平
uint8_t GPIO_Pin_Get(GPIO_TypeDef *GPIOx, uint16_t pinx) {
	if (GPIOx->IDR & pinx)
		return 1; //pinx的状态为1
	else
		return 0; //pinx的状态为0
}

//初始化ADC
//这里我们仅以规则通道为例
//我们默认仅开启ADC1_CH19P
//16位转换时间位:采样周期+8.5个ADC周期
void Adc_Init(void) {
	//先初始化IO口
	//  RCC->AHB1ENR|=1<<5;         //使能ADC1/2时钟
	//  RCC->AHB4ENR|=1<<0;         //使能PORTA时钟
	//  GPIO_Set(GPIOA,PIN5,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PA5,模拟输入,不带上下拉
	RCC->AHB1RSTR |= 1 << 5;      //ADC1/2复位
	RCC->AHB1RSTR &= ~(1 << 5);   //复位结束
	RCC->D3CCIPR &= ~(3 << 16);   //ADCSEL[1:0]清零
	RCC->D3CCIPR |= 2 <<
		16;      //ADCSEL[1:0]=2,per_ck作为ADC时钟源,默认选择hsi_ker_ck作为per_ck,频率:64Mhz
	ADC12_COMMON->CCR |= 1 <<
		18; //PRESC[3:0]=1,输入时钟2分频,即adc_ker_ck=per_ck/2=64/2=32Mhz(不能超过36Mhz)
	ADC1->CR =
		0;             //CR寄存器清零,DEEPPWD清零,从深度睡眠唤醒.
	ADC1->CR |= 1 << 28;      //ADVREGEN=1,使能ADC稳压器
	HAL_Delay(
		1);             //等待稳压器启动完成,约10us,这里延时大一点,没关系.
	ADC1->CR |= 1 <<
		8;       //BOOST=1,ADC工作在boost模式(ADC频率大于20M的时候,必须设置boost位)
	ADC1->CFGR &= ~(1 << 13); //CONT=0,单次转换模式
	ADC1->CFGR |= 1 << 12;    //OVRMOD=1,复写模式(DR寄存器可被复写)
	ADC1->CFGR &= ~(3 << 10); //EXTEN[1:0]=0,软件触发
	ADC1->CFGR &= ~(7 << 2);  //RES[2:0]位清零
	ADC1->CFGR |= 0 <<
		2;     //RES[2:0]=0,16位分辨率(0,16位;1,14位;2,12位;3,10位;4,8位.)
	ADC1->CFGR2 &= ~((uint32_t)15 <<
			28);    //LSHIFT[3:0]=0,不左移,数据右对齐.
	ADC1->CFGR2 &= ~((uint32_t)0X3FF << 16); //OSR[9:0]=0,不使用过采样
	ADC1->CR &= ~((uint32_t)1 << 30); //ADCALDIF=0,校准单端转换通道
	ADC1->CR |= 1 << 16;              //ADCALLIN=1,线性校准
	ADC1->CR |= (uint32_t)1 << 31;    //开启校准
	while (ADC1->CR & ((uint32_t)1 << 31))
		; //等待校准完成
	ADC1->SQR1 &= ~(0XF << 0); //L[3:0]清零
	ADC1->SQR1 |= 0 <<
		0;      //L[3:0]=0,1个转换在规则序列中 也就是只转换规则序列1
	//  //设置通道19的采样时间
	//  ADC1->SMPR2&=~(7<<(3*9));   //通道19采样时间清空
	//  ADC1->SMPR2|=7<<(3*9);      //通道19 810.5个周期,提高采样时间可以提高精确度
	//设置触摸通道4,5,7,8的采样时间
	ADC1->SMPR1 &= !(7 << (3 * 4));
	ADC1->SMPR1 |= 7 << (3 * 4);
	ADC1->SMPR1 &= !(7 << (3 * 5));
	ADC1->SMPR1 |= 7 << (3 * 5);
	ADC1->SMPR1 &= !(7 << (3 * 7));
	ADC1->SMPR1 |= 7 << (3 * 7);
	ADC1->SMPR1 &= !(7 << (3 * 8));
	ADC1->SMPR1 |= 7 << (3 * 8);
	ADC1->CR |= 1 << 0; //开启AD转换器
}
//获得ADC值
//ch:通道值 0~19
//返回值:转换结果
uint16_t Get_Adc(uint8_t ch) {
	ADC1->PCSEL |= 1 << ch; //ADC转换通道预选择
	//设置转换序列
	ADC1->SQR1 &= ~(0X1F << 6 * 1); //规则序列1通道清零
	ADC1->SQR1 |= ch << 6 * 1;      //设置规则序列1的转换通道为ch
	ADC1->CR |= 1 << 2;             //启动规则转换通道
	while (!(ADC1->ISR & 1 << 2))
		; //等待转换结束
	return ADC1->DR; //返回adc值
}
//获取通道ch的转换值，取times次,然后平均
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
uint16_t Get_Adc_Average(uint8_t ch, uint8_t times) {
	uint32_t temp_val = 0;
	uint8_t t;
	for (t = 0; t < times; t++)
		temp_val += Get_Adc(ch);
	return temp_val / times;
}

//进行一次触摸测量
//返回值为是否有有效触摸
//引脚定义：
//  PB1_X+_ADC1-5
//  PC4_X-_ADC1-4
//  PC5_Y+_ADC1-8
//  PA7_Y-_ADC1-7
uint16_t value_t[4] = {0};/*ADC读取值*/
uint32_t value_touch = 0;/*判断是否按下值（按下力度值）*/
int32_t port[2] = {0};/*按下的坐标值，负数代表没有按下*/
uint32_t touch_ad(void) {
	// uint16_t value_t[4]={0};
	uint16_t temp = 0;
	//测量第一步
	//设置PC4-GND PB1-3V3 PC5-ADC PA7-INPUT
	GPIO_Set(GPIOC, PIN4, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PD);
	GPIO_Set(GPIOB, PIN1, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PU);
	GPIO_Set(GPIOC, PIN5, GPIO_MODE_AIN, 0, 0, GPIO_PUPD_NONE);
	GPIO_Set(GPIOA, PIN7, GPIO_MODE_IN, 0, 0, GPIO_PUPD_NONE);
	GPIO_Pin_Set(GPIOC, PIN4, 0);
	GPIO_Pin_Set(GPIOB, PIN1, 1);
	value_t[0] = Get_Adc_Average(8, 5);
	//测量第二步
	//设置PC4-GND PC5-3V3 PB1-ADC PA7-INPUT
	GPIO_Set(GPIOC, PIN4, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PD);
	GPIO_Set(GPIOC, PIN5, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PU);
	GPIO_Set(GPIOB, PIN1, GPIO_MODE_AIN, 0, 0, GPIO_PUPD_NONE);
	GPIO_Set(GPIOA, PIN7, GPIO_MODE_IN, 0, 0, GPIO_PUPD_NONE);
	GPIO_Pin_Set(GPIOC, PIN4, 0);
	GPIO_Pin_Set(GPIOC, PIN5, 1);
	temp = Get_Adc_Average(5, 5);
	value_t[1] = temp < 0x100 ? 0X100 : temp;
	//测量第三步
	//设置PC4-GND PC5-3V3 PA7-ADC PB1-INPUT
	GPIO_Set(GPIOC, PIN4, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PD);
	GPIO_Set(GPIOC, PIN5, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PU);
	GPIO_Set(GPIOA, PIN7, GPIO_MODE_AIN, 0, 0, GPIO_PUPD_NONE);
	GPIO_Set(GPIOB, PIN1, GPIO_MODE_IN, 0, 0, GPIO_PUPD_NONE);
	GPIO_Pin_Set(GPIOC, PIN4, 0);
	GPIO_Pin_Set(GPIOC, PIN5, 1);
	value_t[2] = Get_Adc_Average(7, 5);
	//测量第四步
	//设置PA7-GND PC5-3V3 PB1-ADC PC4-INPUT
	GPIO_Set(GPIOA, PIN7, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PD);
	GPIO_Set(GPIOC, PIN5, GPIO_MODE_OUT, 0, 2, GPIO_PUPD_PU);
	GPIO_Set(GPIOB, PIN1, GPIO_MODE_AIN, 0, 0, GPIO_PUPD_NONE);
	GPIO_Set(GPIOC, PIN4, GPIO_MODE_IN, 0, 0, GPIO_PUPD_NONE);
	GPIO_Pin_Set(GPIOA, PIN7, 0);
	GPIO_Pin_Set(GPIOC, PIN5, 1);
	value_t[3] = Get_Adc_Average(5, 5);
	// //判断是否为有效按下
	value_touch = (float)((value_t[0]) * 1.0 * (value_t[2]) / (value_t[1])) -
		(value_t[0]);
	if ( value_touch < 50000 && value_t[0] > 0xD00 && value_t[3] > 0x1000) {
		port[0] = (double)(value_t[0] - 0xE00) / (0xF300 - 0xE00) * 480;
		port[0] = port[0] <= 0 ? -1 : port[0] >= 479 ? 479 : port[0];
		port[1] = (double)(value_t[3] - 0x1C00) / (0xDF00 - 0x1C00) * 272;
		port[1] = port[1] <= 0 ? -1 : port[1] >= 271 ? 271 : port[1];
		return 1;
	}
	else {
		port[0] = -1;
		port[1] = -1;
		return 0;
	}
}
