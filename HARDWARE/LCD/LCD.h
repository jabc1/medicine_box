#ifndef __HARDWARE_LCD_H__
#define __HARDWARE_LCD_H__
#include "stm32f10x.h"

//屏幕开始时显示方式，注意：当IDelay时显示第一幅画面是逐像素刷新的
//此时必须手动在刷新结束后加上  lcd_wr_reg(0x0007,0x0173);才能显示
//当Immediately时没有被注释掉是不需要此过程

/* 选择BANK1-BORSRAM1 连接 TFT，地址范围为0X60000000~0X63FFFFFF
 * FSMC_A16 接LCD的DC(寄存器/数据选择)脚
 * 16 bit => FSMC[24:0]对应HADDR[25:1]
 * 寄存器基地址 = 0X60000000
 * RAM基地址 = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
 * 当选择不同的地址线时，地址要重新计算。
 */
#define Bank1_LCD_D    ((u32)0x60020000)    //Disp Data ADDR
#define Bank1_LCD_C    ((u32)0x60000000)	   //Disp Reg ADDR

#define Set_Rst GPIOD->BSRR = GPIO_Pin_13;
#define Clr_Rst GPIOD->BRR  = GPIO_Pin_13;

#define Lcd_Light_ON   GPIOA->BSRR = GPIO_Pin_1;
#define Lcd_Light_OFF  GPIOA->BRR  = GPIO_Pin_1;


//typedef enum
//{
//    DIRECTION_VERTICAL = 1,
//    DIRECTION_HORIZONTAL,
//}dis_direction_enum;

#define DIRECTION_VERTICAL          1
#define DIRECTION_HORIZONTAL        2
#define DIS_DIRECTION   DIRECTION_HORIZONTAL

#if DIS_DIRECTION == DIRECTION_VERTICAL

#define LCD_Y_MAX       480
#define LCD_X_MAX       320

#elif DIS_DIRECTION == DIRECTION_HORIZONTAL

#define LCD_Y_MAX       320
#define LCD_X_MAX       480

#endif

//Lcd初始化及其低级控制函数
void Lcd_Configuration(void);
void DataToWrite(u16 data);
void lcd_init(void);
void lcd_wr_reg(u16 Index,u16 CongfigTemp);
void Lcd_WR_Start(void);
//Lcd高级控制函数
void lcd_color_box(u16 x,u16 y,u16 xLong,u16 yLong,u16 Color);
void lcd_draw_pixel(u16 x, u16 y, u16 Color);
u16 lcd_read_pixel(u16 x,u8 y);
void lcd_fill_pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic);
void lcd_block_write(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);
char Tiky_Button(char *filename,u16 x,u16 y);

/*定义常见颜色*/
// #define red 0x001f
// #define blue 0xf800
// #define green 0x07e0
// #define black 0x0000
// #define white 0xffff
// #define yellow 0x07ff
// #define orange 0x05bf
// #define Chocolate4 0x4451
// #define Grey 0xefbd//灰色

#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#endif
