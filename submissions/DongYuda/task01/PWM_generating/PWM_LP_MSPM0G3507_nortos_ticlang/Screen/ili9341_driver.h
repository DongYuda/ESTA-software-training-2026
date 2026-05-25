#ifndef __LCD_H
#define __LCD_H		

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


/*--------------------Gpio configure--------------------*/
#define ILI9341_RS_PORT     GPIO_ILI9341_RS_PORT
#define ILI9341_RS_PIN      GPIO_ILI9341_RS_PIN
#define ILI9341_RST_PORT    GPIO_ILI9341_RST_PORT 
#define ILI9341_RST_PIN     GPIO_ILI9341_RST_PIN
#define ILI9341_SCK_PORT    GPIO_ILI9341_SCK_PORT
#define ILI9341_SCK_PIN     GPIO_ILI9341_SCK_PIN
#define ILI9341_SDI_PORT    GPIO_ILI9341_SDI_PORT
#define ILI9341_SDI_PIN     GPIO_ILI9341_SDI_PIN
#define ILI9341_CS_PORT     GPIO_ILI9341_CS_PORT
#define ILI9341_CS_PIN      GPIO_ILI9341_CS_PIN
//CS - GND,LED VCC 3.3
#define LCD_X_SIZE	        240
#define LCD_Y_SIZE	        320

#define USE_HORIZONTAL  		1	//定义是否使用横屏 		0,不使用.1,使用.
#define IS_STM							0


#ifdef USE_HORIZONTAL//如果定义了横屏 
#define X_MAX_PIXEL	        LCD_Y_SIZE
#define Y_MAX_PIXEL	        LCD_X_SIZE
#else
#define X_MAX_PIXEL	        LCD_X_SIZE
#define Y_MAX_PIXEL	        LCD_Y_SIZE
#endif

//移植一下
#if( IS_STM == 0)
#include "ti_msp_dl_config.h"

#define ILI9341_LED_H DL_GPIO_setPins(GPIO_ILI9341_LED_PORT, GPIO_ILI9341_LED_PIN)//背光

#define ILI9341_RS_H DL_GPIO_setPins(GPIO_ILI9341_RS_PORT, GPIO_ILI9341_RS_PIN)//指令
#define ILI9341_RS_L DL_GPIO_clearPins(GPIO_ILI9341_RS_PORT, GPIO_ILI9341_RS_PIN)

#define ILI9341_RST_H DL_GPIO_setPins(GPIO_ILI9341_RST_PORT, GPIO_ILI9341_RST_PIN)//复位
#define ILI9341_RST_L DL_GPIO_clearPins(GPIO_ILI9341_RST_PORT, GPIO_ILI9341_RST_PIN)

#define ILI9341_SDI_H DL_GPIO_setPins(GPIO_ILI9341_SDI_PORT, GPIO_ILI9341_SDI_PIN)//写入数据
#define ILI9341_SDI_L DL_GPIO_clearPins(GPIO_ILI9341_SDI_PORT, GPIO_ILI9341_SDI_PIN)

#define ILI9341_SCK_H DL_GPIO_setPins(GPIO_ILI9341_SCK_PORT, GPIO_ILI9341_SCK_PIN)//时钟
#define ILI9341_SCK_L DL_GPIO_clearPins(GPIO_ILI9341_SCK_PORT, GPIO_ILI9341_SCK_PIN)

#define ILI9341_CS_H DL_GPIO_setPins(GPIO_ILI9341_CS_PORT, GPIO_ILI9341_CS_PIN)//片选
#define ILI9341_CS_L DL_GPIO_clearPins(GPIO_ILI9341_CS_PORT, GPIO_ILI9341_CS_PIN)

#else
#include "main.h"

#define ILI9341_LED_H  HAL_GPIO_WritePin(ILI9341_LCD_LED_GPIO_Port, ILI9341_LCD_LED_Pin, GPIO_PIN_SET )//背光

#define ILI9341_RS_H   HAL_GPIO_WritePin(ILI9341_LCD_RS_GPIO_Port, ILI9341_LCD_RS_Pin, GPIO_PIN_SET )//指令(DC)
#define ILI9341_RS_L   HAL_GPIO_WritePin(ILI9341_LCD_RS_GPIO_Port, ILI9341_LCD_RS_Pin, GPIO_PIN_RESET )

#define ILI9341_RST_H  HAL_GPIO_WritePin(ILI9341_LCD_RST_GPIO_Port, ILI9341_LCD_RST_Pin, GPIO_PIN_SET )//复位
#define ILI9341_RST_L  HAL_GPIO_WritePin(ILI9341_LCD_RST_GPIO_Port, ILI9341_LCD_RST_Pin, GPIO_PIN_RESET )

#define ILI9341_SDI_H  HAL_GPIO_WritePin(ILI9341_LCD_SDI_GPIO_Port, ILI9341_LCD_SDI_Pin, GPIO_PIN_SET )//写入数据
#define ILI9341_SDI_L  HAL_GPIO_WritePin(ILI9341_LCD_SDI_GPIO_Port, ILI9341_LCD_SDI_Pin, GPIO_PIN_RESET )

#define ILI9341_SCK_H  HAL_GPIO_WritePin(ILI9341_LCD_SCK_GPIO_Port, ILI9341_LCD_SCK_Pin, GPIO_PIN_SET )//时钟
#define ILI9341_SCK_L  HAL_GPIO_WritePin(ILI9341_LCD_SCK_GPIO_Port, ILI9341_LCD_SCK_Pin, GPIO_PIN_RESET )

#define ILI9341_CS_H   HAL_GPIO_WritePin(ILI9341_LCD_CS_GPIO_Port, ILI9341_LCD_CS_Pin, GPIO_PIN_SET )//片选
#define ILI9341_CS_L   HAL_GPIO_WritePin(ILI9341_LCD_CS_GPIO_Port, ILI9341_LCD_CS_Pin, GPIO_PIN_RESET )

#endif

#define uint_8   unsigned char
#define uint_16  unsigned short
#define uint_32  unsigned int
#define int_8    signed char
#define int_16   signed short
#define int_32   signed int

extern  uint_16 BACK_COLOR, POINT_COLOR;   //背景色，画笔色

void ILI9341_init(uint16_t Color);
void ILI9341_clear(uint_16 Color);
void ILI9341_address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2);
void ILI9341_wr_data8(char da);
void ILI9341_wr_data(int da);
void ILI9341_wr_reg(char da);
void ILI9341_draw_point(uint16_t x,uint16_t y,uint16_t COLOR);//画点
void ILI9341_draw_point_big(uint_16 x,uint_16 y,uint16_t COLOR);//画一个大点
void ILI9341_draw_circle(uint16_t x0,uint16_t y0,uint8_t r,uint16_t COLOR);//画圈
void ILI9341_draw_line(uint_16 x1, uint_16 y1, uint_16 x2, uint_16 y2,uint16_t COLOR);//画线
void ILI9341_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t COLOR);	//画矩形	   
void ILI9341_fill(uint_16 xsta,uint_16 ysta,uint_16 xend,uint_16 yend,uint_16 color);//填充
void ILI9341_draw_char(uint16_t x,uint16_t y,char num,uint8_t mode,uint16_t COLOR);//显示一个字符
void ILI9341_draw_num(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint16_t COLOR);//只能显示16位数据 //从后往前显示 
void ILI9341_draw2_Num(uint_16 x,uint_16 y,uint_16 num,uint_8 len,uint16_t COLOR);// UINT 8   UNSIGNED CHAR
void ILI9341_draw_string(uint_16 x,uint_16 y,const char *p,uint16_t COLOR);		 //显示一个字符串,16字体
 
void ILI9341_draw_hanzi(unsigned int x,unsigned int y,unsigned char *displayStr,unsigned char index,uint16_t COLOR);
void ILI9341_draw_hanzi_string(uint16_t x,uint16_t y,unsigned char *displayStr,uint16_t sizeOfStr,uint16_t COLOR);
void ili9341_cursor_pos(char address);      //没啥用吧                                           

uint32_t mypow(uint8_t m,uint8_t n);
///以下几个函数是自己写的
void ili9341_draw_32num(uint_16 x,uint_16 y,uint32_t num,uint_8 len,uint16_t COLOR);//显示32位数据 不彳亍
void ILI9341_draw_real(uint16_t x,uint16_t y,double f_num,uint16_t bp_len,uint16_t ap_len,uint16_t COLOR);   //ap bp是描述小数点前后保留几位用的，更新可以显示负数lethe
float ili9341_Sc_Pr(uint16_t x,uint16_t y,uint8_t dis,uint16_t COLOR);//似乎用不了？？？Lethe 未使用
void ili9341_display_some(uint16_t SR,uint16_t ER);///局部区域显示
void ili9341_display_all(void);///退出局部区域显示
void ili9341_VSCRDEF(uint16_t TFA,uint16_t VSA,uint16_t BFA,uint16_t VSP);///垂直滚动
char* get_num2str(double input,char *output,int set); //代替sprintf 
/****************************Wunyje's part********************************///其中含sprintf函数，可能会使结构体等读取异常
void ILI9341_clear_w1(uint16_t Color);
void ILI9341_int_display(unsigned int x,unsigned int y,int a,uint16_t color);
void ILI9341_float_3_display(unsigned int x,unsigned int y,float a,uint16_t color);//保留几位小数
void ILI9341_float_5_display(unsigned int x,unsigned int y,float a,uint16_t color);
void ILI9341_float_7_display(unsigned int x,unsigned int y,float a,uint16_t color);
void ILI9341_float_4_display(unsigned int x,unsigned int y,float a,uint16_t color);

/*************************************************************************/

//画笔颜色

#define RGB888_To_RGB565(R,G,B)  (uint16_t)((R & 0x1f)<<11|(G & 0x3f)<<5|(B & 0x1f))  //自选RGB888转RGB565

#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	   0x001F  
#define BRED             0XF81F//粉紫
#define GRED 			       0XFFE0 //黄色
#define GBLUE			       0X07FF //浅浅蓝
#define RED           	 0xF800
//#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF//浅浅浅蓝
#define YELLOW        	 0xFFE0
#define BROWN 			     0XBC40 //棕色
#define BRRED 			     0XFC07 //棕红色
#define GRAY  			     0X8430 //灰色
#define ORANGE           0XFD20 //橙色
#define PURPLE           0X8010 
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//深浅浅蓝
#define GRAYBLUE       	 0X5458 //浅蓝
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅紫
#define LGRAY 			     0XC618 //近白(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //青色
#define LBBLUE           0X2B12 //半浅蓝(选择条目的反色)
			  		 
#endif  
