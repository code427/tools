#ifndef __LCD_BASE_H__
#define __LCD_BASE_H__


extern void tft_fw_init(void);
extern void tft_refresh_screan(unsigned short color);
extern void tft_gw_refresh_screan(unsigned short color);
extern void tft_refresh_screan_again(unsigned short color);
extern void tft_set_point(unsigned short color,int location);
extern void test_lcd_reg(unsigned short val);

#endif

