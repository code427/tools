/*
 * (C) Copyright 2003
 * Gerry Hamel, geh@ti.com, Texas Instruments
 *
 * (C) Copyright 2006
 * Bryan O'Donoghue, bodonoghue@codehermit.ie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307	 USA
 *
 */

#include <common.h>
#include <config.h>
#include <stdio_dev.h>
#include <asm/arch-mx25/mx25_pins.h>
#include <asm/arch-mx25/iomux.h>
#include "lcd_hw.h"
#include "lcd_base.h"


#define LCD_DEBUG 1

#ifdef LCD_DEBUG
#define DBG_LCD(fmt,args...)\
	serial_printf("[%s] %s %d: "fmt, __FILE__,__FUNCTION__,__LINE__,##args)
#else
#define DBG_LCD(fmt,args...) do{}while(0)
#endif

int lcd_init = 0;

int do_lcd_test( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned char contrast = 0;
	unsigned short color = 0;
	unsigned short temp = 0;
	
	if(strncmp(argv[1],"init",4)==0)
	{
		tft_gpio_init();
		tft_clock_init();
		tft_fw_init();
		lcd_init = 1;
	}
	
	if(!lcd_init)
	{
		DBG_LCD("Lcd no init\n");
		return 1;
	}

	if(strncmp(argv[1],"ledon",5)==0)
	{
		tft_backlight_open();
		printf("TFT backlight on\n");
	}
	else if(strncmp(argv[1],"ledoff",6)==0)
	{
		tft_backlight_close();
		printf("TFT backlight off\n");
	}
	else if(strncmp(argv[1],"contrast",8)==0)
	{
		if(argc < 3)
		{
			return 2;
		}
		contrast = (unsigned char)simple_strtoul(argv[2], NULL, 10);

		tft_backlight_brightness(contrast);
	}
	else if(strncmp(argv[1],"gwon",4)==0)
	{
		tft_gw_open();
		printf("TFT gw on\n");
	}
	else if(strncmp(argv[1],"gwoff",5)==0)
	{
		tft_gw_close();
		printf("TFT gw off\n");
	}
	else if(strncmp(argv[1],"refresh",7)==0)
	{
		if(argc < 3)
		{
			return 2;
		}
		color = (unsigned short)simple_strtoul(argv[2], NULL, 16);
		printf("bg color is %x\n",color);
		
		tft_refresh_screan(color);
	}
	else if(strncmp(argv[1],"gwfresh",7)==0)
	{
		if(argc < 3)
		{
			return 2;
		}
		color = (unsigned short)simple_strtoul(argv[2], NULL, 16);
		printf("gw color is %x\n",color);
		
		tft_gw_refresh_screan(color);
	}
	else if(strncmp(argv[1],"reg",3)==0)
	{
		if(argc < 3)
		{
			return 2;
		}
		temp = (unsigned short)simple_strtoul(argv[2], NULL, 10);
		
		test_lcd_reg(temp);
	}
	else if(strncmp(argv[1],"battery",7)==0)
	{
extern void tft_fill_battery_img(void);
extern void tft_display_battery_icon(unsigned char icon);
	
		tft_fill_battery_img();
		tft_display_battery_icon(5);
	}

	else if(strncmp(argv[1],"vol",3)==0)
	{
extern void lcd_display_voltage(int volt);
extern volatile unsigned int bat_voltage;
		
		bat_voltage = (int)simple_strtoul(argv[2], NULL, 10);
		
		lcd_display_voltage(bat_voltage);

	}
	return 0;
}


U_BOOT_CMD(
	lcd, 7,	1,do_lcd_test,
	"test lcd",
	"lcdtest list|timeron|timeroff [irq_dec]"
);


