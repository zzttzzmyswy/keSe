#include "DrawingBoard.h"
#include "bsp_lcd.h"

/*按钮结构体数组*/
static DrawingBoard_Button button[BUTTON_NUM];
/*画笔参数*/
static DrawingBoard_Brush_Style DrawingBoard_brush;

/*按钮资源注册函数*/
static void DrawingBoard_Button_Init(void);
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
static void LCD_DrawUniLineCircle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t thick);
/*处理描绘轨迹，用于触摸画板*/
static void
DrawingBoard_Draw_Graph(int16_t pre_x, int16_t pre_y, int16_t x, int16_t y,
	DrawingBoard_Brush_Style *DrawingBoard_brush);
/*用于储存矩形起始点的静态变量*/
static int32_t Rect_x = -1, Rect_y = -1;

/**
 * @brief  画板初始化
 * @retval None
 */
void DrawingBoard_Init(void) {
	uint8_t i;
	/* 整屏清为白色 */
	LCD_Clear(COLOR_WHITE); /* 清屏，显示全黑 */
	/* 初始化按钮 */
	DrawingBoard_Button_Init();
	/* 描绘按钮 */
	for (i = 0; i < BUTTON_NUM; i++)
		button[i].draw_btn(&button[i]);
	/* 初始化画笔 */
	DrawingBoard_brush.color = COLOR_BLACK;
	DrawingBoard_brush.brush_shape = LINE_SINGLE_PIXCEL;
}

/**
 * @brief  按键被按下时调用的函数
 * @param  第一个和第二个参数为上一个点，第三个和第四个参数为当前的点
 * @retval None
 */
void DrawingBoard_Button_Down(int16_t pre_x, int16_t pre_y, int16_t x,
	int16_t y) {
	uint8_t i;
	for (i = 0; i < BUTTON_NUM; i++) {
		/* 触摸到了按钮 */
		if (x <= button[i].end_x && y <= button[i].end_y &&
			y >= button[i].start_y && x >= button[i].start_x) {
			if (button[i].touch_flag == 0) { /*原本的状态为没有按下，则更新状态*/
				button[i].touch_flag = 1;       /* 记录按下标志 */
				button[i].draw_btn(&button[i]); /*重绘按钮*/
			}
		}
		else if (button[i].touch_flag ==
			1) { /* 触摸移出了按键的范围且之前有按下按钮 */
			button[i].touch_flag = 0; /* 清除按下标志，判断为误操作*/
			button[i].draw_btn(&button[i]); /*重绘按钮*/
		}
	}
	/*进行图形绘制*/
	DrawingBoard_Draw_Graph(pre_x, pre_y, x, y, &DrawingBoard_brush);
}

/**
 * @brief  按键被释放时调用的函数
 * @param  第一个和第二个参数当前的点
 * @retval None
 */
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
	if (x > DRAWING_BOARD_START_X && x < DRAWING_BOARD_END_X) {
		/* 触笔在画板区域释放 */
		switch (DrawingBoard_brush.brush_shape) { /*根据画刷参数描绘不同的轨迹*/
		/* 描绘矩形 */
		case RECT:
			if (x - 10 < DRAWING_BOARD_START_X ||
				x + 10 > DRAWING_BOARD_END_X) // 划定边界范围
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

/**
 * @brief  在画板区域描绘图形
 * @param
 * 第一个和第二个参数为上一个点，第三个和第四个参数为当前的点，第五个参数为笔刷结构体指针
 * @retval None
 */
static void
DrawingBoard_Draw_Graph(int16_t pre_x, int16_t pre_y, int16_t x, int16_t y,
	DrawingBoard_Brush_Style *DrawingBoard_brush) {
	/*触摸位置在画板区域*/
	if (x > DRAWING_BOARD_START_X && pre_x > DRAWING_BOARD_START_X
		&& x < DRAWING_BOARD_END_X
		&& pre_x < DRAWING_BOARD_END_X) {
		switch (DrawingBoard_brush->brush_shape) { /*根据画刷参数描绘不同的轨迹*/
		/* 描绘1像素宽度的线 */
		case LINE_SINGLE_PIXCEL:
			if (pre_x < 0 || pre_y < 0) // 新的笔迹
				PutPixel(x, y);
			else // 继续上一次的笔迹
				LCD_DrawUniLine(pre_x, pre_y, x, y);
			break;
		case LINE_2_PIXCEL:
			if (x - 1 < DRAWING_BOARD_START_X
				|| pre_x - 1 < DRAWING_BOARD_START_X) // 划定边界范围
				break;
			LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 1);
			break;
		case LINE_4_PIXCEL:
			if (x - 2 < DRAWING_BOARD_START_X
				|| pre_x - 2 < DRAWING_BOARD_START_X) // 划定边界范围
				break;
			LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 2);
			break;
		case LINE_6_PIXCEL:
			if (x - 3 < DRAWING_BOARD_START_X
				|| pre_x - 3 < DRAWING_BOARD_START_X) // 划定边界范围
				break;
			LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 3);
			break;
		case LINE_8_PIXCEL:
			if (x - 4 < DRAWING_BOARD_START_X
				|| pre_x - 4 < DRAWING_BOARD_START_X) // 划定边界范围
				break;
			LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 4);
			break;
		case LINE_16_PIXCEL:
			if (x - 8 < DRAWING_BOARD_START_X
				|| pre_x - 8 < DRAWING_BOARD_START_X) // 划定边界范围
				break;
			LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 8);
			break;
		/*绘制矩形*/
		case RECT:
			if (x - 10 < DRAWING_BOARD_START_X
				|| pre_x - 10 < DRAWING_BOARD_START_X) // 划定边界范围
				break;
			if (Rect_x == -1) {
				if (!(x > DRAWING_BOARD_END_X - 10 || y > DRAWING_BOARD_END_Y - 10
						|| x - 10 < DRAWING_BOARD_START_X
						|| y - 10 < DRAWING_BOARD_START_Y)) {
					/*记录矩形起点*/
					Rect_x = x;
					Rect_y = y;
				}
			}
			break;
		/*绘制带圆的线*/
		case LINE_WITH_CIRCLE:
			if (x - 3 < DRAWING_BOARD_START_X || pre_x - 3 < DRAWING_BOARD_START_X
				|| y - 3 < DRAWING_BOARD_START_Y
				|| pre_y - 3 < DRAWING_BOARD_START_Y || y + 3 > DRAWING_BOARD_END_Y
				|| pre_y + 3 > DRAWING_BOARD_END_Y)
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
			if (x - 15 < DRAWING_BOARD_START_X || x + 15 > DRAWING_BOARD_END_X
				|| y + 15 > DRAWING_BOARD_END_Y
				|| y - 15 < DRAWING_BOARD_START_Y)
				break;
			LCD_SetColors(COLOR_WHITE, COLOR_BUTTON_GREY);
			LCD_DrawFullRect(x - 30 / 2, y - 30 / 2, 30, 30);
			break;
		}
	}
}

/**
 * @brief  颜色按钮的描绘函数
 * @param  按钮数据结构体指针
 * @retval None
 */
static void Draw_Button_Color(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	uint16_t colors1, colors2;
	/*保存之前的色彩参数*/
	LCD_GetColors(&colors1, &colors2);
	/*释放按键*/
	if (ptr->touch_flag == 0) {
		/*背景为功能键相应的颜色*/
		LCD_SetColors(ptr->para, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
	}
	else {   /*按键按下*/
		/*白色背景*/
		LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
	}
	/*按钮边框*/
	LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
	LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
		ptr->end_y - ptr->start_y);
	/*复原原先的色彩参数*/
	LCD_SetColors(colors1, colors2);
}

/**
 * @brief  清屏按钮的描绘函数
 * @param  按钮数据结构体指针
 * @retval None
 */
static void Draw_Button_Clear(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	uint16_t colors1, colors2;
	/*保存之前的色彩参数*/
	LCD_GetColors(&colors1, &colors2);
	/*释放按键*/
	if (ptr->touch_flag == 0) {
		LCD_SetColors(COLOR_BUTTON_GREY, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"CLEAR");
	}
	else {   /*按键按下*/
		LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"CLEAR");
	}
	/*按钮边框*/
	LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
	LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
		ptr->end_y - ptr->start_y);
	/*复原原先的色彩参数*/
	LCD_SetColors(colors1, colors2);
}

/**
 * @brief  橡皮擦按钮的描绘函数
 * @param  按钮数据结构体指针
 * @retval None
 */
static void Draw_Button_Eraser(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	uint16_t colors1, colors2;
	/*保存之前的色彩参数*/
	LCD_GetColors(&colors1, &colors2);
	/*释放按键*/
	if (ptr->touch_flag == 0) {
		LCD_SetColors(COLOR_BUTTON_GREY, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"ERASER");
	}
	else {   /*按键按下*/
		LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"ERASER");
	}
	/*按钮边框*/
	LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
	LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
		ptr->end_y - ptr->start_y);
	/*复原原先的色彩参数*/
	LCD_SetColors(colors1, colors2);
}

/**
 * @brief  矩形按钮的描绘函数
 * @param  按钮数据结构体指针
 * @retval None
 */
static void Draw_Button_Rect(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	uint16_t colors1, colors2;
	/*保存之前的色彩参数*/
	LCD_GetColors(&colors1, &colors2);
	/*释放按键*/
	if (ptr->touch_flag == 0) {
		LCD_SetColors(COLOR_BUTTON_GREY, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"RECT");
	}
	else {   /*按键按下*/
		LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"RECT");
	}
	/*按钮边框*/
	LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
	LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
		ptr->end_y - ptr->start_y);
	/*复原原先的色彩参数*/
	LCD_SetColors(colors1, colors2);
}

/**
 * @brief  笔刷按钮的描绘函数
 * @param  按钮数据结构体指针
 * @retval None
 */
static void Draw_Button_Shape(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	uint16_t i;
	uint16_t colors1, colors2;
	/*保存之前的色彩参数*/
	LCD_GetColors(&colors1, &colors2);
	/* 背景颜色 没按下时为灰色，按下时为白色*/
	if (ptr->touch_flag == 0) {
		LCD_SetColors(COLOR_BUTTON_GREY, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		/*绘制边框矩形*/
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
	}
	else {
		LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		/*绘制边框矩形*/
		LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
		LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
	}
	LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
	/*根据画刷形状描绘按钮图案*/
	switch (ptr->para) {
	case LINE_SINGLE_PIXCEL:
		LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
		LCD_DrawUniLine(
			ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
			ptr->end_x - 20,
			ptr->start_y + (ptr->end_y - ptr->start_y) / 2);
		break;
	case LINE_2_PIXCEL:
		LCD_DrawUniLineCircle(
			ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
			ptr->end_x - 20,
			ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 1);
		break;
	case LINE_4_PIXCEL:
		LCD_DrawUniLineCircle(
			ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
			ptr->end_x - 20,
			ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 2);
		break;
	case LINE_6_PIXCEL:
		LCD_DrawUniLineCircle(
			ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
			ptr->end_x - 20,
			ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 3);
		break;
	case LINE_8_PIXCEL:
		LCD_DrawUniLineCircle(
			ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
			ptr->end_x - 20,
			ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 4);
		break;
	case LINE_16_PIXCEL:
		LCD_DrawUniLineCircle(
			ptr->start_x + 20, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
			ptr->end_x - 20,
			ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 8);
		break;
	case RECT: /*绘制矩形按钮*/
		LCD_SetColors(COLOR_WHITE, COLOR_BLACK);
		LCD_DrawFullRect(ptr->start_x + ((ptr->end_x - ptr->start_x - 40) / 2),
			ptr->start_y + ((ptr->end_y - ptr->start_y - 40 - 30) / 2), 40, 40);
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->end_y - 19,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"Rect");
		break;
		break;
	case LINE_WITH_CIRCLE: /*绘制带圆的线按钮*/
		LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
		LCD_DrawUniLine(
			ptr->start_x + 5, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
			ptr->end_x - 5,
			ptr->start_y + (ptr->end_y - ptr->start_y) / 2);
		for (i = 0; i < ((ptr->end_x - ptr->start_x - 10) / 10); i++)
			LCD_DrawFullCircle(ptr->start_x + 10 + i * 10,
				ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 3);
		break;
	case ERASER: /*绘制橡皮檫按钮*/
		LCD_SetColors(COLOR_WHITE, COLOR_BLACK);
		LCD_DrawFullRect(ptr->start_x + ((ptr->end_x - ptr->start_x - 40) / 2),
			ptr->start_y + ((ptr->end_y - ptr->start_y - 40 - 30) / 2), 40, 40);
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->end_y - 19,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"ERASER");
		break;
	}
	/*复原原先的色彩参数*/
	LCD_SetColors(colors1, colors2);
}

/**
 * @brief  切换画刷颜色
 * @param  按钮数据结构体指针
 * @retval None
 */
static void Select_Shape_Color(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	DrawingBoard_brush.color = ptr->para;
	LCD_SetColors(ptr->para, COLOR_WHITE);
	if (DrawingBoard_brush.brush_shape == ERASER)
		DrawingBoard_brush.brush_shape = LINE_SINGLE_PIXCEL;
}

/**
 * @brief  切换画刷类型
 * @param  按钮数据结构体指针
 * @retval None
 */
static void Select_Brush_Class(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	DrawingBoard_brush.brush_shape = (BRUSH_SHAPE)ptr->para;
}

/**
 * @brief  切换画刷颜色
 * @param  按钮数据结构体指针
 * @retval None
 */
static void Clear_DrawingBoard_Area(void *btn) {
	LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
	LCD_DrawFullRect(DRAWING_BOARD_START_X, DRAWING_BOARD_START_Y,
		DRAWING_BOARD_END_X - (DRAWING_BOARD_START_X) + 1,
		DRAWING_BOARD_END_Y - DRAWING_BOARD_START_Y + 1);
}

/**
 * @brief  初始化按钮参数
 * @param  None
 * @retval None
 */
static void DrawingBoard_Button_Init(void) {
	/*初始化按钮参数,由于太长了所以用宏定义代替*/
	DrawingBoard_BUTTON_INIT_CODE;
}

/**
 * @brief  在两点之间描绘线
 * @param
 * 第一个和第二个参数为起始点坐标，第三个和第四个为终止点坐标，第五个为宽度
 * @retval None
 */
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
	}
	else {   /* The x-values are decreasing */
		xinc1 = -1;
		xinc2 = -1;
	}
	if (y2 >= y1) { /* The y-values are increasing */
		yinc1 = 1;
		yinc2 = 1;
	}
	else {   /* The y-values are decreasing */
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
	}
	else {                /* There is at least one y-value for every x-value */
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
