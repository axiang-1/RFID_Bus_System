#ifndef LCD_DRAW_BMP_H
#define LCD_DRAW_BMP_H


//打开lcd
int open_lcd();



//画进度条
void draw_bar(int x0, int y0, int width, int height, int speed, int color);


//清空屏幕
void clear_lcd();


/******************************************
*
*	函数功能：	在任意的坐标显示任意大小的图片
*
*	函数版本：  v1.0
*
*	编写作者：	xxx
*
*	编写时间：	2019.6.21
*
*	参数说明：	
*
*	特殊说明：	图片的起点+宽度或者高度不能超过 800  480
*
*********************************************/
int lcd_draw_bmp(int x0, int y0, char *bmpname);  //argv[0] -- 就是程序本身， argv[1]第一个参数


//关闭lcd屏幕
void close_lcd();


//显示数字
void show_num8x16(int x0,int y0, int num, int color);

//显示数字16*16
void show_num16x32(int x,int y, int num, int color);

//显示金额 2300   1  0  0  0     2   int = 2331/1000   
void show_money(int x, int y, int money);

//画制定颜色
void draw_color(int x0, int y0, int x_e, int y_e, int color);

#endif //LCD_DRAW_BMP_H


