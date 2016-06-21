
#include <common.h>
#include <config.h>
#include <asm/arch-mx25/mx25_pins.h>
#include <asm/arch-mx25/imx-regs.h>
#include <asm/arch-mx25/iomux.h>
#include "lcd_hw.h"


#undef TFT_DEBUG
//#define TFT_DEBUG

#ifdef TFT_DEBUG
#define TFT_DBG(x...) printf(x)
#else
#define TFT_DBG(x...)
#endif


#define IMX_LCDC_REG(reg)	(IMX_LCDC_BASE + reg)


static unsigned int pcr  =  0xfae08b92;

static unsigned int HSPW =  2;
static unsigned int HFPD =  2;
static unsigned int HBPD =  6;
static unsigned int VSPW =  1;
static unsigned int VFPD =  2;
static unsigned int VBPD =  6;

unsigned int TFT_FRAMEBUFFER_BASE = TFT_FRAMEBUFFER_BG_BASE;

static inline unsigned int tft_reg_read(unsigned int addr)
{
	return *(volatile unsigned int*)addr;
}

static inline void tft_reg_write(unsigned int val,unsigned int addr)
{
	*(volatile unsigned int*)addr = val;
}

void tft_switch_fb_base_addr(unsigned int addr)
{
	TFT_FRAMEBUFFER_BASE = addr;

	tft_reg_write(TFT_FRAMEBUFFER_BASE, IMX_LCDC_REG(LCDC_LSSAR));
	TFT_DBG("LCDC_LSSAR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LSSAR)));
}

void tft_backlight_brightness(unsigned char level)
{
	/* Set LCDC PWM contract control register */
	tft_reg_write(0x00A90300 | level, IMX_LCDC_REG(LCDC_LPCCR));
}

void tft_backlight_open(void)
{
	unsigned int regVal = tft_reg_read(IMX_LCDC_REG(LCDC_LPCCR));
	
	tft_backlight_brightness(1);
	regVal |= (0x01<<8);
	tft_reg_write(regVal, IMX_LCDC_REG(LCDC_LPCCR));
}


void tft_backlight_close(void)
{
    unsigned int regVal = tft_reg_read(IMX_LCDC_REG(LCDC_LPCCR));

	tft_backlight_brightness(1);
	regVal &= (~(0x01<<8));
    tft_reg_write(regVal, IMX_LCDC_REG(LCDC_LPCCR));
}

void tft_gw_open(void)
{
	unsigned int regVal = tft_reg_read(IMX_LCDC_REG(LCDC_LGWCR));
	
	regVal |= 0xff400000;
	tft_reg_write(regVal, IMX_LCDC_REG(LCDC_LGWCR));
	
	TFT_DBG("LGWCR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LGWCR)));
}


void tft_gw_close(void)
{
    unsigned int regVal = tft_reg_read(IMX_LCDC_REG(LCDC_LGWCR));

	regVal &= ~0x400000;
    tft_reg_write(regVal, IMX_LCDC_REG(LCDC_LGWCR));
	
	TFT_DBG("LGWCR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LGWCR)));
}


void tft_gpio_init(void)
{
	mxc_request_iomux(MX25_PIN_LD1, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD2, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD3, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD4, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD5, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD6, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD7, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD8, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD9, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD10, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD11, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD13, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD14, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LD15, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_D14, MUX_CONFIG_ALT1);
	mxc_request_iomux(MX25_PIN_D15, MUX_CONFIG_ALT1);
	mxc_request_iomux(MX25_PIN_HSYNC, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_VSYNC, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_LSCLK, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_OE_ACD, MUX_CONFIG_FUNC);
	mxc_request_iomux(MX25_PIN_CONTRAST, MUX_CONFIG_FUNC);


#define LCD_PAD_CTL (PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_PUD | PAD_CTL_100K_PU)
	mxc_iomux_set_pad(MX25_PIN_LD1, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD2, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD3, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD4, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD5, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD6, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD7, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD8, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD9, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD10, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD11, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD13, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD14, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LD15, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_D14, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_D15, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_HSYNC, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_VSYNC, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_LSCLK, LCD_PAD_CTL | PAD_CTL_SRE_FAST);
	mxc_iomux_set_pad(MX25_PIN_OE_ACD, LCD_PAD_CTL);
	mxc_iomux_set_pad(MX25_PIN_CONTRAST, LCD_PAD_CTL);

}


void tft_clock_init(void)
{
	unsigned int ccm_cgr0,ccm_cgr1;	
	unsigned int ccm_mcr;
	unsigned int ccm_pcdr1;
	
	// step1:turn off all lcd clock.
	ccm_cgr1 = __REG(CCM_CGR1);
	ccm_cgr1 &=~(0x1<<29);	  //ipg clk
	__REG(CCM_CGR1)=ccm_cgr1;
	TFT_DBG("ccm_cgr1 = %08x\n",ccm_cgr1);
	
	ccm_cgr0 = __REG(CCM_CGR0);
	ccm_cgr0 &= ~(0x1<<24);	 //ahb clk
	
	ccm_cgr0 &=~(0x1<<7);	 // per clk
	__REG(CCM_CGR0) = ccm_cgr0;
	TFT_DBG("ccm_cgr0 = %08x\n",ccm_cgr0);

	// step2:setting lcd registers.
	tft_reg_write(TFT_FRAMEBUFFER_BASE, IMX_LCDC_REG(LCDC_LSSAR));
	TFT_DBG("LCDC_LSSAR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LSSAR)));

	tft_reg_write((((LCD_XSIZE_TFT >> 4) << 20) + LCD_YSIZE_TFT), IMX_LCDC_REG(LCDC_LSR));
	TFT_DBG("LCDC_LSR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LSR)));

	tft_reg_write((LCD_XSIZE_TFT>>1), IMX_LCDC_REG(LCDC_LVPWR));
	TFT_DBG("LCDC_LVPWR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LVPWR)));

	// step3:setting lcd resisters
	tft_reg_write(pcr, IMX_LCDC_REG(LCDC_LPCR));
	TFT_DBG("PCR = %08x\n",pcr);

	tft_reg_write(((HSPW - 1) << 26) + ((HFPD - 1) << 8) + (HBPD - 3), IMX_LCDC_REG(LCDC_LHCR));
	TFT_DBG("LHCR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LHCR)));
	
	tft_reg_write((VSPW << 26) + (VFPD << 8) + VBPD, IMX_LCDC_REG(LCDC_LVCR));
	TFT_DBG("LVCR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LVCR)));
    
	tft_reg_write(0x00120300, IMX_LCDC_REG(LCDC_LSCR));//Sharp configuration register
    tft_reg_write(0x00000000, IMX_LCDC_REG(LCDC_LRMCR));// Refresh mode control reigster
    tft_reg_write(0x00020010, IMX_LCDC_REG(LCDC_LDCR));// DMA control register

	// step4:setting lcd graphic window resisters.
	tft_reg_write(TFT_FRAMEBUFFER_GW_BASE, IMX_LCDC_REG(LCDC_LGWSAR));
	TFT_DBG("BASE LGWSAR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LGWSAR)));
    
	tft_reg_write(((LCD_GW_XSIZE_TFT >> 4) << 20) + LCD_GW_YSIZE_TFT, IMX_LCDC_REG(LCDC_LGWSR));//Graphic window size register
	TFT_DBG("LGWSR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LGWSR)));
    
	tft_reg_write(LCD_GW_XSIZE_TFT>>1, IMX_LCDC_REG(LCDC_LGWVPWR));//Graphic window virtual page width register
	TFT_DBG("LGWVPWR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LGWVPWR)));
    
	tft_reg_write(0,IMX_LCDC_REG(LCDC_LGWPR));//Graphic window position register
	TFT_DBG("LGWPR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LGWPR)));
    
	tft_reg_write(0, IMX_LCDC_REG(LCDC_LGWPOR));//Graphic window panning offset register
	TFT_DBG("LGWPOR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LGWPOR)));
    
	tft_reg_write(0x80040060, IMX_LCDC_REG(LCDC_LGWDCR));//Graphic window DMA control register
	TFT_DBG("LGWDCR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LGWDCR)));
    
	tft_reg_write(0, IMX_LCDC_REG(LCDC_LGWCR));
	TFT_DBG("LGWCR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LGWCR)));

	tft_reg_write(0,IMX_LCDC_REG(LCDC_LAUSCR));
	TFT_DBG("LAUSCR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LAUSCR)));
	
	tft_reg_write(0,IMX_LCDC_REG(LCDC_LAUSCCR));
	TFT_DBG("LAUSCCR = %08x\n",tft_reg_read(IMX_LCDC_REG(LCDC_LAUSCCR)));
	
	// step5:setting the source clock of perclk7 from upll.
	ccm_mcr = __REG(CCM_MCR)|(0x1<<7); 
	__REG(CCM_MCR)=ccm_mcr;
	TFT_DBG("ccm mcr = %08x\n",ccm_mcr);

	// step6:setting pixel clock.pixel clock = uclk/(PCDR1[5:0]+1)
	ccm_pcdr1 = *(volatile unsigned long*)(CCM_PCDR1);
	ccm_pcdr1 = (ccm_pcdr1 & ~(0x3f<<24)) | (1<<24);
	__REG(CCM_PCDR1) = ccm_pcdr1;
	TFT_DBG("ccm pcdr1 = %08x\n",ccm_pcdr1);

	// step7:open perclk7
	ccm_cgr0 = __REG(CCM_CGR0)|(1<<7);
	__REG(CCM_CGR0)=ccm_cgr0;
	TFT_DBG("ccm cgr0 plk= %08x\n",ccm_cgr0);

	// step8:open lcdc_ahbclk
	ccm_cgr0 = __REG(CCM_CGR0)|(1<<24);
	__REG(CCM_CGR0)=ccm_cgr0;
	TFT_DBG("ccm cgr0 ahb = %08x\n",ccm_cgr0);

	// step9:open lcdc_ipgclk
	ccm_cgr1 = __REG(CCM_CGR1)|(1<<29);
	__REG(CCM_CGR1)=ccm_cgr1;
	TFT_DBG("ccm ipg = %08x\n",ccm_cgr1);
}

