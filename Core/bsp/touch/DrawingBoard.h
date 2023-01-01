#ifndef _DrawingBoard_H
#define _DrawingBoard_H

#include "bsp_lcd.h"
#include "main.h"

#define COLOR_BLOCK_WIDTH 60                              /* ��ť��� */
#define COLOR_BLOCK_HEIGHT 30                             /* ��ť�߶� */
#define BUTTON_NUM 18                                     /* ��ť��Ŀ */
#define DRAWING_BOARD_START_X (COLOR_BLOCK_WIDTH * 1 + 1) /* �������Ͻ�X���� \
                                                           */
#define DRAWING_BOARD_START_Y 1 /* �������Ͻ�Y���� */
#define DRAWING_BOARD_END_X                                                    \
  (LCD_PIXEL_WIDTH - 1 - COLOR_BLOCK_WIDTH)        /* �������½�X���� */
#define DRAWING_BOARD_END_Y (LCD_PIXEL_HEIGHT - 1) /* �������½�Y���� */

/*��ˢ��״�б�*/
typedef enum {
  /*1���ؿ����*/
  LINE_SINGLE_PIXCEL = 0,
  /*2���ؿ����*/
  LINE_2_PIXCEL,
  /*4���ؿ����*/
  LINE_4_PIXCEL,
  /*6���ؿ����*/
  LINE_6_PIXCEL,
  /*8���ؿ����*/
  LINE_8_PIXCEL,
  /*16���ؿ����*/
  LINE_16_PIXCEL,
  /*��Բ����*/
  LINE_WITH_CIRCLE,
  /*����*/
  RECT,
  /*��Ƥ*/
  ERASER,
} SHAPE;

/*��ˢ����*/
typedef struct {
  /*��ɫ*/
  uint16_t color;
  /*��ˢ����*/
  SHAPE shape;
} DrawingBoard_Brush_Style;

typedef struct {
  uint16_t start_x; /*������x��ʼ����*/
  uint16_t start_y; /*������y��ʼ����*/
  uint16_t end_x;   /*������x��������*/
  uint16_t end_y;   /*������y��������*/
  uint32_t para; /*��ɫ��ť�б�ʾѡ�����ɫ���ʼ���״��ť�б�ʾѡ��Ļ�ˢ*/
  uint8_t touch_flag;          /*�������µı�־*/
  void (*draw_btn)(void *btn); /*������溯��*/
  void (*btn_command)(void *btn); /*��������ִ�к����������л���ɫ����ˢ*/
} DrawingBoard_Button;

/*
    LCD ��ɫ���룬CL_��Color�ļ�д
    16Bit�ɸ�λ����λ�� RRRR RGGG GGGB BBBB
*/
#if LCD_RGB_888
/*RGB888��ɫת��*/
#define RGB(R, G, B)                                                           \
  ((R << 16) | (G << 8) | (B)) /* ��8λR,G,Bת��Ϊ 24λRGB888��ʽ */

#else
/*RGB565 ��ɫת��*/
#define RGB(R, G, B)                                                           \
  (((R >> 3) << 11) | ((G >> 2) << 5) |                                        \
   (B >> 3)) /* ��8λR,G,Bת��Ϊ 16λRGB565��ʽ */
#define RGB565_R(x) ((x >> 8) & 0xF8)
#define RGB565_G(x) ((x >> 3) & 0xFC)
#define RGB565_B(x) ((x << 3) & 0xF8)

#endif

enum {
  CL_WHITE = RGB(255, 255, 255),       /* ��ɫ */
  CL_BLACK = RGB(0, 0, 0),             /* ��ɫ */
  CL_RED = RGB(255, 0, 0),             /* ��ɫ */
  CL_GREEN = RGB(0, 255, 0),           /* ��ɫ */
  CL_BLUE = RGB(0, 0, 255),            /* ��ɫ */
  CL_YELLOW = RGB(255, 255, 0),        /* ��ɫ */
  CL_GREY = RGB(98, 98, 98),           /* ���ɫ */
  CL_GREY1 = RGB(150, 150, 150),       /* ǳ��ɫ */
  CL_GREY2 = RGB(180, 180, 180),       /* ǳ��ɫ */
  CL_GREY3 = RGB(200, 200, 200),       /* ��ǳ��ɫ */
  CL_GREY4 = RGB(230, 230, 230),       /* ��ǳ��ɫ */
  CL_BUTTON_GREY = RGB(220, 220, 220), /* WINDOWS ��ť�����ɫ */
  CL_MAGENTA = RGB(255, 0, 255),       /* ����ɫ�����ɫ */
  CL_CYAN = RGB(0, 255, 255),          /* ����ɫ����ɫ */
  CL_BLUE1 = RGB(0, 0, 240),           /* ����ɫ */
  CL_BLUE2 = RGB(0, 0, 128),           /* ����ɫ */
  CL_BLUE3 = RGB(68, 68, 255),         /* ǳ��ɫ1 */
  CL_BLUE4 = RGB(0, 64, 128),          /* ǳ��ɫ1 */
  CL_BTN_FACE = RGB(236, 233, 216),    /* ��ť������ɫ(��) */
  CL_BOX_BORDER1 = RGB(172, 168, 153), /* �����������ɫ */
  CL_BOX_BORDER2 = RGB(255, 255, 255), /* �������Ӱ����ɫ */
  CL_MASK = 0x7FFF /* RGB565��ɫ���룬�������ֱ���͸�� */
};

/*DrawingBoard_brushȫ�ֱ����������ļ���ʹ�õ�*/
extern DrawingBoard_Brush_Style DrawingBoard_brush;

/*������Դ��ʼ��*/
void DrawingBoard_Init(void);
/*��ʼ����ť����*/
void DrawingBoard_Button_Init(void);
/*���Ͱ���*/
void DrawingBoard_Button_Down(uint16_t x, uint16_t y);
/*���Ͱ��µ�̧��*/
void DrawingBoard_Button_Up(uint16_t x, uint16_t y);
/*�������켣�����ڴ�������*/
void DrawingBoard_Draw_Graph(int16_t pre_x, int16_t pre_y, int16_t x, int16_t y,
                             DrawingBoard_Brush_Style *DrawingBoard_brush);

#endif //_DrawingBoard_H
