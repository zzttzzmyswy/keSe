#include "DrawingBoard.h"
#include "bsp_lcd.h"

/*按钮结构体数组*/
DrawingBoard_Button button[BUTTON_NUM];

/*画笔参数*/
DrawingBoard_Brush_Style DrawingBoard_brush;

/*颜色按钮的描绘函数*/
static void Draw_Button_Color(void *btn);
/*清屏按钮绘制函数*/
static void Draw_Button_Clear(void *btn);
/*画笔按钮绘制函数*/
static void Draw_Button_Shape(void *btn);
/*橡皮按钮的描绘函数*/
static void Draw_Button_Eraser(void *btn);
/*矩形按钮的描绘函数*/
static void Draw_Button_Rect(void *btn);
/*选择颜色*/
static void Select_Shape_Color(void *btn);
/*选择笔刷类型*/
static void Select_Brush_Class(void *btn);
/*清屏*/
static void Clear_DrawingBoard_Area(void *btn);
/*描绘一定像素宽度的轨迹线*/
static void LCD_DrawUniLineCircle(uint16_t x1, uint16_t y1, uint16_t x2,
                                  uint16_t y2, uint8_t thick);
/*用于储存矩形起始点的静态变量*/
static int32_t Rect_x = -1, Rect_y = -1;

/**
 * @brief  DrawingBoard_Init 画板初始化
 * @param  无
 * @retval 无
 */
void DrawingBoard_Init(void) {
  uint8_t i;
  /* 整屏清为白色 */
  LCD_Clear(CL_WHITE); /* 清屏，显示全黑 */
  /* 初始化按钮 */
  DrawingBoard_Button_Init();
  /* 描绘按钮 */
  for (i = 0; i < BUTTON_NUM; i++)
    button[i].draw_btn(&button[i]);
  /* 初始化画笔 */
  DrawingBoard_brush.color = CL_BLACK;
  DrawingBoard_brush.shape = LINE_SINGLE_PIXCEL;
}

/*初始化按钮参数*/
void DrawingBoard_Button_Init(void) {
  /*笔刷相关按钮*/
  /*结构体参数如下*/
  /*左上角开始x*/
  /*左上角开始y*/
  /*右下角x*/
  /*右下角y*/
  /*按钮参数*/
  /*按钮绘制函数*/
  /*按钮回调函数*/
  /*橡皮檫*/
  button[0].start_x = 0;
  button[0].start_y = COLOR_BLOCK_HEIGHT * 0;
  button[0].end_x = COLOR_BLOCK_WIDTH * 1;
  button[0].end_y = COLOR_BLOCK_HEIGHT * 1;
  button[0].para = ERASER;
  button[0].touch_flag = 0;
  button[0].draw_btn = Draw_Button_Eraser;
  button[0].btn_command = Select_Brush_Class;
  /*矩形*/
  button[1].start_x = 0;
  button[1].start_y = COLOR_BLOCK_HEIGHT * 1;
  button[1].end_x = COLOR_BLOCK_WIDTH * 1;
  button[1].end_y = COLOR_BLOCK_HEIGHT * 2;
  button[1].para = RECT;
  button[1].touch_flag = 0;
  button[1].draw_btn = Draw_Button_Rect;
  button[1].btn_command = Select_Brush_Class;
  /*带圆线*/
  button[2].start_x = 0;
  button[2].start_y = COLOR_BLOCK_HEIGHT * 2;
  button[2].end_x = COLOR_BLOCK_WIDTH * 1;
  button[2].end_y = COLOR_BLOCK_HEIGHT * 3;
  button[2].para = LINE_WITH_CIRCLE;
  button[2].touch_flag = 0;
  button[2].draw_btn = Draw_Button_Shape;
  button[2].btn_command = Select_Brush_Class;
  /*宽度为1的线*/
  button[3].start_x = 0;
  button[3].start_y = COLOR_BLOCK_HEIGHT * 3;
  button[3].end_x = COLOR_BLOCK_WIDTH * 1;
  button[3].end_y = COLOR_BLOCK_HEIGHT * 4;
  button[3].para = LINE_SINGLE_PIXCEL;
  button[3].touch_flag = 0;
  button[3].draw_btn = Draw_Button_Shape;
  button[3].btn_command = Select_Brush_Class;
  /*宽度为2的线*/
  button[4].start_x = 0;
  button[4].start_y = COLOR_BLOCK_HEIGHT * 4;
  button[4].end_x = COLOR_BLOCK_WIDTH * 1;
  button[4].end_y = COLOR_BLOCK_HEIGHT * 5;
  button[4].para = LINE_2_PIXCEL;
  button[4].touch_flag = 0;
  button[4].draw_btn = Draw_Button_Shape;
  button[4].btn_command = Select_Brush_Class;
  /*宽度为4的线*/
  button[5].start_x = 0;
  button[5].start_y = COLOR_BLOCK_HEIGHT * 5;
  button[5].end_x = COLOR_BLOCK_WIDTH * 1;
  button[5].end_y = COLOR_BLOCK_HEIGHT * 6;
  button[5].para = LINE_4_PIXCEL;
  button[5].touch_flag = 0;
  button[5].draw_btn = Draw_Button_Shape;
  button[5].btn_command = Select_Brush_Class;
  /*宽度为6的线*/
  button[6].start_x = 0;
  button[6].start_y = COLOR_BLOCK_HEIGHT * 6;
  button[6].end_x = COLOR_BLOCK_WIDTH * 1;
  button[6].end_y = COLOR_BLOCK_HEIGHT * 7;
  button[6].para = LINE_6_PIXCEL;
  button[6].touch_flag = 0;
  button[6].draw_btn = Draw_Button_Shape;
  button[6].btn_command = Select_Brush_Class;
  /*宽度为8的线*/
  button[7].start_x = 0;
  button[7].start_y = COLOR_BLOCK_HEIGHT * 7;
  button[7].end_x = COLOR_BLOCK_WIDTH * 1;
  button[7].end_y = COLOR_BLOCK_HEIGHT * 8;
  button[7].para = LINE_8_PIXCEL;
  button[7].touch_flag = 0;
  button[7].draw_btn = Draw_Button_Shape;
  button[7].btn_command = Select_Brush_Class;
  /*宽度为16的线*/
  button[8].start_x = 0;
  button[8].start_y = COLOR_BLOCK_HEIGHT * 8;
  button[8].end_x = COLOR_BLOCK_WIDTH * 1;
  button[8].end_y = DRAWING_BOARD_END_Y;
  button[8].para = LINE_16_PIXCEL;
  button[8].touch_flag = 0;
  button[8].draw_btn = Draw_Button_Shape;
  button[8].btn_command = Select_Brush_Class;
  /*清屏和颜色按钮*/
  /*清屏*/
  button[9].start_x = LCD_PIXEL_WIDTH - COLOR_BLOCK_WIDTH;
  button[9].start_y = COLOR_BLOCK_HEIGHT * 0;
  button[9].end_x = LCD_PIXEL_WIDTH - 1;
  button[9].end_y = COLOR_BLOCK_HEIGHT * 1;
  button[9].para = CL_BUTTON_GREY;
  button[9].touch_flag = 0;
  button[9].draw_btn = Draw_Button_Clear;
  button[9].btn_command = Clear_DrawingBoard_Area;
  /*红色*/
  button[10].start_x = LCD_PIXEL_WIDTH - COLOR_BLOCK_WIDTH;
  button[10].start_y = COLOR_BLOCK_HEIGHT * 1;
  button[10].end_x = LCD_PIXEL_WIDTH - 1;
  button[10].end_y = COLOR_BLOCK_HEIGHT * 2;
  button[10].para = CL_RED;
  button[10].touch_flag = 0;
  button[10].draw_btn = Draw_Button_Color;
  button[10].btn_command = Select_Shape_Color;
  /*绿色*/
  button[11].start_x = LCD_PIXEL_WIDTH - COLOR_BLOCK_WIDTH;
  button[11].start_y = COLOR_BLOCK_HEIGHT * 2;
  button[11].end_x = LCD_PIXEL_WIDTH - 1;
  button[11].end_y = COLOR_BLOCK_HEIGHT * 3;
  button[11].para = CL_GREEN;
  button[11].touch_flag = 0;
  button[11].draw_btn = Draw_Button_Color;
  button[11].btn_command = Select_Shape_Color;
  /*蓝色*/
  button[12].start_x = LCD_PIXEL_WIDTH - COLOR_BLOCK_WIDTH;
  button[12].start_y = COLOR_BLOCK_HEIGHT * 3;
  button[12].end_x = LCD_PIXEL_WIDTH - 1;
  button[12].end_y = COLOR_BLOCK_HEIGHT * 4;
  button[12].para = CL_BLUE;
  button[12].touch_flag = 0;
  button[12].draw_btn = Draw_Button_Color;
  button[12].btn_command = Select_Shape_Color;
  /*紫色*/
  button[13].start_x = LCD_PIXEL_WIDTH - COLOR_BLOCK_WIDTH;
  button[13].start_y = COLOR_BLOCK_HEIGHT * 4;
  button[13].end_x = LCD_PIXEL_WIDTH - 1;
  button[13].end_y = COLOR_BLOCK_HEIGHT * 5;
  button[13].para = CL_MAGENTA;
  button[13].touch_flag = 0;
  button[13].draw_btn = Draw_Button_Color;
  button[13].btn_command = Select_Shape_Color;
  /*黄色*/
  button[14].start_x = LCD_PIXEL_WIDTH - COLOR_BLOCK_WIDTH;
  button[14].start_y = COLOR_BLOCK_HEIGHT * 5;
  button[14].end_x = LCD_PIXEL_WIDTH - 1;
  button[14].end_y = COLOR_BLOCK_HEIGHT * 6;
  button[14].para = CL_YELLOW;
  button[14].touch_flag = 0;
  button[14].draw_btn = Draw_Button_Color;
  button[14].btn_command = Select_Shape_Color;
  /*青色*/
  button[15].start_x = LCD_PIXEL_WIDTH - COLOR_BLOCK_WIDTH;
  button[15].start_y = COLOR_BLOCK_HEIGHT * 6;
  button[15].end_x = LCD_PIXEL_WIDTH - 1;
  button[15].end_y = COLOR_BLOCK_HEIGHT * 7;
  button[15].para = CL_CYAN;
  button[15].touch_flag = 0;
  button[15].draw_btn = Draw_Button_Color;
  button[15].btn_command = Select_Shape_Color;
  /*黑色*/
  button[16].start_x = LCD_PIXEL_WIDTH - COLOR_BLOCK_WIDTH;
  button[16].start_y = COLOR_BLOCK_HEIGHT * 7;
  button[16].end_x = LCD_PIXEL_WIDTH - 1;
  button[16].end_y = COLOR_BLOCK_HEIGHT * 8;
  button[16].para = CL_BLACK;
  button[16].touch_flag = 0;
  button[16].draw_btn = Draw_Button_Color;
  button[16].btn_command = Select_Shape_Color;
  /*灰色*/
  button[17].start_x = LCD_PIXEL_WIDTH - COLOR_BLOCK_WIDTH;
  button[17].start_y = COLOR_BLOCK_HEIGHT * 8;
  button[17].end_x = LCD_PIXEL_WIDTH - 1;
  button[17].end_y = DRAWING_BOARD_END_Y;
  button[17].para = CL_GREY;
  button[17].touch_flag = 0;
  button[17].draw_btn = Draw_Button_Color;
  button[17].btn_command = Select_Shape_Color;
}

/*按键被按下时调用的函数，由触摸屏调用*/
void DrawingBoard_Button_Down(uint16_t x, uint16_t y) {
  uint8_t i;
  for (i = 0; i < BUTTON_NUM; i++) {
    /* 触摸到了按钮 */
    if (x <= button[i].end_x && y <= button[i].end_y &&
        y >= button[i].start_y && x >= button[i].start_x) {
      if (button[i].touch_flag == 0) { /*原本的状态为没有按下，则更新状态*/
        button[i].touch_flag = 1;       /* 记录按下标志 */
        button[i].draw_btn(&button[i]); /*重绘按钮*/
      }
    } else if (button[i].touch_flag ==
               1) { /* 触摸移出了按键的范围且之前有按下按钮 */
      button[i].touch_flag = 0; /* 清除按下标志，判断为误操作*/
      button[i].draw_btn(&button[i]); /*重绘按钮*/
    }
  }
}

/*按键被释放时调用的函数，由触摸屏调用*/
void DrawingBoard_Button_Up(uint16_t x, uint16_t y) {
  uint8_t i;
  for (i = 0; i < BUTTON_NUM; i++) {
    /* 触笔在按钮区域释放 */
    if ((x < button[i].end_x && x > button[i].start_x && y < button[i].end_y &&
         y > button[i].start_y)) {
      button[i].touch_flag = 0;          /*释放触摸标志*/
      button[i].draw_btn(&button[i]);    /*重绘按钮*/
      button[i].btn_command(&button[i]); /*执行按键的功能命令*/
      break;
    }
  }
  if (x > DRAWING_BOARD_START_X) {
    /* 触笔在画板区域释放 */
    switch (DrawingBoard_brush.shape) { /*根据画刷参数描绘不同的轨迹*/
    /* 描绘1像素宽度的轨迹线 */
    case RECT:
      if (x - 10 < DRAWING_BOARD_START_X) // 矩形
        break;
      if (Rect_x != x && Rect_y != y) {
        if (!(x > DRAWING_BOARD_END_X - 10 || y > DRAWING_BOARD_END_Y - 10 ||
              x < 10 || y < 10))
          LCD_DrawRect(Rect_x < x ? Rect_x : x, Rect_y < y ? Rect_y : y,
                       x - Rect_x > 0 ? x - Rect_x : Rect_x - x,
                       y - Rect_y > 0 ? y - Rect_y : Rect_y - y);
      }
      Rect_x = -1;
      Rect_y = -1;
      break;
    default:
      break;
    }
  }
}

/*在画板区域描绘图形*/
void DrawingBoard_Draw_Graph(int16_t pre_x, int16_t pre_y, int16_t x, int16_t y,
                             DrawingBoard_Brush_Style *DrawingBoard_brush) {
  /*设置画板区域为活动窗口,bsp_lcd.c驱动还没有这样的函数，用于限制绘画窗口*/
  //  RA8875_SetActiveWindow(DRAWING_BOARD_START_X,DRAWING_BOARD_START_Y,DRAWING_BOARD_END_X,DRAWING_BOARD_END_Y);
  /*触摸位置在画板区域*/
  if (x > DRAWING_BOARD_START_X && pre_x > DRAWING_BOARD_START_X &&
      x < DRAWING_BOARD_END_X && pre_x < DRAWING_BOARD_END_X) {
    switch (DrawingBoard_brush->shape) { /*根据画刷参数描绘不同的轨迹*/
    /* 描绘1像素宽度的线 */
    case LINE_SINGLE_PIXCEL:
      if (pre_x < 0 || pre_y < 0) // 新的笔迹
        PutPixel(x, y);
      else // 继续上一次的笔迹
        LCD_DrawUniLine(pre_x, pre_y, x, y);
      break;
    case LINE_2_PIXCEL:
      if (x - 1 < DRAWING_BOARD_START_X ||
          pre_x - 1 < DRAWING_BOARD_START_X) // 画板左边界
        break;
      LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 1);
      break;
    case LINE_4_PIXCEL:
      if (x - 2 < DRAWING_BOARD_START_X ||
          pre_x - 2 < DRAWING_BOARD_START_X) // 画板左边界
        break;
      LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 2);
      break;
    case LINE_6_PIXCEL:
      if (x - 3 < DRAWING_BOARD_START_X ||
          pre_x - 3 < DRAWING_BOARD_START_X) // 画板左边界
        break;
      LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 3);
      break;
    case LINE_8_PIXCEL:
      if (x - 4 < DRAWING_BOARD_START_X ||
          pre_x - 4 < DRAWING_BOARD_START_X) // 画板左边界
        break;
      LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 4);
      break;
    case LINE_16_PIXCEL:
      if (x - 8 < DRAWING_BOARD_START_X ||
          pre_x - 8 < DRAWING_BOARD_START_X) // 画板左边界
        break;
      LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 8);
      break;
    /*绘制矩形*/
    case RECT:
      if (x - 10 < DRAWING_BOARD_START_X ||
          pre_x - 10 < DRAWING_BOARD_START_X) // 画板左边界
        break;
      if (Rect_x == -1) {
        if (!(x > DRAWING_BOARD_END_X - 10 || y > DRAWING_BOARD_END_Y - 10 ||
              x < 10 || y < 10)) {
          /*记录矩形起点*/
          Rect_x = x;
          Rect_y = y;
        }
      }
      break;
    /*绘制带圆的线*/
    case LINE_WITH_CIRCLE:
      if (x - 3 < DRAWING_BOARD_START_X || pre_x - 3 < DRAWING_BOARD_START_X ||
          y - 3 < DRAWING_BOARD_START_Y || pre_y - 3 < DRAWING_BOARD_START_Y ||
          y + 3 > DRAWING_BOARD_END_Y || pre_y + 3 > DRAWING_BOARD_END_Y)
        break;
      if (pre_x < 0 || pre_y < 0) // 新的笔迹
        PutPixel(x, y);
      else { // 继续上一次的笔迹
        LCD_DrawUniLine(pre_x, pre_y, x, y);
        LCD_DrawFullCircle(x, y, 3);
      }
      break;
    /*橡皮功能*/
    case ERASER:
      if (x - 15 < DRAWING_BOARD_START_X || x + 15 > DRAWING_BOARD_END_X ||
          y + 15 > DRAWING_BOARD_END_Y || y - 15 < DRAWING_BOARD_START_Y)
        break;
      LCD_SetColors(CL_WHITE, CL_BUTTON_GREY);
      LCD_DrawFullRect(x - 30 / 2, y - 30 / 2, 30, 30);
      break;
    }
  }
}

/*颜色按钮的描绘函数*/
static void Draw_Button_Color(void *btn) {
  DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
  uint16_t colors1, colors2;
  /*保存之前的色彩参数*/
  LCD_GetColors(&colors1, &colors2);
  /*释放按键*/
  if (ptr->touch_flag == 0) {
    /*背景为功能键相应的颜色*/
    LCD_SetColors(ptr->para, CL_WHITE);
    LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                     ptr->end_y - ptr->start_y);
  } else { /*按键按下*/
    /*白色背景*/
    LCD_SetColors(CL_WHITE, CL_WHITE);
    LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                     ptr->end_y - ptr->start_y);
  }
  /*按钮边框*/
  LCD_SetColors(CL_BLUE4, CL_WHITE);
  LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
               ptr->end_y - ptr->start_y);
  /*复原原先的色彩参数*/
  LCD_SetColors(colors1, colors2);
}

/*清屏按钮的描绘函数*/
static void Draw_Button_Clear(void *btn) {
  DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
  uint16_t colors1, colors2;
  /*保存之前的色彩参数*/
  LCD_GetColors(&colors1, &colors2);
  /*释放按键*/
  if (ptr->touch_flag == 0) {
    LCD_SetColors(CL_BUTTON_GREY, CL_WHITE);
    LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                     ptr->end_y - ptr->start_y);
    LCD_SetColors(CL_RED, CL_BUTTON_GREY);
    LCD_SetFont(&Font8x8);
    LCD_DispString_EN(ptr->start_y + 11,
                      ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2,
                      (uint8_t *)"CLEAR");
  } else { /*按键按下*/
    LCD_SetColors(CL_WHITE, CL_WHITE);
    LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                     ptr->end_y - ptr->start_y);
    LCD_SetColors(CL_RED, CL_WHITE);
    LCD_SetFont(&Font8x8);
    LCD_DispString_EN(ptr->start_y + 11,
                      ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2,
                      (uint8_t *)"CLEAR");
  }
  /*按钮边框*/
  LCD_SetColors(CL_BLUE4, CL_WHITE);
  LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
               ptr->end_y - ptr->start_y);
  /*复原原先的色彩参数*/
  LCD_SetColors(colors1, colors2);
}

/*橡皮擦按钮的描绘函数*/
static void Draw_Button_Eraser(void *btn) {
  DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
  uint16_t colors1, colors2;
  /*保存之前的色彩参数*/
  LCD_GetColors(&colors1, &colors2);
  /*释放按键*/
  if (ptr->touch_flag == 0) {
    LCD_SetColors(CL_BUTTON_GREY, CL_WHITE);
    LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                     ptr->end_y - ptr->start_y);
    LCD_SetColors(CL_RED, CL_BUTTON_GREY);
    LCD_SetFont(&Font8x8);
    LCD_DispString_EN(ptr->start_y + 11,
                      ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2,
                      (uint8_t *)"ERASER");
  } else { /*按键按下*/
    LCD_SetColors(CL_WHITE, CL_WHITE);
    LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                     ptr->end_y - ptr->start_y);
    LCD_SetColors(CL_RED, CL_WHITE);
    LCD_SetFont(&Font8x8);
    LCD_DispString_EN(ptr->start_y + 11,
                      ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2,
                      (uint8_t *)"ERASER");
  }
  /*按钮边框*/
  LCD_SetColors(CL_BLUE4, CL_WHITE);
  LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
               ptr->end_y - ptr->start_y);
  /*复原原先的色彩参数*/
  LCD_SetColors(colors1, colors2);
}

/*矩形按钮的描绘函数*/
static void Draw_Button_Rect(void *btn) {
  DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
  uint16_t colors1, colors2;
  /*保存之前的色彩参数*/
  LCD_GetColors(&colors1, &colors2);
  /*释放按键*/
  if (ptr->touch_flag == 0) {
    LCD_SetColors(CL_BUTTON_GREY, CL_WHITE);
    LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                     ptr->end_y - ptr->start_y);
    LCD_SetColors(CL_RED, CL_BUTTON_GREY);
    LCD_SetFont(&Font8x8);
    LCD_DispString_EN(ptr->start_y + 11,
                      ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2,
                      (uint8_t *)"RECT");
  } else { /*按键按下*/
    LCD_SetColors(CL_WHITE, CL_WHITE);
    LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                     ptr->end_y - ptr->start_y);
    LCD_SetColors(CL_RED, CL_WHITE);
    LCD_SetFont(&Font8x8);
    LCD_DispString_EN(ptr->start_y + 11,
                      ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2,
                      (uint8_t *)"RECT");
  }
  /*按钮边框*/
  LCD_SetColors(CL_BLUE4, CL_WHITE);
  LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
               ptr->end_y - ptr->start_y);
  /*复原原先的色彩参数*/
  LCD_SetColors(colors1, colors2);
}

/*笔刷按钮的描绘函数*/
static void Draw_Button_Shape(void *btn) {
  DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
  uint16_t i;
  uint16_t colors1, colors2;
  /*保存之前的色彩参数*/
  LCD_GetColors(&colors1, &colors2);
  /* 背景颜色 没按下时为灰色，按下时为白色*/
  if (ptr->touch_flag == 0) {
    LCD_SetColors(CL_BUTTON_GREY, CL_WHITE);
    LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                     ptr->end_y - ptr->start_y);
    /*显示文字时的背景颜色*/
    LCD_SetColors(CL_BLUE4, CL_BUTTON_GREY);
    LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                 ptr->end_y - ptr->start_y);
  } else {
    LCD_SetColors(CL_WHITE, CL_WHITE);
    LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                     ptr->end_y - ptr->start_y);
    /*显示文字时的背景颜色*/
    LCD_SetColors(CL_BLUE4, CL_WHITE);
    LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
                 ptr->end_y - ptr->start_y);
  }
  LCD_SetColors(CL_BLACK, CL_WHITE);
  /*根据画刷形状描绘按钮图案*/
  switch (ptr->para) {
  case LINE_SINGLE_PIXCEL:
    LCD_SetColors(CL_BLACK, CL_WHITE);
    LCD_DrawUniLine(
        ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
        ptr->end_x - 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2);
    break;
  case LINE_2_PIXCEL:
    LCD_DrawUniLineCircle(
        ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
        ptr->end_x - 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 1);
    break;
  case LINE_4_PIXCEL:
    LCD_DrawUniLineCircle(
        ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
        ptr->end_x - 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 2);
    break;
  case LINE_6_PIXCEL:
    LCD_DrawUniLineCircle(
        ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
        ptr->end_x - 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 3);
    break;
  case LINE_8_PIXCEL:
    LCD_DrawUniLineCircle(
        ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
        ptr->end_x - 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 4);
    break;
  case LINE_16_PIXCEL:
    LCD_DrawUniLineCircle(
        ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
        ptr->end_x - 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 8);
    break;
  case RECT: /*绘制矩形按钮*/
    LCD_SetColors(CL_WHITE, CL_BLACK);
    LCD_DrawFullRect(ptr->start_x + ((ptr->end_x - ptr->start_x - 40) / 2),
                     ptr->start_y + ((ptr->end_y - ptr->start_y - 40 - 30) / 2),
                     40, 40);
    LCD_SetColors(CL_RED, CL_BUTTON_GREY);
    LCD_SetFont(&Font8x8);
    LCD_DispString_EN(ptr->end_y - 19,
                      ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2,
                      (uint8_t *)"Rect");
    break;
    break;
  case LINE_WITH_CIRCLE: /*绘制带圆的线按钮*/
    LCD_SetColors(CL_BLACK, CL_WHITE);
    LCD_DrawUniLine(
        ptr->start_x + 5, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
        ptr->end_x - 5, ptr->start_y + (ptr->end_y - ptr->start_y) / 2);
    for (i = 0; i < ((ptr->end_x - ptr->start_x - 10) / 10); i++) {
      LCD_DrawFullCircle(ptr->start_x + 10 + i * 10,
                         ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 3);
    }
    break;
  case ERASER: /*绘制橡皮檫按钮*/
    LCD_SetColors(CL_WHITE, CL_BLACK);
    LCD_DrawFullRect(ptr->start_x + ((ptr->end_x - ptr->start_x - 40) / 2),
                     ptr->start_y + ((ptr->end_y - ptr->start_y - 40 - 30) / 2),
                     40, 40);
    LCD_SetColors(CL_RED, CL_BUTTON_GREY);
    LCD_SetFont(&Font8x8);
    LCD_DispString_EN(ptr->end_y - 19,
                      ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2,
                      (uint8_t *)"ERASER");
    break;
  }
  /*复原原先的色彩参数*/
  LCD_SetColors(colors1, colors2);
}

/*切换画刷颜色*/
static void Select_Shape_Color(void *btn) {
  DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
  DrawingBoard_brush.color = ptr->para;
  LCD_SetColors(ptr->para, CL_WHITE);
  if (DrawingBoard_brush.shape == ERASER)
    DrawingBoard_brush.shape = LINE_SINGLE_PIXCEL;
}

/*切换画刷类*/
static void Select_Brush_Class(void *btn) {
  DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
  DrawingBoard_brush.shape = (SHAPE)ptr->para;
}

/*切换画刷颜色*/
static void Clear_DrawingBoard_Area(void *btn) {
  LCD_SetColors(CL_WHITE, CL_WHITE);
  LCD_DrawFullRect(DRAWING_BOARD_START_X, DRAWING_BOARD_START_Y,
                   DRAWING_BOARD_END_X - (DRAWING_BOARD_START_X) + 1,
                   DRAWING_BOARD_END_Y - DRAWING_BOARD_START_Y + 1);
}

#define ABS(X) ((X) > 0 ? (X) : -(X))

/*在两点之间描绘线*/
static void LCD_DrawUniLineCircle(uint16_t x1, uint16_t y1, uint16_t x2,
                                  uint16_t y2, uint8_t thick) {
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 = 0,
          yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, curpixel = 0;
  deltax = ABS(x2 - x1); /* The difference between the x's */
  deltay = ABS(y2 - y1); /* The difference between the y's */
  x = x1;                /* Start x off at the first pixel */
  y = y1;                /* Start y off at the first pixel */
  if (x2 >= x1) {        /* The x-values are increasing */
    xinc1 = 1;
    xinc2 = 1;
  } else { /* The x-values are decreasing */
    xinc1 = -1;
    xinc2 = -1;
  }
  if (y2 >= y1) { /* The y-values are increasing */
    yinc1 = 1;
    yinc2 = 1;
  } else { /* The y-values are decreasing */
    yinc1 = -1;
    yinc2 = -1;
  }
  if (deltax >= deltay) { /* There is at least one x-value for every y-value */
    xinc1 = 0;            /* Don't change the x when numerator >= denominator */
    yinc2 = 0;            /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax; /* There are more x-values than y-values */
  } else {              /* There is at least one y-value for every x-value */
    xinc2 = 0;          /* Don't change the x for every iteration */
    yinc1 = 0;          /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay; /* There are more y-values than x-values */
  }
  for (curpixel = 0; curpixel <= numpixels; curpixel++) {
    // 判断边界
    if (x + thick > DRAWING_BOARD_END_X || x - thick < 0 || // 液晶左右边界
        y + thick > DRAWING_BOARD_END_Y || y - thick < 0) // 液晶上下边界
      continue;
    LCD_DrawFullCircle(x, y, thick); /* Draw the current pixel */
    num += numadd;    /* Increase the numerator by the top of the fraction */
    if (num >= den) { /* Check if numerator >= denominator */
      num -= den;     /* Calculate the new numerator value */
      x += xinc1;     /* Change the x as appropriate */
      y += yinc1;     /* Change the y as appropriate */
    }
    x += xinc2; /* Change the x as appropriate */
    y += yinc2; /* Change the y as appropriate */
  }
}
