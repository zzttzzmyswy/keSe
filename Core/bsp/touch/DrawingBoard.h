#ifndef _DrawingBoard_H
#define _DrawingBoard_H

#include "bsp_lcd.h"
#include "main.h"

/*
    LCD 颜色代码，COLOR_是Color的简写
    16Bit由高位至低位， RRRR RGGG GGGB BBBB
*/
/*RGB565 颜色转换*/
#define RGB(R, G, B)                                                           \
	(((R >> 3) << 11) | ((G >> 2) << 5) |                                        \
		(B >> 3)) /* 将8位R,G,B转化为 16位RGB565格式 */
#define RGB565_R(x) ((x >> 8) & 0xF8)
#define RGB565_G(x) ((x >> 3) & 0xFC)
#define RGB565_B(x) ((x << 3) & 0xF8)

enum {
	COLOR_WHITE = RGB(255, 255, 255),       /* 白色 */
	COLOR_BLACK = RGB(0, 0, 0),             /* 黑色 */
	COLOR_RED = RGB(255, 0, 0),             /* 红色 */
	COLOR_GREEN = RGB(0, 255, 0),           /* 绿色 */
	COLOR_BLUE = RGB(0, 0, 255),            /* 蓝色 */
	COLOR_YELLOW = RGB(255, 255, 0),        /* 黄色 */
	COLOR_GREY = RGB(98, 98, 98),           /* 深灰色 */
	COLOR_GREY1 = RGB(150, 150, 150),       /* 浅灰色 */
	COLOR_GREY2 = RGB(180, 180, 180),       /* 浅灰色 */
	COLOR_GREY3 = RGB(200, 200, 200),       /* 最浅灰色 */
	COLOR_GREY4 = RGB(230, 230, 230),       /* 最浅灰色 */
	COLOR_BUTTON_GREY = RGB(240, 240, 240), /* 按钮表面灰色 */
	COLOR_MAGENTA = RGB(255, 0, 255),       /* 红紫色，洋红色 */
	COLOR_CYAN = RGB(0, 255, 255),          /* 蓝绿色，青色 */
	COLOR_BLUE1 = RGB(0, 0, 240),           /* 深蓝色 */
	COLOR_BLUE2 = RGB(0, 0, 128),           /* 深蓝色 */
	COLOR_BLUE3 = RGB(68, 68, 255),         /* 浅蓝色1 */
	COLOR_BLUE4 = RGB(0, 64, 128),          /* 浅蓝色1 */
	COLOR_MASK = 0x7FFF /* RGB565颜色掩码，用于文字背景透明 */
};

/*画刷形状列表*/
typedef enum {
	/*1像素宽度线*/
	LINE_SINGLE_PIXCEL = 0,
	/*2像素宽度线*/
	LINE_2_PIXCEL,
	/*4像素宽度线*/
	LINE_4_PIXCEL,
	/*6像素宽度线*/
	LINE_6_PIXCEL,
	/*8像素宽度线*/
	LINE_8_PIXCEL,
	/*16像素宽度线*/
	LINE_16_PIXCEL,
	/*带圆连线*/
	LINE_WITH_CIRCLE,
	/*矩形*/
	RECT,
	/*橡皮*/
	ERASER,
} BRUSH_SHAPE;

/*画刷参数*/
typedef struct {
	/*颜色*/
	uint16_t color;
	/*画刷类型*/
	BRUSH_SHAPE brush_shape;
} DrawingBoard_Brush_Style;

/*按钮结构体参数*/
/*左上角开始x*/
/*左上角开始y*/
/*右下角x*/
/*右下角y*/
/*按钮参数*/
/*按钮绘制函数*/
/*按钮回调函数*/
typedef struct {
	uint16_t start_x;
	uint16_t start_y;
	uint16_t end_x;
	uint16_t end_y;
	uint32_t para;
	uint8_t touch_flag;
	void (*draw_btn)(void *btn);
	void (*btn_command)(void *btn);
} DrawingBoard_Button;

/**************宏定义*****************/

#define ABS(X) ((X) > 0 ? (X) : -(X))

#define BUTTON_WIDTH 60                              /* 按钮宽度 */
#define BUTTON_HEIGHT 30                             /* 按钮高度 */
#define BUTTON_NUM 18                                /* 按钮数目 */
#define DRAWING_BOARD_START_X (BUTTON_WIDTH * 1 + 1) /* 画板左上角X坐标 */
#define DRAWING_BOARD_START_Y 1 /* 画板左上角Y坐标 */
#define DRAWING_BOARD_END_X                                                    \
	(LCD_PIXEL_WIDTH - 1 - BUTTON_WIDTH)             /* 画板右下角X坐标 */
#define DRAWING_BOARD_END_Y (LCD_PIXEL_HEIGHT - 1) /* 画板右下角Y坐标 */

#define DrawingBoard_BUTTON_INIT_CODE { \
		/*笔刷相关按钮*/\
		/*结构体参数如下*/\
		/*左上角开始x*/\
		/*左上角开始y*/\
		/*右下角x*/\
		/*右下角y*/\
		/*按钮参数*/\
		/*按钮绘制函数*/\
		/*按钮回调函数*/\
		/*橡皮檫*/\
		button[0].start_x = 0;\
		button[0].start_y = BUTTON_HEIGHT * 0;\
		button[0].end_x = BUTTON_WIDTH * 1;\
		button[0].end_y = BUTTON_HEIGHT * 1;\
		button[0].para = ERASER;\
		button[0].touch_flag = 0;\
		button[0].draw_btn = Draw_Button_Eraser;\
		button[0].btn_command = Select_Brush_Class;\
		/*矩形*/\
		button[1].start_x = 0;\
		button[1].start_y = BUTTON_HEIGHT * 1;\
		button[1].end_x = BUTTON_WIDTH * 1;\
		button[1].end_y = BUTTON_HEIGHT * 2;\
		button[1].para = RECT;\
		button[1].touch_flag = 0;\
		button[1].draw_btn = Draw_Button_Rect;\
		button[1].btn_command = Select_Brush_Class;\
		/*带圆线*/                                    \
		button[2].start_x = 0;\
		button[2].start_y = BUTTON_HEIGHT * 2;\
		button[2].end_x = BUTTON_WIDTH * 1;\
		button[2].end_y = BUTTON_HEIGHT * 3;\
		button[2].para = LINE_WITH_CIRCLE;\
		button[2].touch_flag = 0;\
		button[2].draw_btn = Draw_Button_Shape;\
		button[2].btn_command = Select_Brush_Class;\
		/*宽度为1的线*/\
		button[3].start_x = 0;\
		button[3].start_y = BUTTON_HEIGHT * 3;\
		button[3].end_x = BUTTON_WIDTH * 1;\
		button[3].end_y = BUTTON_HEIGHT * 4;\
		button[3].para = LINE_SINGLE_PIXCEL;\
		button[3].touch_flag = 0;\
		button[3].draw_btn = Draw_Button_Shape;\
		button[3].btn_command = Select_Brush_Class;\
		/*宽度为2的线*/\
		button[4].start_x = 0;\
		button[4].start_y = BUTTON_HEIGHT * 4;\
		button[4].end_x = BUTTON_WIDTH * 1;\
		button[4].end_y = BUTTON_HEIGHT * 5;\
		button[4].para = LINE_2_PIXCEL;\
		button[4].touch_flag = 0;\
		button[4].draw_btn = Draw_Button_Shape;\
		button[4].btn_command = Select_Brush_Class;\
		/*宽度为4的线*/\
		button[5].start_x = 0;\
		button[5].start_y = BUTTON_HEIGHT * 5;\
		button[5].end_x = BUTTON_WIDTH * 1;\
		button[5].end_y = BUTTON_HEIGHT * 6;\
		button[5].para = LINE_4_PIXCEL;\
		button[5].touch_flag = 0;\
		button[5].draw_btn = Draw_Button_Shape;\
		button[5].btn_command = Select_Brush_Class;\
		/*宽度为6的线*/\
		button[6].start_x = 0;\
		button[6].start_y = BUTTON_HEIGHT * 6;\
		button[6].end_x = BUTTON_WIDTH * 1;\
		button[6].end_y = BUTTON_HEIGHT * 7;\
		button[6].para = LINE_6_PIXCEL;\
		button[6].touch_flag = 0;\
		button[6].draw_btn = Draw_Button_Shape;\
		button[6].btn_command = Select_Brush_Class;\
		/*宽度为8的线*/\
		button[7].start_x = 0;\
		button[7].start_y = BUTTON_HEIGHT * 7;\
		button[7].end_x = BUTTON_WIDTH * 1;\
		button[7].end_y = BUTTON_HEIGHT * 8;\
		button[7].para = LINE_8_PIXCEL;\
		button[7].touch_flag = 0;\
		button[7].draw_btn = Draw_Button_Shape;\
		button[7].btn_command = Select_Brush_Class;\
		/*宽度为16的线*/\
		button[8].start_x = 0;\
		button[8].start_y = BUTTON_HEIGHT * 8;\
		button[8].end_x = BUTTON_WIDTH * 1;\
		button[8].end_y = DRAWING_BOARD_END_Y;\
		button[8].para = LINE_16_PIXCEL;\
		button[8].touch_flag = 0;\
		button[8].draw_btn = Draw_Button_Shape;\
		button[8].btn_command = Select_Brush_Class;\
		/*清屏和颜色按钮*/\
		/*清屏*/\
		button[9].start_x = LCD_PIXEL_WIDTH - BUTTON_WIDTH;\
		button[9].start_y = BUTTON_HEIGHT * 0;\
		button[9].end_x = LCD_PIXEL_WIDTH - 1;\
		button[9].end_y = BUTTON_HEIGHT * 1;\
		button[9].para = COLOR_BUTTON_GREY;\
		button[9].touch_flag = 0;\
		button[9].draw_btn = Draw_Button_Clear;\
		button[9].btn_command = Clear_DrawingBoard_Area;\
		/*红色*/\
		button[10].start_x = LCD_PIXEL_WIDTH - BUTTON_WIDTH;\
		button[10].start_y = BUTTON_HEIGHT * 1;\
		button[10].end_x = LCD_PIXEL_WIDTH - 1;\
		button[10].end_y = BUTTON_HEIGHT * 2;\
		button[10].para = COLOR_RED;\
		button[10].touch_flag = 0;\
		button[10].draw_btn = Draw_Button_Color;\
		button[10].btn_command = Select_Shape_Color;\
		/*绿色*/\
		button[11].start_x = LCD_PIXEL_WIDTH - BUTTON_WIDTH;\
		button[11].start_y = BUTTON_HEIGHT * 2;\
		button[11].end_x = LCD_PIXEL_WIDTH - 1;\
		button[11].end_y = BUTTON_HEIGHT * 3;\
		button[11].para = COLOR_GREEN;\
		button[11].touch_flag = 0;\
		button[11].draw_btn = Draw_Button_Color;\
		button[11].btn_command = Select_Shape_Color;\
		/*蓝色*/\
		button[12].start_x = LCD_PIXEL_WIDTH - BUTTON_WIDTH;\
		button[12].start_y = BUTTON_HEIGHT * 3;\
		button[12].end_x = LCD_PIXEL_WIDTH - 1;\
		button[12].end_y = BUTTON_HEIGHT * 4;\
		button[12].para = COLOR_BLUE;\
		button[12].touch_flag = 0;\
		button[12].draw_btn = Draw_Button_Color;\
		button[12].btn_command = Select_Shape_Color;\
		/*紫色*/\
		button[13].start_x = LCD_PIXEL_WIDTH - BUTTON_WIDTH;\
		button[13].start_y = BUTTON_HEIGHT * 4;\
		button[13].end_x = LCD_PIXEL_WIDTH - 1;\
		button[13].end_y = BUTTON_HEIGHT * 5;\
		button[13].para = COLOR_MAGENTA;\
		button[13].touch_flag = 0;\
		button[13].draw_btn = Draw_Button_Color;\
		button[13].btn_command = Select_Shape_Color;\
		/*黄色*/\
		button[14].start_x = LCD_PIXEL_WIDTH - BUTTON_WIDTH;\
		button[14].start_y = BUTTON_HEIGHT * 5;\
		button[14].end_x = LCD_PIXEL_WIDTH - 1;\
		button[14].end_y = BUTTON_HEIGHT * 6;\
		button[14].para = COLOR_YELLOW;\
		button[14].touch_flag = 0;\
		button[14].draw_btn = Draw_Button_Color;\
		button[14].btn_command = Select_Shape_Color;\
		/*青色*/\
		button[15].start_x = LCD_PIXEL_WIDTH - BUTTON_WIDTH;\
		button[15].start_y = BUTTON_HEIGHT * 6;\
		button[15].end_x = LCD_PIXEL_WIDTH - 1;\
		button[15].end_y = BUTTON_HEIGHT * 7;\
		button[15].para = COLOR_CYAN;\
		button[15].touch_flag = 0;\
		button[15].draw_btn = Draw_Button_Color;\
		button[15].btn_command = Select_Shape_Color;\
		/*黑色*/\
		button[16].start_x = LCD_PIXEL_WIDTH - BUTTON_WIDTH;\
		button[16].start_y = BUTTON_HEIGHT * 7;\
		button[16].end_x = LCD_PIXEL_WIDTH - 1;\
		button[16].end_y = BUTTON_HEIGHT * 8;\
		button[16].para = COLOR_BLACK;\
		button[16].touch_flag = 0;\
		button[16].draw_btn = Draw_Button_Color;\
		button[16].btn_command = Select_Shape_Color;\
		/*灰色*/\
		button[17].start_x = LCD_PIXEL_WIDTH - BUTTON_WIDTH;\
		button[17].start_y = BUTTON_HEIGHT * 8;\
		button[17].end_x = LCD_PIXEL_WIDTH - 1;\
		button[17].end_y = DRAWING_BOARD_END_Y;\
		button[17].para = COLOR_GREY2;\
		button[17].touch_flag = 0;\
		button[17].draw_btn = Draw_Button_Color;\
		button[17].btn_command = Select_Shape_Color;}

/**************函数*****************/
/*滑板资源初始化*/
void DrawingBoard_Init(void);
/*发送按下*/
void DrawingBoard_Button_Down(int16_t pre_x, int16_t pre_y, int16_t x,
	int16_t y);
/*发送按下到抬起*/
void DrawingBoard_Button_Up(uint16_t x, uint16_t y);

#endif //_DrawingBoard_H
