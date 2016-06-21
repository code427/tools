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
#include <circbuf.h>
#include <stdio_dev.h>
#include <irq.h>

//#define DEBUG 1

#ifdef DEBUG
#define DBG(fmt,args...)\
	serial_printf("[%s] %s %d: "fmt, __FILE__,__FUNCTION__,__LINE__,##args)
#else
#define DBG(fmt,args...) do{}while(0)
#endif

extern void green_on(void);
extern void green_off(void);
extern void red_on(void);
extern void red_off(void);

volatile unsigned int timer_base = 0;
unsigned int timer_flag = 0;
unsigned int timer_flag2 = 0;

volatile unsigned int time_tick = 0;

static void gpt2_irq_handler(unsigned int irq)
{
	/* close GPT2 Interrupt */
	__REG(IMX_GPT2_BASE+MX25_GPTCR) = (__REG(IMX_GPT2_BASE+MX25_GPTCR)&(~0x01));

	/* clear all interrupt flag on GPT2 */
	__REG(IMX_GPT2_BASE+MX25_GPTSR) = 0x3F;	

	timer_base++;
	if(timer_base > 10)
	{
		timer_base = 0;
		time_tick = 1;
	}

	/* enable GPT2 Interrupt */
	__REG(IMX_GPT2_BASE+MX25_GPTCR) = (__REG(IMX_GPT2_BASE+MX25_GPTCR)|0x01);
}

int gpt2_init(void)
{
	int ret = 0;

	/* divide value (AHB 90M now, 90M/3600 = 25K Hz) */
	__REG(IMX_GPT2_BASE+MX25_GPTPR) = (3600-1);
	
	/* chose the ipg_clk_highfreq clock source */
	__REG(IMX_GPT2_BASE+MX25_GPTCR) = 2<<6; 
	
	/* Output compare 1 interrupt enable */
	__REG(IMX_GPT2_BASE+MX25_GPTIR) = 1<<0; 

	/* chanel 1 compare value generate interrupt counter (1ms)*/
	__REG(IMX_GPT2_BASE+MX25_GPTOCR1) = 25; 
	
	/* Enable MXC_INT_GPT2 Interrupt */
	ret = irq_request(MXC_INT_GPT2, gpt2_irq_handler, "gpt2");
	if(ret !=0) 
	{
		DBG("GPT2 irq request failed \n");	
	}
	
	/* GPT Enable */
	__REG(IMX_GPT2_BASE+MX25_GPTCR) = (__REG(IMX_GPT2_BASE + MX25_GPTCR)|0x01);
	
	return ret;
}

static void gpt2_finished(void)
{
	/* disable GPT */
 	__REG(IMX_GPT2_BASE+MX25_GPTPR) = (__REG(IMX_GPT2_BASE+MX25_GPTPR)&(~0x01));
	
	udelay(10);
	
	/* free MXC_INT_GPT2 */
	irq_request_free(MXC_INT_GPT2);
}

int do_irq_test( cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if(strncmp(argv[1],"list",4)==0)
	{
		list_active_irqs();
	}
	else if(strncmp(argv[1],"timeron",7)==0)
	{
		gpt2_init();
		printf("init finish\n");
	}
	else if(strncmp(argv[1],"timeroff",8)==0)
	{
		gpt2_finished();
	}
	return 0;
}


U_BOOT_CMD(
	irqtest,3,	1,do_irq_test,
	"test irq",
	"irqtest list|timeron|timeroff [irq_dec]"
);


