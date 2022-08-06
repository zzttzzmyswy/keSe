#ifndef __LCD_H
#define __LCD_H

#include "main.h"

#include "./font/fonts.h"

/*把这个宏设置成非0值 液晶屏使用RGB888色彩，若为0则使用RGB565色彩*/
#define LCD_RGB_888  0


#define LCD_COLOR_WHITE888          0xFFFFFF
#define LCD_COLOR_BLACK888          0x000000
#define LCD_COLOR_GREY888           0xC0C0C0
#define LCD_COLOR_BLUE888           0x0000FF
#define LCD_COLOR_BLUE2888          0x0000A0
#define LCD_COLOR_RED888            0xFF0000
#define LCD_COLOR_MAGENTA888       0xFF00FF
#define LCD_COLOR_GREEN888         0x00FF00
#define LCD_COLOR_CYAN888           0x00FFFF
#define LCD_COLOR_YELLOW888         0xFFFF00

#if  LCD_RGB_888

#else
/***RGB565头文件*****************************RGB565头文件*****************************RGB565头文件*****************************RGB565头文件*****************************RGB565头文件*****************************RGB565头文件*****************************RGB565头文件*****************************/


/** @defgroup STM32F429I_DISCOVERY_LCD_Exported_Types
  * @{
  */
typedef struct {
	int16_t X;
	int16_t Y;
} Point, * pPoint;
/**
  * @}
  */

/** @defgroup stm32f429i_discovery_LCD_Exported_Constants
  * @{
  */

/* LCD Size (Width and Height) */
#define  LCD_PIXEL_WIDTH       ((uint16_t)480)
#define  LCD_PIXEL_HEIGHT      ((uint16_t)272)

#define LCD_FRAME_BUFFER       ((uint32_t)0x24040400)
//#define BUFFER_OFFSET          ((uint32_t)480*272*2)
/* 仅有一个层 */
#define BUFFER_OFFSET          ((uint32_t)0)


//RGB565
#define LCD_COLOR565_WHITE           0xFFFF        //11111 111111 11111
#define LCD_COLOR565_BLACK           0x0000        //00000 000000 00000
#define LCD_COLOR565_GREY            0xF7DE        //11110 111110 11110
#define LCD_COLOR565_BLUE            0x001F        //00000 000000 11111
#define LCD_COLOR565_BLUE2           0x051F        //00000 101000 11111
#define LCD_COLOR565_RED             0xF800        //11111 000000 00000
#define LCD_COLOR565_MAGENTA         0xF81F        //11111 000000 11111
#define LCD_COLOR565_GREEN           0x07E0        //00000 111111 00000
#define LCD_COLOR565_CYAN            0x7FFF        //01111 111111 11111
#define LCD_COLOR565_YELLOW          0xFFE0        //11111 111111 00000

#define TRANSPARENCY                 0x7FFF  //透明


/**
  * @brief  LCD Lines depending on the chosen fonts.
  */
#define LCD_LINE_0               LINE(0)
#define LCD_LINE_1               LINE(1)
#define LCD_LINE_2               LINE(2)
#define LCD_LINE_3               LINE(3)
#define LCD_LINE_4               LINE(4)
#define LCD_LINE_5               LINE(5)
#define LCD_LINE_6               LINE(6)
#define LCD_LINE_7               LINE(7)
#define LCD_LINE_8               LINE(8)
#define LCD_LINE_9               LINE(9)
#define LCD_LINE_10              LINE(10)
#define LCD_LINE_11              LINE(11)
#define LCD_LINE_12              LINE(12)
#define LCD_LINE_13              LINE(13)
#define LCD_LINE_14              LINE(14)
#define LCD_LINE_15              LINE(15)
#define LCD_LINE_16              LINE(16)
#define LCD_LINE_17              LINE(17)
#define LCD_LINE_18              LINE(18)
#define LCD_LINE_19              LINE(19)
#define LCD_LINE_20              LINE(20)
#define LCD_LINE_21              LINE(21)
#define LCD_LINE_22              LINE(22)
#define LCD_LINE_23              LINE(23)
#define LCD_LINE_24              LINE(24)
#define LCD_LINE_25              LINE(25)
#define LCD_LINE_26              LINE(26)
#define LCD_LINE_27              LINE(27)
#define LCD_LINE_28              LINE(28)
#define LCD_LINE_29              LINE(29)
#define LCD_LINE_30              LINE(30)
#define LCD_LINE_31              LINE(31)
#define LCD_LINE_32              LINE(32)
#define LCD_LINE_33              LINE(33)
#define LCD_LINE_34              LINE(34)
#define LCD_LINE_35              LINE(35)
#define LCD_LINE_36              LINE(36)
#define LCD_LINE_37              LINE(37)
#define LCD_LINE_38              LINE(38)
#define LCD_LINE_39              LINE(39)

/**
  * @brief LCD default font
  */
#define LCD_DEFAULT_FONT         Font8x8

/**
  * @brief  LCD Direction
  */
#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001

/**
  * @}
  */

/**
  * @brief  LCD Layer
  */
#define LCD_BACKGROUND_LAYER     0x0000
#define LCD_FOREGROUND_LAYER     0x0001

/**
  * @}
  */

/** @defgroup STM32F429I_DISCOVERY_LCD_Exported_Macros
  * @{
  */
#define ASSEMBLE_RGB(R, G, B)     ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3))


/** @defgroup STM32F429I_DISCOVERY_LCD_Exported_Functions
  * @{
  */
void     LCD_Init(void);
void     LCD_LayerInit(void);
void     LCD_SetLayer(uint32_t Layerx);
void     LCD_SetColors(uint16_t _TextColor, uint16_t _BackColor);
void     LCD_GetColors(uint16_t *_TextColor, uint16_t *_BackColor);
void     LCD_SetTextColor(uint16_t Color);
void     LCD_SetBackColor(uint16_t Color);
void     LCD_SetTransparency(uint8_t transparency);
void     LCD_ClearLine(uint16_t Line);
void     LCD_Clear(uint16_t Color);
uint32_t LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void     LCD_SetColorKeying(uint32_t RGBValue);
void     LCD_ReSetColorKeying(void);
void     LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c);
void     LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii);
void     LCD_SetFont(sFONT *fonts);
sFONT   *LCD_GetFont(void);
void     LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr);
void     LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length,
	uint8_t Direction);
void     LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width,
	uint16_t Height);
void     LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void     LCD_DrawEllipse(int Xpos, int Ypos, int Radius, int Radius2);
void     LCD_DrawFullEllipse(int Xpos, int Ypos, int Radius, int Radius2);
void     LCD_DrawMonoPict(const uint32_t *Pict);
void     LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void     LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width,
	uint16_t Height);
void     LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void     LCD_PolyLine(pPoint Points, uint16_t PointCount);
void     LCD_PolyLineRelative(pPoint Points, uint16_t PointCount);
void     LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount);
void     LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount);
void     LCD_FillPolyLine(pPoint Points, uint16_t PointCount);
void     LCD_Triangle(pPoint Points, uint16_t PointCount);
void     LCD_FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1,
	uint16_t y2, uint16_t y3);
void     PutPixel(int16_t x, int16_t y);

void        LCD_DispString_EN(uint16_t Line, uint16_t Column,
	const uint8_t *pStr);




/**
  * @}
  */

#endif


#endif /* __LCD_H */
