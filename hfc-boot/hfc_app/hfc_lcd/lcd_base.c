#include <common.h>
#include <asm/arch/mx25.h>
#include <asm/arch-mx25/imx-regs.h>
#include <asm/arch-mx25/mx25_pins.h>
#include <asm/arch-mx25/iomux.h>
#include <asm/io.h>
#include <asm/spi.h>
#include <asm/gpio.h>
#include "lcd_hw.h"
#include "lcd_font24.c"

#define BAT_CLR_COLOR_TFT  0x0
#define BAT_FIL_COLOR_TFT  0xdd
#define BAT_IMG_COLOR_TFT  0x400

extern unsigned int TFT_FRAMEBUFFER_BASE;

static struct spi_device *tft_spi = NULL;

void init_yushun_ili9341(void);

static inline void __set_pixel(unsigned int x, unsigned int y, unsigned short color)
{
    unsigned short *fb;   

	fb = (unsigned short int *)TFT_FRAMEBUFFER_BASE;

    if ((y < LCD_XSIZE_TFT) && (x < LCD_YSIZE_TFT)) 
    {
		fb[((y)*LCD_XSIZE_TFT) + (x)] = color;
    }
}


void tft_backlight_on(void)
{
	tft_backlight_brightness(127);
}

void tft_backlight_off(void)
{
	tft_backlight_brightness(1);
}

void tft_fw_init(void)
{
	hfc_tft_spi_probe();
	tft_spi = hfc_get_tft_device();
	imx25_spi_setup(tft_spi,1000000,9,0);
	init_yushun_ili9341();
}

void tft_set_point(unsigned short color,int location)
{
	unsigned short int *pFB; //16bit color
    
	pFB = (unsigned short int *)TFT_FRAMEBUFFER_BASE;
	
	if(location < LCD_XSIZE_TFT*LCD_YSIZE_TFT)
		pFB[location] = color;
}

void tft_refresh_screan(unsigned short color)
{
	int i;
	unsigned short int *pFB; //16bit color
    
	pFB = (unsigned short int *)TFT_FRAMEBUFFER_BASE;
	
	for(i=0; i<LCD_XSIZE_TFT*LCD_YSIZE_TFT; i++)
		*pFB++ = color;
}

void tft_gw_refresh_screan(unsigned short color)
{
	int i;
	unsigned short int *pFB; //16bit color
    
	pFB = (unsigned short int *)TFT_FRAMEBUFFER_GW_BASE;
	
	for(i=0; i < LCD_GW_XSIZE_TFT*LCD_GW_YSIZE_TFT; i++)
		*pFB++ = color;
}

void lcd_reset_pin_init(void)
{
	mxc_request_iomux(MX25_PIN_UART1_CTS, MUX_CONFIG_ALT5);
	mxc_iomux_set_pad(MX25_PIN_UART1_CTS,PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_PUD | PAD_CTL_22K_PU);
	mxc_set_gpio_direction(MX25_PIN_UART1_CTS, 0);
	mxc_set_gpio_dataout(MX25_PIN_UART1_CTS,1);
}

void LCM_RES(unsigned int val)
{
    mxc_set_gpio_dataout(MX25_PIN_UART1_CTS,val);
}

void SPI_WR_DATA_ID0(unsigned char data1)
{
    unsigned short frame[2] = {0,0};
    frame[0] =( data1|0x01<<8);
    poll_spi_write(tft_spi, frame, 1);
}

void SPI_WR_COMMAND_ID0(unsigned char COMMAND)
{
    unsigned short frame[2] = {0,0};
    frame[0] = COMMAND;
    poll_spi_write(tft_spi, frame, 1);
}


void init_yushun_ili9341(void)
{
	lcd_reset_pin_init();
	
	LCM_RES(1);
	udelay(10000);
	LCM_RES(0);
	udelay(10000);
	LCM_RES(1);
	udelay(100000);
	
	SPI_WR_COMMAND_ID0(0xB9);
	SPI_WR_DATA_ID0 (0xFF);
	SPI_WR_DATA_ID0 (0x93);
	SPI_WR_DATA_ID0 (0x42);

	SPI_WR_COMMAND_ID0(0x20);

	SPI_WR_COMMAND_ID0(0x36);
	SPI_WR_DATA_ID0 (0x48);

	SPI_WR_COMMAND_ID0(0xC0);
	SPI_WR_DATA_ID0 (0x26);
	SPI_WR_DATA_ID0 (0x00);

	SPI_WR_COMMAND_ID0(0xC1);
	SPI_WR_DATA_ID0 (0x10);

	SPI_WR_COMMAND_ID0(0xC5);
	SPI_WR_DATA_ID0 (0x31);
	SPI_WR_DATA_ID0 (0x3C);

	SPI_WR_COMMAND_ID0(0xC7);
	SPI_WR_DATA_ID0 (0xC0);

	SPI_WR_COMMAND_ID0(0xB8);
	SPI_WR_DATA_ID0 (0x03);

	SPI_WR_COMMAND_ID0(0x2a);
	SPI_WR_DATA_ID0 (0x00);
	SPI_WR_DATA_ID0 (0x00);
	SPI_WR_DATA_ID0 (0x00);
	SPI_WR_DATA_ID0 (0xEF);


	SPI_WR_COMMAND_ID0(0x2b);
	SPI_WR_DATA_ID0 (0x00);
	SPI_WR_DATA_ID0 (0x00);
	SPI_WR_DATA_ID0 (0x01);
	SPI_WR_DATA_ID0 (0x3F);

	SPI_WR_COMMAND_ID0(0xE0);   // Set Gamma
	SPI_WR_DATA_ID0 (0x1F);
	SPI_WR_DATA_ID0 (0x24);
	SPI_WR_DATA_ID0 (0x23);
	SPI_WR_DATA_ID0 (0x0b);
	SPI_WR_DATA_ID0 (0x0f);
	SPI_WR_DATA_ID0 (0x08);
	SPI_WR_DATA_ID0 (0x50);
	SPI_WR_DATA_ID0 (0xd8);
	SPI_WR_DATA_ID0 (0x3b);
	SPI_WR_DATA_ID0 (0x08);
	SPI_WR_DATA_ID0 (0x0a);
	SPI_WR_DATA_ID0 (0x00);
	SPI_WR_DATA_ID0 (0x00);
	SPI_WR_DATA_ID0 (0x00);
	SPI_WR_DATA_ID0 (0x00);

	SPI_WR_COMMAND_ID0(0XE1);   // Set Gamma
	SPI_WR_DATA_ID0 (0x00); 	
	SPI_WR_DATA_ID0 (0x1b);
	SPI_WR_DATA_ID0 (0x1c);
	SPI_WR_DATA_ID0 (0x04);
	SPI_WR_DATA_ID0 (0x10);
	SPI_WR_DATA_ID0 (0x07);
	SPI_WR_DATA_ID0 (0x2f);
	SPI_WR_DATA_ID0 (0x27);
	SPI_WR_DATA_ID0 (0x44);
	SPI_WR_DATA_ID0 (0x07);
	SPI_WR_DATA_ID0 (0x15);
	SPI_WR_DATA_ID0 (0x0f);
	SPI_WR_DATA_ID0 (0x3f);
	SPI_WR_DATA_ID0 (0x3f);
	SPI_WR_DATA_ID0 (0x1F);

	SPI_WR_COMMAND_ID0(0x3A);   // SET 65K COLOR
	SPI_WR_DATA_ID0 (0x55);	    // note RGB signal define, 16bit use (D15~D0)

	SPI_WR_COMMAND_ID0(0xB0);   // SET EPL,DPL,VSL,HSL
	SPI_WR_DATA_ID0 (0xC2);

	SPI_WR_COMMAND_ID0(0xf6);   // SET Interface Control
	SPI_WR_DATA_ID0 (0x01);
	SPI_WR_DATA_ID0 (0x00);
	SPI_WR_DATA_ID0 (0x06);

	SPI_WR_COMMAND_ID0(0x11);   // Exit Sleep
	udelay(120000);
	SPI_WR_COMMAND_ID0(0x11);   // Exit Sleep
	udelay(120000);

	SPI_WR_COMMAND_ID0(0x29);   // Display ON
	SPI_WR_COMMAND_ID0(0x2c);
}

void test_lcd_reg(unsigned short val)
{
	SPI_WR_COMMAND_ID0(val & 0xff);
}


static inline void __disp_row_nr(unsigned int x, unsigned int y, unsigned short c, unsigned char coldat, int nr)
{
    int i;
    for(i = 0; i < nr; i++)
    {
        if(coldat & (0x80 >> i))
            __set_pixel(x+i, y , c);
    }
}

void __drawchars_24x12(unsigned int x, unsigned int y, unsigned short color, char *str, unsigned int count)
{
    int i, m;
    int n;
    unsigned char bits;

    for(m = 0; m < count; m++, x += 12)
    {
        n = (*str++ - 0x20) * 48;
        for(i = 0; i < 24; i++)
        {
			bits = ASCII24[n + i * 2];
			__disp_row_nr(x  , y + i , color, bits,8);
			bits = ASCII24[n + i * 2 + 1];
			__disp_row_nr(x + 8,y + i ,color, bits,4);
        }
	}
}

void lcd_put_strings(unsigned int x, unsigned int y, unsigned short color, char *str)
{
    __drawchars_24x12(x, y, color, str, (unsigned int )strlen(str));
}

void fill_frame_buffer(int x0, int y0, int x1, int y1, int color)
{
    int i, j;

    for(j = y0; j < y1; j++)
	{
        for(i = x0; i < x1; i++)
		{
            __set_pixel(i, j, color);
		}
	}
}

void lcd_display_voltage(int volt)
{
    char buf[16];
	sprintf(buf, "Voltage:%d.%01dV", volt / 100, (volt % 100) / 10);
	
	fill_frame_buffer(142, 100, 185, 130, 0); //clear display

	lcd_put_strings(52,100,0xff00,buf);
}

/*
 * test battery img using the command
 * lcd fill 0x0400 69 133 175 139
 * lcd fill 0x0400 69 181 175 187
 * lcd fill 0x0400 65 145 69 175 
 * lcd fill 0x0400 69 139 75 181
 * lcd fill 0x0400 169 139 175 151
 * lcd fill 0x0400 169 169 175 181
 */
void tft_fill_battery_img(void)
{
	fill_frame_buffer(69, 133, 175, 139, BAT_IMG_COLOR_TFT); //up
	fill_frame_buffer(69, 181, 175, 187, BAT_IMG_COLOR_TFT); //down
	fill_frame_buffer(65, 145, 69, 175, BAT_IMG_COLOR_TFT); //left1
	fill_frame_buffer(69, 139, 75, 181, BAT_IMG_COLOR_TFT); //left2
	fill_frame_buffer(169, 139, 175, 151, BAT_IMG_COLOR_TFT); //right1
	fill_frame_buffer(169, 169, 175, 181, BAT_IMG_COLOR_TFT); //right2
}

/*
* icon 1: means no battery
* icon 2: means 1 level
* icon 3: means 2 level
* icon 4: means 3 level
* icon 5: means is full
* lcd fill 0xdd 77 141 106 179   
* lcd fill 0xdd 107 141 136 179  
* lcd fill 0xdd 137 141 167 179
*/
void tft_display_battery_icon(unsigned char icon)
{
	switch(icon)
	{
		case 4:
		case 3:
			fill_frame_buffer(77, 141, 106, 179, BAT_FIL_COLOR_TFT);
		case 2:
			fill_frame_buffer(107, 141, 136, 179, BAT_FIL_COLOR_TFT);
		case 1:
			fill_frame_buffer(137, 141, 167, 179, BAT_FIL_COLOR_TFT);
			break;
		default:
			fill_frame_buffer(77, 141, 167, 179, BAT_CLR_COLOR_TFT); //clear all
			break;  
	}
}



