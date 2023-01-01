#ifndef _DrawingBoard_H
#define _DrawingBoard_H

#include "bsp_lcd.h"
#include "main.h"

#define COLOR_BLOCK_WIDTH 60                              /* 按钮宽度 */
#define COLOR_BLOCK_HEIGHT 30                             /* 按钮高度 */
#define BUTTON_NUM 18                                     /* 按钮数目 */
#define DRAWING_BOARD_START_X (COLOR_BLOCK_WIDTH * 1 + 1) /* 画板左上角X坐标 \
                                                           */
#define DRAWING_BOARD_START_Y 1 /* 画板左上角Y坐标 */
#define DRAWING_BOARD_END_X                                                    \
  (LCD_PIXEL_WIDTH - 1 - COLOR_BLOCK_WIDTH)        /* 画板右下角X坐标 */
#define DRAWING_BOARD_END_Y (LCD_PIXEL_HEIGHT - 1) /* 画板右下角Y坐标 */

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
} SHAPE;

/*画刷参数*/
typedef struct {
  /*颜色*/
  uint16_t color;
  /*画刷类型*/
  SHAPE shape;
} DrawingBoard_Brush_Style;

typedef struct {
  uint16_t start_x; /*按键的x起始坐标*/
  uint16_t start_y; /*按键的y起始坐标*/
  uint16_t end_x;   /*按键的x结束坐标*/
  uint16_t end_y;   /*按键的y结束坐标*/
  uint32_t para; /*颜色按钮中表示选择的颜色，笔迹形状按钮中表示选择的画刷*/
  uint8_t touch_flag;          /*按键按下的标志*/
  void (*draw_btn)(void *btn); /*按键描绘函数*/
  void (*btn_command)(void *btn); /*按键功能执行函数，例如切换颜色、画刷*/
} DrawingBoard_Button;

/*
    LCD 颜色代码，CL_是Color的简写
    16Bit由高位至低位， RRRR RGGG GGGB BBBB
*/
#if LCD_RGB_888
/*RGB888颜色转换*/
#define RGB(R, G, B)                                                           \
  ((R << 16) | (G << 8) | (B)) /* 将8位R,G,B转化为 24位RGB888格式 */

#else
/*RGB565 颜色转换*/
#define RGB(R, G, B)                                                           \
  (((R >> 3) << 11) | ((G >> 2) << 5) |                                        \
   (B >> 3)) /* 将8位R,G,B转化为 16位RGB565格式 */
#define RGB565_R(x) ((x >> 8) & 0xF8)
#define RGB565_G(x) ((x >> 3) & 0xFC)
#define RGB565_B(x) ((x << 3) & 0xF8)

#endif

enum {
  CL_WHITE = RGB(255, 255, 255),       /* 白色 */
  CL_BLACK = RGB(0, 0, 0),             /* 黑色 */
  CL_RED = RGB(255, 0, 0),             /* 红色 */
  CL_GREEN = RGB(0, 255, 0),           /* 绿色 */
  CL_BLUE = RGB(0, 0, 255),            /* 蓝色 */
  CL_YELLOW = RGB(255, 255, 0),        /* 黄色 */
  CL_GREY = RGB(98, 98, 98),           /* 深灰色 */
  CL_GREY1 = RGB(150, 150, 150),       /* 浅灰色 */
  CL_GREY2 = RGB(180, 180, 180),       /* 浅灰色 */
  CL_GREY3 = RGB(200, 200, 200),       /* 最浅灰色 */
  CL_GREY4 = RGB(230, 230, 230),       /* 最浅灰色 */
  CL_BUTTON_GREY = RGB(220, 220, 220), /* WINDOWS 按钮表面灰色 */
  CL_MAGENTA = RGB(255, 0, 255),       /* 红紫色，洋红色 */
  CL_CYAN = RGB(0, 255, 255),          /* 蓝绿色，青色 */
  CL_BLUE1 = RGB(0, 0, 240),           /* 深蓝色 */
  CL_BLUE2 = RGB(0, 0, 128),           /* 深蓝色 */
  CL_BLUE3 = RGB(68, 68, 255),         /* 浅蓝色1 */
  CL_BLUE4 = RGB(0, 64, 128),          /* 浅蓝色1 */
  CL_BTN_FACE = RGB(236, 233, 216),    /* 按钮表面颜色(灰) */
  CL_BOX_BORDER1 = RGB(172, 168, 153), /* 分组框主线颜色 */
  CL_BOX_BORDER2 = RGB(255, 255, 255), /* 分组框阴影线颜色 */
  CL_MASK = 0x7FFF /* RGB565颜色掩码，用于文字背景透明 */
};

/*DrawingBoard_brush全局变量在其它文件有使用到*/
extern DrawingBoard_Brush_Style DrawingBoard_brush;

/*滑板资源初始化*/
void DrawingBoard_Init(void);
/*初始化按钮参数*/
void DrawingBoard_Button_Init(void);
/*发送按下*/
void DrawingBoard_Button_Down(uint16_t x, uint16_t y);
/*发送按下到抬起*/
void DrawingBoard_Button_Up(uint16_t x, uint16_t y);
/*处理描绘轨迹，用于触摸画板*/
void DrawingBoard_Draw_Graph(int16_t pre_x, int16_t pre_y, int16_t x, int16_t y,
                             DrawingBoard_Brush_Style *DrawingBoard_brush);

#endif //_DrawingBoard_H
