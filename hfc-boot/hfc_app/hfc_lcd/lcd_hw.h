/*
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

#ifndef __LCD_HW_H__
#define __LCD_HW_H__


#define LCDC_LSSAR		0x00
#define LCDC_LSR		0x04
#define LCDC_LVPWR		0x08
#define LCDC_LCPR		0x0C
#define LCDC_LCWHBR		0x10
#define LCDC_LCCMR		0x14
#define LCDC_LPCR		0x18
#define LCDC_LHCR		0x1C
#define LCDC_LVCR		0x20
#define LCDC_LPOR		0x24
#define LCDC_LSCR		0x28
#define LCDC_LPCCR		0x2C
#define LCDC_LDCR		0x30
#define LCDC_LRMCR		0x34
#define LCDC_LICR		0x38
#define LCDC_LIER		0x3C
#define LCDC_LISR		0x40
#define LCDC_LGWSAR		0x50
#define LCDC_LGWSR		0x54
#define LCDC_LGWVPWR	0x58
#define LCDC_LGWPOR		0x5C
#define LCDC_LGWPR		0x60
#define LCDC_LGWCR		0x64
#define LCDC_LGWDCR		0x68
#define LCDC_LAUSCR		0x80
#define LCDC_LAUSCCR	0x84

#define LCD_XSIZE_TFT   240
#define LCD_YSIZE_TFT   320 

#define LCD_GW_XSIZE_TFT   240
#define LCD_GW_YSIZE_TFT   200

extern void tft_backlight_brightness(unsigned char level);
extern void tft_backlight_open(void);
extern void tft_backlight_close(void);
extern void tft_gpio_init(void);
extern void tft_clock_init(void);
extern void tft_clock_init_again(void);
extern void tft_gw_open(void);
extern void tft_gw_close(void);

#endif
