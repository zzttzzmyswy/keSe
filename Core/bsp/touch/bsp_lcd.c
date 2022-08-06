#include "bsp_lcd.h"
#include "dma2d.h"
extern DMA2D_HandleTypeDef hdma2d;
extern uint32_t fontFlag;
/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32F4_DISCOVERY
  * @{
  */

/** @addtogroup STM32F429I_DISCOVERY
  * @{
  */

/** @defgroup STM32F429I_DISCOVERY_LCD
  * @brief This file includes the LCD driver for (ILI9341)
  * @{
  */

/** @defgroup STM32F429I_DISCOVERY_LCD_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32F429I_DISCOVERY_LCD_Private_Defines
  * @{
  */

#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))
/**
  * @}
  */

/** @defgroup STM32F429I_DISCOVERY_LCD_Private_Macros
  * @{
  */
#define ABS(X)  ((X) > 0 ? (X) : -(X))
/**
  * @}
  */


#if  LCD_RGB_888
/****************RGB888驱动*****************************RGB888驱动*****************************RGB888驱动**********************************RGB888驱动**********************************RGB888驱动*********************************/

#else
/****************RG565驱动***************************RGB565驱动**********************************RGB565驱动*****************************RGB565驱动*****************************RGB565驱动*****************************/

#define RGB565_RED      0xf800      //1111 1000 0000 0000 | 0000 0000 1111 1000
#define RGB565_GREEN    0x07e0      //0000 0111 1110 0000 | 0000 0000 1111 1100
#define RGB565_BLUE     0x001f      //0000 0000 0001 1111 | 0000 0000 1111 1000
uint32_t RGB565ToRGB888(uint16_t n565Color) {
	uint32_t n888Color = 0;
	uint32_t cRed   = (n565Color & RGB565_RED)    << 8;
	uint32_t cGreen = (n565Color & RGB565_GREEN)  << 5;
	uint32_t cBlue  = (n565Color & RGB565_BLUE)   << 3;
	n888Color = cRed | cGreen  | cBlue ;
	return n888Color;
}
/**
 ******************************************************************************
 * @file    stm32f429i_discovery_lcd.c
 * @author  MCD Application Team
 * @version V1.0.1
 * @date    28-October-2013
 * @brief   This file includes the LCD driver for ILI9341 Liquid Crystal
 *          Display Modules of STM32F429I-DISCO kit (MB1075).
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */
/**
 * @}
 */

/** @defgroup STM32F429I_DISCOVERY_LCD_Private_Variables
 * @{
 */
static sFONT *LCD_Currentfonts;
/* Global variables to set the written text color */
static uint16_t CurrentTextColor   = 0x0000;
static uint16_t CurrentBackColor   = 0xFFFF;
/* Default LCD configuration with LCD Layer 1 */
static uint32_t CurrentFrameBuffer = LCD_FRAME_BUFFER;
static uint32_t CurrentLayer = LCD_BACKGROUND_LAYER;
/**
 * @}
 */
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount,
	uint16_t Closed);
static void LCD_GPIO_Config(void);

/**
 * @}
 */


/**
 * @brief  Sets the LCD Layer.
 * @param  Layerx: specifies the Layer foreground or background.
 * @retval None
 */
void LCD_SetLayer(uint32_t Layerx) {
	if (Layerx == LCD_BACKGROUND_LAYER) {
		CurrentFrameBuffer = LCD_FRAME_BUFFER;
		CurrentLayer = LCD_BACKGROUND_LAYER;
	}
	else {
		CurrentFrameBuffer = LCD_FRAME_BUFFER + BUFFER_OFFSET;
		CurrentLayer = LCD_FOREGROUND_LAYER;
	}
}

/**
 * @brief  Sets the LCD Text and Background colors.
 * @param  TextColor: specifies the Text Color.
 * @param  BackColor: specifies the Background Color.
 * @retval None
 */
void LCD_SetColors(uint16_t TextColor, uint16_t BackColor) {
	CurrentTextColor = TextColor;
	CurrentBackColor = BackColor;
}

/**
 * @brief  Gets the LCD Text and Background colors.
 * @param  TextColor: pointer to the variable that will contain the Text
           Color.
 * @param  BackColor: pointer to the variable that will contain the Background
           Color.
 * @retval None
 */
void LCD_GetColors(uint16_t *TextColor, uint16_t *BackColor) {
	*TextColor = CurrentTextColor;
	*BackColor = CurrentBackColor;
}

/**
 * @brief  Sets the Text color.
 * @param  Color: specifies the Text color code RGB(5-6-5).
 * @retval None
 */
void LCD_SetTextColor(uint16_t Color) {
	CurrentTextColor = Color;
}

/**
 * @brief  Sets the Background color.
 * @param  Color: specifies the Background color code RGB(5-6-5).
 * @retval None
 */
void LCD_SetBackColor(uint16_t Color) {
	CurrentBackColor = Color;
}

/**
 * @brief  Sets the Text Font.
 * @param  fonts: specifies the font to be used.
 * @retval None
 */
void LCD_SetFont(sFONT *fonts) {
	LCD_Currentfonts = fonts;
}

/**
 * @brief  Configure the transparency.
 * @param  transparency: specifies the transparency,
 *         This parameter must range from 0x00 to 0xFF.
 *         0x00 -> Transparent   0xFF -> Opaque
 * @retval None
 */
void LCD_SetTransparency(uint8_t transparency) {
}

/**
 * @brief  Gets the Text Font.
 * @param  None.
 * @retval the used font.
 */
sFONT *LCD_GetFont(void) {
	return LCD_Currentfonts;
}

/**
 * @brief  Clears the selected line.
 * @param  Line: the Line to be cleared.
 *   This parameter can be one of the following values:
 *     @arg LCD_LINE_x: where x can be: 0..13 if LCD_Currentfonts is Font16x24
 *                                      0..26 if LCD_Currentfonts is Font12x12 or Font8x12
 *                                      0..39 if LCD_Currentfonts is Font8x8
 * @retval None
 */
void LCD_ClearLine(uint16_t Line) {
	uint16_t refcolumn = 0;
	/* Send the string character by character on lCD */
	while ((refcolumn < LCD_PIXEL_WIDTH)
		&& (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >=
			LCD_Currentfonts->Width)) {
		/* Display one character on LCD */
		LCD_DisplayChar(Line, refcolumn, ' ');
		/* Decrement the column position by 16 */
		refcolumn += LCD_Currentfonts->Width;
	}
}

/**
 * @brief  Clears the hole LCD.
 * @param  Color: the color of the background.
 * @retval None
 */
void LCD_Clear(uint16_t Color) {
	HAL_DMA2D_DeInit(&hdma2d);
	hdma2d.Instance = DMA2D;
	hdma2d.Init.Mode = DMA2D_R2M;
	hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
	hdma2d.Init.OutputOffset = 0;
	if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
		Error_Handler();
	HAL_DMA2D_Start_IT(&hdma2d, RGB565ToRGB888(Color), CurrentFrameBuffer,
		LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT);
	while (HAL_DMA2D_GetState(&hdma2d) != HAL_DMA2D_STATE_READY);
	LCD_SetTextColor(CurrentTextColor);
}

/**
 * @brief  Sets the cursor position.
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @retval Display Address
 */
uint32_t LCD_SetCursor(uint16_t Xpos, uint16_t Ypos) {
	return CurrentFrameBuffer + 2 * (Xpos + (LCD_PIXEL_WIDTH * Ypos));
}

/**
 * @brief  Config and Sets the color Keying.
 * @param  RGBValue: Specifies the Color reference.
 * @retval None
 */
//void LCD_SetColorKeying(uint32_t RGBValue)
//{
// LTDC_ColorKeying_InitTypeDef   LTDC_colorkeying_InitStruct;

// /* configure the color Keying */
// LTDC_colorkeying_InitStruct.LTDC_ColorKeyBlue = 0x0000FF & RGBValue;
// LTDC_colorkeying_InitStruct.LTDC_ColorKeyGreen = (0x00FF00 & RGBValue) >> 8;
// LTDC_colorkeying_InitStruct.LTDC_ColorKeyRed = (0xFF0000 & RGBValue) >> 16;

// if (CurrentLayer == LCD_BACKGROUND_LAYER)
// {
//   /* Enable the color Keying for Layer1 */
//   LTDC_ColorKeyingConfig(LTDC_Layer1, &LTDC_colorkeying_InitStruct, ENABLE);
//   LTDC_ReloadConfig(LTDC_IMReload);
// }
// else
// {
//   /* Enable the color Keying for Layer2 */
//   LTDC_ColorKeyingConfig(LTDC_Layer2, &LTDC_colorkeying_InitStruct, ENABLE);
//   LTDC_ReloadConfig(LTDC_IMReload);
// }
//}

/**
 * @brief  Disable the color Keying.
 * @param  RGBValue: Specifies the Color reference.
 * @retval None
 */
//void LCD_ReSetColorKeying(void)
//{
// LTDC_ColorKeying_InitTypeDef   LTDC_colorkeying_InitStruct;

// if (CurrentLayer == LCD_BACKGROUND_LAYER)
// {
//   /* Disable the color Keying for Layer1 */
//   LTDC_ColorKeyingConfig(LTDC_Layer1, &LTDC_colorkeying_InitStruct, DISABLE);
//   LTDC_ReloadConfig(LTDC_IMReload);
// }
// else
// {
//   /* Disable the color Keying for Layer2 */
//   LTDC_ColorKeyingConfig(LTDC_Layer2, &LTDC_colorkeying_InitStruct, DISABLE);
//   LTDC_ReloadConfig(LTDC_IMReload);
// }
//}

/**
 * @brief  Draws a character on LCD.
 * @param  Xpos: the Line where to display the character shape.
 * @param  Ypos: start column address.
 * @param  c: pointer to the character data.
 * @retval None
 */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c) {
	uint32_t index = 0, counter = 0, xpos = 0;
	uint32_t  Xaddress = 0;
	xpos = Xpos * LCD_PIXEL_WIDTH * 2;
	Xaddress += Ypos;
	for (index = 0; index < LCD_Currentfonts->Height; index++) {
		for (counter = 0; counter < LCD_Currentfonts->Width; counter++) {
			if ((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12) * 8)) >> counter))
						== 0x00) && (LCD_Currentfonts->Width <= 12)) ||
				(((c[index] & (0x1 << counter)) == 0x00) && (LCD_Currentfonts->Width > 12))) {
				/* Write data value to all SDRAM memory */
				if (fontFlag == 0)
					*(__IO uint16_t *)(CurrentFrameBuffer + (2 * Xaddress) + xpos) =
						CurrentBackColor;
			}
			else {
				/* Write data value to all SDRAM memory */
				*(__IO uint16_t *)(CurrentFrameBuffer + (2 * Xaddress) + xpos) =
					CurrentTextColor;
			}
			Xaddress++;
		}
		Xaddress += (LCD_PIXEL_WIDTH - LCD_Currentfonts->Width);
	}
}

/**
 * @brief  Displays one character (16dots width, 24dots height).
 * @param  Line: the Line where to display the character shape .
 *   This parameter can be one of the following values:
 *     @arg Linex: where x can be 0..29
 * @param  Column: start column address.
 * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
 * @retval None
 */
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii) {
	Ascii -= 32;
	LCD_DrawChar(Line, Column,
		&LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}

/**
 * @brief  Displays a maximum of 20 char on the LCD.
 * @param  Line: the Line where to display the character shape .
 *   This parameter can be one of the following values:
 *     @arg Linex: where x can be 0..9
 * @param  *ptr: pointer to string to display on LCD.
 * @retval None
 */
void LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr) {
	uint16_t refcolumn = 0;
	/* Send the string character by character on lCD */
	while ((refcolumn < LCD_PIXEL_WIDTH)
		&& ((*ptr != 0) & (((refcolumn + LCD_Currentfonts->Width) & 0xFFFF) >=
				LCD_Currentfonts->Width))) {
		/* Display one character on LCD */
		LCD_DisplayChar(Line, refcolumn, *ptr);
		/* Decrement the column position by width */
		refcolumn += LCD_Currentfonts->Width;
		/* Point on the next character */
		ptr++;
	}
}



/**
 * @brief  Displays a line.
 * @param Xpos: specifies the X position, can be a value from 0 to 240.
 * @param Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param Length: line length.
 * @param Direction: line direction.
 *   This parameter can be one of the following values: LCD_DIR_HORIZONTAL or LCD_DIR_VERTICAL.
 * @retval None
 */
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length,
	uint8_t Direction) {
	uint32_t  Xaddress = 0;
	Xaddress = CurrentFrameBuffer + 2 * (LCD_PIXEL_WIDTH * Ypos + Xpos);
	HAL_DMA2D_DeInit(&hdma2d);
	hdma2d.Instance = DMA2D;
	hdma2d.Init.Mode = DMA2D_R2M;
	hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
	hdma2d.Init.OutputOffset = 0;
	if (Direction == LCD_DIR_HORIZONTAL)
		hdma2d.Init.OutputOffset = 0;
	else
		hdma2d.Init.OutputOffset = LCD_PIXEL_WIDTH - 1;
	if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
		Error_Handler();
	if (Direction == LCD_DIR_HORIZONTAL)
		HAL_DMA2D_Start_IT(&hdma2d, RGB565ToRGB888(CurrentTextColor), Xaddress, Length,
			1);
	else
		HAL_DMA2D_Start_IT(&hdma2d, RGB565ToRGB888(CurrentTextColor), Xaddress, 1,
			Length);
	while (HAL_DMA2D_GetState(&hdma2d) != HAL_DMA2D_STATE_READY);
}

/**
 * @brief  Displays a rectangle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Height: display rectangle height, can be a value from 0 to 320.
 * @param  Width: display rectangle width, can be a value from 0 to 240.
 * @retval None
 */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width,
	uint16_t Height) {
	/* draw horizontal lines */
	LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
	LCD_DrawLine(Xpos, (Ypos + Height), Width, LCD_DIR_HORIZONTAL);
	/* draw vertical lines */
	LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
	LCD_DrawLine((Xpos + Width), Ypos, Height, LCD_DIR_VERTICAL);
}

/**
 * @brief  Draw a circle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: radius of the circle.
 * @retval None
 */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius) {
	int x = -Radius, y = 0, err = 2 - 2 * Radius, e2;
	do {
		*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos - x) + LCD_PIXEL_WIDTH *
					(Ypos + y)))) = CurrentTextColor;
		*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos + x) + LCD_PIXEL_WIDTH *
					(Ypos + y)))) = CurrentTextColor;
		*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos + x) + LCD_PIXEL_WIDTH *
					(Ypos - y)))) = CurrentTextColor;
		*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos - x) + LCD_PIXEL_WIDTH *
					(Ypos - y)))) = CurrentTextColor;
		e2 = err;
		if (e2 <= y) {
			err += ++y * 2 + 1;
			if (-x == y && e2 <= x)
				e2 = 0;
		}
		if (e2 > x)
			err += ++x * 2 + 1;
	} while (x <= 0);
}

/**
 * @brief  Draw a full ellipse.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: minor radius of ellipse.
 * @param  Radius2: major radius of ellipse.
 * @retval None
 */
void LCD_DrawFullEllipse(int Xpos, int Ypos, int Radius, int Radius2) {
	int x = -Radius, y = 0, err = 2 - 2 * Radius, e2;
	float K = 0, rad1 = 0, rad2 = 0;
	rad1 = Radius;
	rad2 = Radius2;
	if (Radius > Radius2) {
		do {
			K = (float)(rad1 / rad2);
			LCD_DrawLine((Xpos + x), (Ypos - (uint16_t)(y / K)),
				(2 * (uint16_t)(y / K) + 1), LCD_DIR_VERTICAL);
			LCD_DrawLine((Xpos - x), (Ypos - (uint16_t)(y / K)),
				(2 * (uint16_t)(y / K) + 1), LCD_DIR_VERTICAL);
			e2 = err;
			if (e2 <= y) {
				err += ++y * 2 + 1;
				if (-x == y && e2 <= x)
					e2 = 0;
			}
			if (e2 > x)
				err += ++x * 2 + 1;
		} while (x <= 0);
	}
	else {
		y = -Radius2;
		x = 0;
		do {
			K = (float)(rad2 / rad1);
			LCD_DrawLine((Xpos - (uint16_t)(x / K)), (Ypos + y),
				(2 * (uint16_t)(x / K) + 1), LCD_DIR_HORIZONTAL);
			LCD_DrawLine((Xpos - (uint16_t)(x / K)), (Ypos - y),
				(2 * (uint16_t)(x / K) + 1), LCD_DIR_HORIZONTAL);
			e2 = err;
			if (e2 <= x) {
				err += ++x * 2 + 1;
				if (-y == x && e2 <= y)
					e2 = 0;
			}
			if (e2 > y)
				err += ++y * 2 + 1;
		} while (y <= 0);
	}
}

/**
 * @brief  Displays an Ellipse.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: specifies Radius.
 * @param  Radius2: specifies Radius2.
 * @retval None
 */
void LCD_DrawEllipse(int Xpos, int Ypos, int Radius, int Radius2) {
	int x = -Radius, y = 0, err = 2 - 2 * Radius, e2;
	float K = 0, rad1 = 0, rad2 = 0;
	rad1 = Radius;
	rad2 = Radius2;
	if (Radius > Radius2) {
		do {
			K = (float)(rad1 / rad2);
			*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos - x) + LCD_PIXEL_WIDTH *
						(Ypos + (uint16_t)(y / K))))) = CurrentTextColor;
			*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos + x) + LCD_PIXEL_WIDTH *
						(Ypos + (uint16_t)(y / K))))) = CurrentTextColor;
			*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos + x) + LCD_PIXEL_WIDTH *
						(Ypos - (uint16_t)(y / K))))) = CurrentTextColor;
			*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos - x) + LCD_PIXEL_WIDTH *
						(Ypos - (uint16_t)(y / K))))) = CurrentTextColor;
			e2 = err;
			if (e2 <= y) {
				err += ++y * 2 + 1;
				if (-x == y && e2 <= x)
					e2 = 0;
			}
			if (e2 > x)
				err += ++x * 2 + 1;
		} while (x <= 0);
	}
	else {
		y = -Radius2;
		x = 0;
		do {
			K = (float)(rad2 / rad1);
			*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos - (uint16_t)(
								x / K)) + LCD_PIXEL_WIDTH * (Ypos + y)))) = CurrentTextColor;
			*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos + (uint16_t)(
								x / K)) + LCD_PIXEL_WIDTH * (Ypos + y)))) = CurrentTextColor;
			*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos + (uint16_t)(
								x / K)) + LCD_PIXEL_WIDTH * (Ypos - y)))) = CurrentTextColor;
			*(__IO uint16_t *)(CurrentFrameBuffer + (2 * ((Xpos - (uint16_t)(
								x / K)) + LCD_PIXEL_WIDTH * (Ypos - y)))) = CurrentTextColor;
			e2 = err;
			if (e2 <= x) {
				err += ++x * 2 + 1;
				if (-y == x && e2 <= y)
					e2 = 0;
			}
			if (e2 > y)
				err += ++y * 2 + 1;
		} while (y <= 0);
	}
}

/**
 * @brief  Displays a mono-color picture.
 * @param  Pict: pointer to the picture array.
 * @retval None
 */
void LCD_DrawMonoPict(const uint32_t *Pict) {
	uint32_t index = 0, counter = 0;
	for (index = 0; index < 2400; index++) {
		for (counter = 0; counter < 32; counter++) {
			if ((Pict[index] & (1 << counter)) == 0x00)
				*(__IO uint16_t *)(CurrentFrameBuffer) = CurrentBackColor;
			else
				*(__IO uint16_t *)(CurrentFrameBuffer) = CurrentTextColor;
		}
	}
}

/**
 * @brief  Displays a full rectangle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Height: rectangle height.
 * @param  Width: rectangle width.
 * @retval None
 */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width,
	uint16_t Height) {
	uint32_t  Xaddress = 0;
	Xaddress = CurrentFrameBuffer + 2 * (LCD_PIXEL_WIDTH * Ypos + Xpos);
	HAL_DMA2D_DeInit(&hdma2d);
	hdma2d.Instance = DMA2D;
	hdma2d.Init.Mode = DMA2D_R2M;
	hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
	hdma2d.Init.OutputOffset = (LCD_PIXEL_WIDTH - Width);
	if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
		Error_Handler();
	HAL_DMA2D_Start_IT(&hdma2d, RGB565ToRGB888(CurrentTextColor), Xaddress, Width,
		Height);
	while (HAL_DMA2D_GetState(&hdma2d) != HAL_DMA2D_STATE_READY);
	LCD_SetTextColor(CurrentTextColor);
}

/**
 * @brief  Displays a full circle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius
 * @retval None
 */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius) {
	int32_t  D;    /* Decision Variable */
	uint32_t  CurX;/* Current X Value */
	uint32_t  CurY;/* Current Y Value */
	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;
	while (CurX <= CurY) {
		if (CurY > 0) {
			LCD_DrawLine(Xpos - CurX, Ypos - CurY, 2 * CurY, LCD_DIR_VERTICAL);
			LCD_DrawLine(Xpos + CurX, Ypos - CurY, 2 * CurY, LCD_DIR_VERTICAL);
		}
		if (CurX > 0) {
			LCD_DrawLine(Xpos - CurY, Ypos - CurX, 2 * CurX, LCD_DIR_VERTICAL);
			LCD_DrawLine(Xpos + CurY, Ypos - CurX, 2 * CurX, LCD_DIR_VERTICAL);
		}
		if (D < 0)
			D += (CurX << 2) + 6;
		else {
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
	LCD_DrawCircle(Xpos, Ypos, Radius);
}

/**
 * @brief  Displays an uni-line (between two points).
 * @param  x1: specifies the point 1 x position.
 * @param  y1: specifies the point 1 y position.
 * @param  x2: specifies the point 2 x position.
 * @param  y2: specifies the point 2 y position.
 * @retval None
 */
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
			yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;
	deltax = ABS(x2 - x1);        /* The difference between the x's */
	deltay = ABS(y2 - y1);        /* The difference between the y's */
	x = x1;                       /* Start x off at the first pixel */
	y = y1;                       /* Start y off at the first pixel */
	if (x2 >= x1) {               /* The x-values are increasing */
		xinc1 = 1;
		xinc2 = 1;
	}
	else {                        /* The x-values are decreasing */
		xinc1 = -1;
		xinc2 = -1;
	}
	if (y2 >= y1) {               /* The y-values are increasing */
		yinc1 = 1;
		yinc2 = 1;
	}
	else {                        /* The y-values are decreasing */
		yinc1 = -1;
		yinc2 = -1;
	}
	if (deltax >=
		deltay) {       /* There is at least one x-value for every y-value */
		xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
		yinc2 = 0;                  /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;         /* There are more x-values than y-values */
	}
	else {                        /* There is at least one y-value for every x-value */
		xinc2 = 0;                  /* Don't change the x for every iteration */
		yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;         /* There are more y-values than x-values */
	}
	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		PutPixel(x, y);             /* Draw the current pixel */
		num += numadd;              /* Increase the numerator by the top of the fraction */
		if (num >= den) {           /* Check if numerator >= denominator */
			num -= den;               /* Calculate the new numerator value */
			x += xinc1;               /* Change the x as appropriate */
			y += yinc1;               /* Change the y as appropriate */
		}
		x += xinc2;                 /* Change the x as appropriate */
		y += yinc2;                 /* Change the y as appropriate */
	}
}

/**
 * @brief  Displays an triangle.
 * @param  Points: pointer to the points array.
 * @retval None
 */
void LCD_Triangle(pPoint Points, uint16_t PointCount) {
	int16_t X = 0, Y = 0;
	pPoint First = Points;
	if (PointCount != 3)
		return;
	while (--PointCount) {
		X = Points->X;
		Y = Points->Y;
		Points++;
		LCD_DrawUniLine(X, Y, Points->X, Points->Y);
	}
	LCD_DrawUniLine(First->X, First->Y, Points->X, Points->Y);
}

/**
 * @brief  Fill an triangle (between 3 points).
 * @param  x1..3: x position of triangle point 1..3.
 * @param  y1..3: y position of triangle point 1..3.
 * @retval None
 */
void LCD_FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1,
	uint16_t y2, uint16_t y3) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
			yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;
	deltax = ABS(x2 - x1);        /* The difference between the x's */
	deltay = ABS(y2 - y1);        /* The difference between the y's */
	x = x1;                       /* Start x off at the first pixel */
	y = y1;                       /* Start y off at the first pixel */
	if (x2 >= x1) {               /* The x-values are increasing */
		xinc1 = 1;
		xinc2 = 1;
	}
	else {                        /* The x-values are decreasing */
		xinc1 = -1;
		xinc2 = -1;
	}
	if (y2 >= y1) {               /* The y-values are increasing */
		yinc1 = 1;
		yinc2 = 1;
	}
	else {                        /* The y-values are decreasing */
		yinc1 = -1;
		yinc2 = -1;
	}
	if (deltax >=
		deltay) {       /* There is at least one x-value for every y-value */
		xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
		yinc2 = 0;                  /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;         /* There are more x-values than y-values */
	}
	else {                        /* There is at least one y-value for every x-value */
		xinc2 = 0;                  /* Don't change the x for every iteration */
		yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;         /* There are more y-values than x-values */
	}
	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		LCD_DrawUniLine(x, y, x3, y3);
		num += numadd;              /* Increase the numerator by the top of the fraction */
		if (num >= den) {           /* Check if numerator >= denominator */
			num -= den;               /* Calculate the new numerator value */
			x += xinc1;               /* Change the x as appropriate */
			y += yinc1;               /* Change the y as appropriate */
		}
		x += xinc2;                 /* Change the x as appropriate */
		y += yinc2;                 /* Change the y as appropriate */
	}
}
/**
 * @brief  Displays an poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LCD_PolyLine(pPoint Points, uint16_t PointCount) {
	int16_t X = 0, Y = 0;
	if (PointCount < 2)
		return;
	while (--PointCount) {
		X = Points->X;
		Y = Points->Y;
		Points++;
		LCD_DrawUniLine(X, Y, Points->X, Points->Y);
	}
}

/**
 * @brief  Displays an relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @param  Closed: specifies if the draw is closed or not.
 *           1: closed, 0 : not closed.
 * @retval None
 */
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount,
	uint16_t Closed) {
	int16_t X = 0, Y = 0;
	pPoint First = Points;
	if (PointCount < 2)
		return;
	X = Points->X;
	Y = Points->Y;
	while (--PointCount) {
		Points++;
		LCD_DrawUniLine(X, Y, X + Points->X, Y + Points->Y);
		X = X + Points->X;
		Y = Y + Points->Y;
	}
	if (Closed)
		LCD_DrawUniLine(First->X, First->Y, X, Y);
}

/**
 * @brief  Displays a closed poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount) {
	LCD_PolyLine(Points, PointCount);
	LCD_DrawUniLine(Points->X, Points->Y, (Points + PointCount - 1)->X,
		(Points + PointCount - 1)->Y);
}

/**
 * @brief  Displays a relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount) {
	LCD_PolyLineRelativeClosed(Points, PointCount, 0);
}

/**
 * @brief  Displays a closed relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount) {
	LCD_PolyLineRelativeClosed(Points, PointCount, 1);
}

/**
 * @brief  在显示器上显示中英文字符串,超出液晶宽度时会自动换行。
                     中英文混显示时，请把英文字体设置为Font16x24格式
 * @param  Line ：行(也可理解为y坐标)
 * @param  Column ：列（也可理解为x坐标）
 * @param  pStr ：要显示的字符串的首地址
 * @retval 无
 */
void LCD_DispString_EN(uint16_t Line, uint16_t Column, const uint8_t *pStr) {
	while (* pStr != '\0') {
		/*自动换行*/
		if ((Column + LCD_Currentfonts->Width) > LCD_PIXEL_WIDTH) {
			Column = 0;
			Line += LCD_Currentfonts->Height;
		}
		if ((Line + LCD_Currentfonts->Height) > LCD_PIXEL_HEIGHT) {
			Column = 0;
			Line = 0;
		}
		LCD_DisplayChar(Line, Column, *pStr);
		Column += LCD_Currentfonts->Width;
		pStr ++;
	}
}

/**
 * @brief  Displays a  full poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount) {
	int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0,
			Y_first = 0, pixelX = 0, pixelY = 0, counter = 0;
	uint16_t  IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;
	IMAGE_LEFT = IMAGE_RIGHT = Points->X;
	IMAGE_TOP = IMAGE_BOTTOM = Points->Y;
	for (counter = 1; counter < PointCount; counter++) {
		pixelX = POLY_X(counter);
		if (pixelX < IMAGE_LEFT)
			IMAGE_LEFT = pixelX;
		if (pixelX > IMAGE_RIGHT)
			IMAGE_RIGHT = pixelX;
		pixelY = POLY_Y(counter);
		if (pixelY < IMAGE_TOP)
			IMAGE_TOP = pixelY;
		if (pixelY > IMAGE_BOTTOM)
			IMAGE_BOTTOM = pixelY;
	}
	if (PointCount < 2)
		return;
	X_center = (IMAGE_LEFT + IMAGE_RIGHT) / 2;
	Y_center = (IMAGE_BOTTOM + IMAGE_TOP) / 2;
	X_first = Points->X;
	Y_first = Points->Y;
	while (--PointCount) {
		X = Points->X;
		Y = Points->Y;
		Points++;
		X2 = Points->X;
		Y2 = Points->Y;
		LCD_FillTriangle(X, X2, X_center, Y, Y2, Y_center);
		LCD_FillTriangle(X, X_center, X2, Y, Y_center, Y2);
		LCD_FillTriangle(X_center, X2, X, Y_center, Y2, Y);
	}
	LCD_FillTriangle(X_first, X2, X_center, Y_first, Y2, Y_center);
	LCD_FillTriangle(X_first, X_center, X2, Y_first, Y_center, Y2);
	LCD_FillTriangle(X_center, X2, X_first, Y_center, Y2, Y_first);
}

/**
 * @brief  Displays a pixel.
 * @param  x: pixel x.
 * @param  y: pixel y.
 * @retval None
 */
void PutPixel(int16_t x, int16_t y) {
	if (x < 0 || x > LCD_PIXEL_WIDTH || y < 0 || y > LCD_PIXEL_HEIGHT)
		return;
#if 0
	LCD_DrawLine(x, y, 1, LCD_DIR_HORIZONTAL);
#else
	{
		uint32_t  Xaddress = 0;
		Xaddress = CurrentFrameBuffer + 2 * (LCD_PIXEL_WIDTH * y + x);
		*(__IO uint16_t *) Xaddress = CurrentTextColor;
	}
#endif
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
