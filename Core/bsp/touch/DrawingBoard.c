#include "DrawingBoard.h"
#include "bsp_lcd.h"

/*��ť�ṹ������*/
static DrawingBoard_Button button[BUTTON_NUM];
/*���ʲ���*/
static DrawingBoard_Brush_Style DrawingBoard_brush;

/*��ť��Դע�ắ��*/
static void DrawingBoard_Button_Init(void);
/*��ɫ��ť����溯��*/
static void Draw_Button_Color(void *btn);
/*������ť���ƺ���*/
static void Draw_Button_Clear(void *btn);
/*���ʰ�ť���ƺ���*/
static void Draw_Button_Shape(void *btn);
/*��Ƥ��ť����溯��*/
static void Draw_Button_Eraser(void *btn);
/*���ΰ�ť����溯��*/
static void Draw_Button_Rect(void *btn);
/*ѡ����ɫ*/
static void Select_Shape_Color(void *btn);
/*ѡ���ˢ����*/
static void Select_Brush_Class(void *btn);
/*����*/
static void Clear_DrawingBoard_Area(void *btn);
/*���һ�����ؿ�ȵĹ켣��*/
static void LCD_DrawUniLineCircle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t thick);
/*�������켣�����ڴ�������*/
static void
DrawingBoard_Draw_Graph(int16_t pre_x, int16_t pre_y, int16_t x, int16_t y,
	DrawingBoard_Brush_Style *DrawingBoard_brush);
/*���ڴ��������ʼ��ľ�̬����*/
static int32_t Rect_x = -1, Rect_y = -1;

/**
 * @brief  �����ʼ��
 * @retval None
 */
void DrawingBoard_Init(void) {
	uint8_t i;
	/* ������Ϊ��ɫ */
	LCD_Clear(COLOR_WHITE); /* ��������ʾȫ�� */
	/* ��ʼ����ť */
	DrawingBoard_Button_Init();
	/* ��水ť */
	for (i = 0; i < BUTTON_NUM; i++)
		button[i].draw_btn(&button[i]);
	/* ��ʼ������ */
	DrawingBoard_brush.color = COLOR_BLACK;
	DrawingBoard_brush.brush_shape = LINE_SINGLE_PIXCEL;
}

/**
 * @brief  ����������ʱ���õĺ���
 * @param  ��һ���͵ڶ�������Ϊ��һ���㣬�������͵��ĸ�����Ϊ��ǰ�ĵ�
 * @retval None
 */
void DrawingBoard_Button_Down(int16_t pre_x, int16_t pre_y, int16_t x,
	int16_t y) {
	uint8_t i;
	for (i = 0; i < BUTTON_NUM; i++) {
		/* �������˰�ť */
		if (x <= button[i].end_x && y <= button[i].end_y &&
			y >= button[i].start_y && x >= button[i].start_x) {
			if (button[i].touch_flag == 0) { /*ԭ����״̬Ϊû�а��£������״̬*/
				button[i].touch_flag = 1;       /* ��¼���±�־ */
				button[i].draw_btn(&button[i]); /*�ػ水ť*/
			}
		}
		else if (button[i].touch_flag ==
			1) { /* �����Ƴ��˰����ķ�Χ��֮ǰ�а��°�ť */
			button[i].touch_flag = 0; /* ������±�־���ж�Ϊ�����*/
			button[i].draw_btn(&button[i]); /*�ػ水ť*/
		}
	}
	/*����ͼ�λ���*/
	DrawingBoard_Draw_Graph(pre_x, pre_y, x, y, &DrawingBoard_brush);
}

/**
 * @brief  �������ͷ�ʱ���õĺ���
 * @param  ��һ���͵ڶ���������ǰ�ĵ�
 * @retval None
 */
void DrawingBoard_Button_Up(uint16_t x, uint16_t y) {
	uint8_t i;
	for (i = 0; i < BUTTON_NUM; i++) {
		/* �����ڰ�ť�����ͷ� */
		if ((x < button[i].end_x && x > button[i].start_x && y < button[i].end_y &&
				y > button[i].start_y)) {
			button[i].touch_flag = 0;          /*�ͷŴ�����־*/
			button[i].draw_btn(&button[i]);    /*�ػ水ť*/
			button[i].btn_command(&button[i]); /*ִ�а����Ĺ�������*/
			break;
		}
	}
	if (x > DRAWING_BOARD_START_X && x < DRAWING_BOARD_END_X) {
		/* �����ڻ��������ͷ� */
		switch (DrawingBoard_brush.brush_shape) { /*���ݻ�ˢ������治ͬ�Ĺ켣*/
		/* ������ */
		case RECT:
			if (x - 10 < DRAWING_BOARD_START_X ||
				x + 10 > DRAWING_BOARD_END_X) // �����߽緶Χ
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
 * @brief  �ڻ����������ͼ��
 * @param
 * ��һ���͵ڶ�������Ϊ��һ���㣬�������͵��ĸ�����Ϊ��ǰ�ĵ㣬���������Ϊ��ˢ�ṹ��ָ��
 * @retval None
 */
static void
DrawingBoard_Draw_Graph(int16_t pre_x, int16_t pre_y, int16_t x, int16_t y,
	DrawingBoard_Brush_Style *DrawingBoard_brush) {
	/*����λ���ڻ�������*/
	if (x > DRAWING_BOARD_START_X && pre_x > DRAWING_BOARD_START_X
		&& x < DRAWING_BOARD_END_X
		&& pre_x < DRAWING_BOARD_END_X) {
		switch (DrawingBoard_brush->brush_shape) { /*���ݻ�ˢ������治ͬ�Ĺ켣*/
		/* ���1���ؿ�ȵ��� */
		case LINE_SINGLE_PIXCEL:
			if (pre_x < 0 || pre_y < 0) // �µıʼ�
				PutPixel(x, y);
			else // ������һ�εıʼ�
				LCD_DrawUniLine(pre_x, pre_y, x, y);
			break;
		case LINE_2_PIXCEL:
			if (x - 1 < DRAWING_BOARD_START_X
				|| pre_x - 1 < DRAWING_BOARD_START_X) // �����߽緶Χ
				break;
			LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 1);
			break;
		case LINE_4_PIXCEL:
			if (x - 2 < DRAWING_BOARD_START_X
				|| pre_x - 2 < DRAWING_BOARD_START_X) // �����߽緶Χ
				break;
			LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 2);
			break;
		case LINE_6_PIXCEL:
			if (x - 3 < DRAWING_BOARD_START_X
				|| pre_x - 3 < DRAWING_BOARD_START_X) // �����߽緶Χ
				break;
			LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 3);
			break;
		case LINE_8_PIXCEL:
			if (x - 4 < DRAWING_BOARD_START_X
				|| pre_x - 4 < DRAWING_BOARD_START_X) // �����߽緶Χ
				break;
			LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 4);
			break;
		case LINE_16_PIXCEL:
			if (x - 8 < DRAWING_BOARD_START_X
				|| pre_x - 8 < DRAWING_BOARD_START_X) // �����߽緶Χ
				break;
			LCD_DrawUniLineCircle(pre_x, pre_y, x, y, 8);
			break;
		/*���ƾ���*/
		case RECT:
			if (x - 10 < DRAWING_BOARD_START_X
				|| pre_x - 10 < DRAWING_BOARD_START_X) // �����߽緶Χ
				break;
			if (Rect_x == -1) {
				if (!(x > DRAWING_BOARD_END_X - 10 || y > DRAWING_BOARD_END_Y - 10
						|| x - 10 < DRAWING_BOARD_START_X
						|| y - 10 < DRAWING_BOARD_START_Y)) {
					/*��¼�������*/
					Rect_x = x;
					Rect_y = y;
				}
			}
			break;
		/*���ƴ�Բ����*/
		case LINE_WITH_CIRCLE:
			if (x - 3 < DRAWING_BOARD_START_X || pre_x - 3 < DRAWING_BOARD_START_X
				|| y - 3 < DRAWING_BOARD_START_Y
				|| pre_y - 3 < DRAWING_BOARD_START_Y || y + 3 > DRAWING_BOARD_END_Y
				|| pre_y + 3 > DRAWING_BOARD_END_Y)
				break;
			if (pre_x < 0 || pre_y < 0) // �µıʼ�
				PutPixel(x, y);
			else { // ������һ�εıʼ�
				LCD_DrawUniLine(pre_x, pre_y, x, y);
				LCD_DrawFullCircle(x, y, 3);
			}
			break;
		/*��Ƥ����*/
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
 * @brief  ��ɫ��ť����溯��
 * @param  ��ť���ݽṹ��ָ��
 * @retval None
 */
static void Draw_Button_Color(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	uint16_t colors1, colors2;
	/*����֮ǰ��ɫ�ʲ���*/
	LCD_GetColors(&colors1, &colors2);
	/*�ͷŰ���*/
	if (ptr->touch_flag == 0) {
		/*����Ϊ���ܼ���Ӧ����ɫ*/
		LCD_SetColors(ptr->para, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
	}
	else {   /*��������*/
		/*��ɫ����*/
		LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
	}
	/*��ť�߿�*/
	LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
	LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
		ptr->end_y - ptr->start_y);
	/*��ԭԭ�ȵ�ɫ�ʲ���*/
	LCD_SetColors(colors1, colors2);
}

/**
 * @brief  ������ť����溯��
 * @param  ��ť���ݽṹ��ָ��
 * @retval None
 */
static void Draw_Button_Clear(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	uint16_t colors1, colors2;
	/*����֮ǰ��ɫ�ʲ���*/
	LCD_GetColors(&colors1, &colors2);
	/*�ͷŰ���*/
	if (ptr->touch_flag == 0) {
		LCD_SetColors(COLOR_BUTTON_GREY, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"CLEAR");
	}
	else {   /*��������*/
		LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"CLEAR");
	}
	/*��ť�߿�*/
	LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
	LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
		ptr->end_y - ptr->start_y);
	/*��ԭԭ�ȵ�ɫ�ʲ���*/
	LCD_SetColors(colors1, colors2);
}

/**
 * @brief  ��Ƥ����ť����溯��
 * @param  ��ť���ݽṹ��ָ��
 * @retval None
 */
static void Draw_Button_Eraser(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	uint16_t colors1, colors2;
	/*����֮ǰ��ɫ�ʲ���*/
	LCD_GetColors(&colors1, &colors2);
	/*�ͷŰ���*/
	if (ptr->touch_flag == 0) {
		LCD_SetColors(COLOR_BUTTON_GREY, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"ERASER");
	}
	else {   /*��������*/
		LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"ERASER");
	}
	/*��ť�߿�*/
	LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
	LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
		ptr->end_y - ptr->start_y);
	/*��ԭԭ�ȵ�ɫ�ʲ���*/
	LCD_SetColors(colors1, colors2);
}

/**
 * @brief  ���ΰ�ť����溯��
 * @param  ��ť���ݽṹ��ָ��
 * @retval None
 */
static void Draw_Button_Rect(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	uint16_t colors1, colors2;
	/*����֮ǰ��ɫ�ʲ���*/
	LCD_GetColors(&colors1, &colors2);
	/*�ͷŰ���*/
	if (ptr->touch_flag == 0) {
		LCD_SetColors(COLOR_BUTTON_GREY, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"RECT");
	}
	else {   /*��������*/
		LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->start_y + 11,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"RECT");
	}
	/*��ť�߿�*/
	LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
	LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
		ptr->end_y - ptr->start_y);
	/*��ԭԭ�ȵ�ɫ�ʲ���*/
	LCD_SetColors(colors1, colors2);
}

/**
 * @brief  ��ˢ��ť����溯��
 * @param  ��ť���ݽṹ��ָ��
 * @retval None
 */
static void Draw_Button_Shape(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	uint16_t i;
	uint16_t colors1, colors2;
	/*����֮ǰ��ɫ�ʲ���*/
	LCD_GetColors(&colors1, &colors2);
	/* ������ɫ û����ʱΪ��ɫ������ʱΪ��ɫ*/
	if (ptr->touch_flag == 0) {
		LCD_SetColors(COLOR_BUTTON_GREY, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		/*���Ʊ߿����*/
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
	}
	else {
		LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
		LCD_DrawFullRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
		/*���Ʊ߿����*/
		LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
		LCD_DrawRect(ptr->start_x, ptr->start_y, ptr->end_x - ptr->start_x,
			ptr->end_y - ptr->start_y);
	}
	LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
	/*���ݻ�ˢ��״��水ťͼ��*/
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
	case RECT: /*���ƾ��ΰ�ť*/
		LCD_SetColors(COLOR_WHITE, COLOR_BLACK);
		LCD_DrawFullRect(ptr->start_x + ((ptr->end_x - ptr->start_x - 40) / 2),
			ptr->start_y + ((ptr->end_y - ptr->start_y - 40 - 30) / 2), 40, 40);
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->end_y - 19,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"Rect");
		break;
		break;
	case LINE_WITH_CIRCLE: /*���ƴ�Բ���߰�ť*/
		LCD_SetColors(COLOR_BLACK, COLOR_WHITE);
		LCD_DrawUniLine(
			ptr->start_x + 5, ptr->start_y + (ptr->end_y - ptr->start_y) / 2,
			ptr->end_x - 5,
			ptr->start_y + (ptr->end_y - ptr->start_y) / 2);
		for (i = 0; i < ((ptr->end_x - ptr->start_x - 10) / 10); i++)
			LCD_DrawFullCircle(ptr->start_x + 10 + i * 10,
				ptr->start_y + (ptr->end_y - ptr->start_y) / 2, 3);
		break;
	case ERASER: /*������Ƥ�߰�ť*/
		LCD_SetColors(COLOR_WHITE, COLOR_BLACK);
		LCD_DrawFullRect(ptr->start_x + ((ptr->end_x - ptr->start_x - 40) / 2),
			ptr->start_y + ((ptr->end_y - ptr->start_y - 40 - 30) / 2), 40, 40);
		LCD_SetColors(COLOR_BLACK, COLOR_BUTTON_GREY);
		LCD_SetFont(&Font8x8);
		LCD_DispString_EN(ptr->end_y - 19,
			ptr->start_x + (ptr->end_x - ptr->start_x - 24 * 2) / 2, (uint8_t *)"ERASER");
		break;
	}
	/*��ԭԭ�ȵ�ɫ�ʲ���*/
	LCD_SetColors(colors1, colors2);
}

/**
 * @brief  �л���ˢ��ɫ
 * @param  ��ť���ݽṹ��ָ��
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
 * @brief  �л���ˢ����
 * @param  ��ť���ݽṹ��ָ��
 * @retval None
 */
static void Select_Brush_Class(void *btn) {
	DrawingBoard_Button *ptr = (DrawingBoard_Button *)btn;
	DrawingBoard_brush.brush_shape = (BRUSH_SHAPE)ptr->para;
}

/**
 * @brief  �л���ˢ��ɫ
 * @param  ��ť���ݽṹ��ָ��
 * @retval None
 */
static void Clear_DrawingBoard_Area(void *btn) {
	LCD_SetColors(COLOR_WHITE, COLOR_WHITE);
	LCD_DrawFullRect(DRAWING_BOARD_START_X, DRAWING_BOARD_START_Y,
		DRAWING_BOARD_END_X - (DRAWING_BOARD_START_X) + 1,
		DRAWING_BOARD_END_Y - DRAWING_BOARD_START_Y + 1);
}

/**
 * @brief  ��ʼ����ť����
 * @param  None
 * @retval None
 */
static void DrawingBoard_Button_Init(void) {
	/*��ʼ����ť����,����̫���������ú궨�����*/
	DrawingBoard_BUTTON_INIT_CODE;
}

/**
 * @brief  ������֮�������
 * @param
 * ��һ���͵ڶ�������Ϊ��ʼ�����꣬�������͵��ĸ�Ϊ��ֹ�����꣬�����Ϊ���
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
		// �жϱ߽�
		if (x + thick > DRAWING_BOARD_END_X || x - thick < 0 || // Һ�����ұ߽�
			y + thick > DRAWING_BOARD_END_Y || y - thick < 0) // Һ�����±߽�
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
