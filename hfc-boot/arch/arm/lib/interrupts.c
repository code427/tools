/*
 * (C) Copyright 2003
 * Texas Instruments <www.ti.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002-2004
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * (C) Copyright 2004
 * Philippe Robin, ARM Ltd. <philippe.robin@arm.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/proc-armv/ptrace.h>
#include "irq.h"

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_USE_IRQ

#define	IRQ_DEBUG

#ifdef  IRQ_DEBUG
# define DEBUG_IRQ(fmt, args...)	do \
								{  \
										printf(fmt ,##args); \
								} while(0)
#else
# define DEBUG_IRQ(fmt, args...)
#endif

#define IRQ_FLAG_INUSE		0x01

struct irq_s 
{
	irq_handler_t irq_handler;
	unsigned int flag;
	char name[IRQ_DESC_NAME_LEN];
};

static struct irq_s  def_irq_handler[MXC_INTERNAL_IRQS];

static void irq_chip_enable(int irq_num)
{
	unsigned int reg = 0; 
	if(irq_num > 63) 
		return;
	
	if(irq_num > 31)
	{
		reg = __REG(IMX_ARM926_ASIC + AVIC_INTENABLEH);
		reg = reg|(0x001<<(irq_num-32));
			
		__REG(IMX_ARM926_ASIC + AVIC_INTENABLEH) = reg;
		__REG(IMX_ARM926_ASIC + AVIC_INTENNUM) = irq_num;
		return ;	
	}
	
	reg = __REG(IMX_ARM926_ASIC + AVIC_INTENABLEL);
	reg = reg|(0x001<<irq_num);
	
	__REG(IMX_ARM926_ASIC + AVIC_INTENABLEL) = reg;
	__REG(IMX_ARM926_ASIC + AVIC_INTENNUM) = irq_num;
	
	return;				
}

static void irq_chip_disable(int irq_num)
{
	unsigned int reg = 0; 
	
	if(irq_num > 63) 
		return;

	if(irq_num > 31)
	{
		reg = __REG(IMX_ARM926_ASIC + AVIC_INTENABLEH);
		reg = reg&(~(0x001<<(irq_num-32)));
			
		// disable irq
		__REG(IMX_ARM926_ASIC + AVIC_INTENABLEH) = reg;
		
		// disable irq source
		__REG(IMX_ARM926_ASIC + AVIC_INTDISNUM) = irq_num;
		return ;	
	}

	reg = __REG(IMX_ARM926_ASIC + AVIC_INTENABLEL);
	
	reg = reg&(~(0x001<<irq_num));
	
	// disable irq
	__REG(IMX_ARM926_ASIC + AVIC_INTENABLEL) = reg;
	
	// disable irq source
	__REG(IMX_ARM926_ASIC + AVIC_INTDISNUM) = irq_num;
	
	return ;				
}

static int set_irq_handler(unsigned int irq_num, irq_handler_t func,char *name )
{
	if(irq_num < 0 || irq_num > 63) 
		return -1;
	
	if(def_irq_handler[irq_num].irq_handler)	
		return -1;	
	
	def_irq_handler[irq_num].irq_handler= func;
	
	if(NULL == name)
		sprintf(def_irq_handler[irq_num].name,"defaut:%d",irq_num);
	else
	{
		if(strlen(name)>= IRQ_DESC_NAME_LEN)
			memcpy(def_irq_handler[irq_num].name,name,IRQ_DESC_NAME_LEN-1);
		else
			strcpy(def_irq_handler[irq_num].name,name);
	}

	def_irq_handler[irq_num].flag |= IRQ_FLAG_INUSE;
	return 0;
}

static void clr_irq_handler(unsigned int irq_num)
{
	if(irq_num < 0 || irq_num > 63) return ;
	
	def_irq_handler[irq_num].irq_handler= NULL;
	memset(def_irq_handler[irq_num].name,0,IRQ_DESC_NAME_LEN);
	def_irq_handler[irq_num].flag &= (~IRQ_FLAG_INUSE);
}

static void init_irq_default_handler(void)
{
	int i=0;
	for(i=0;i< MXC_INTERNAL_IRQS;i++)
	{
		def_irq_handler[i].irq_handler= NULL;
		memset(def_irq_handler[i].name,0,IRQ_DESC_NAME_LEN);
		def_irq_handler[i].flag = 0;
	}
}

static void run_irq_handler(unsigned int irq)
{

	if(def_irq_handler[irq].irq_handler &&(def_irq_handler[irq].flag&IRQ_FLAG_INUSE))
	{
		def_irq_handler[irq].irq_handler(irq);
	}
}

int irq_request(unsigned int irq_num, irq_handler_t func,char *name )
{
	int ret =0;
	int flag =0;
	if(MXC_INT_GPT1 == irq_num)
	{
		DEBUG_IRQ("GPT1 as system default timer,can't be used %s \n",__FUNCTION__);
		ret =-1;
		goto errout;
	}
	
	if(irq_num >=(MXC_INTERNAL_IRQS + MAX_GPIO_PIN_NUM))
	{
		DEBUG_IRQ("irq out of range %s \n",__FUNCTION__);
		ret =-1;
		goto errout;
	}
	
	flag = disable_interrupts();
	if(irq_num < MXC_INTERNAL_IRQS)
	{
		irq_chip_enable(irq_num);
		ret = set_irq_handler(irq_num,func,name);
	}
	
	if(flag) 
	{
		enable_interrupts();
	}

errout:
	return ret;
}


int irq_request_free(unsigned int irq_num)
{

	int ret =0;
	int flag_irq=0;
	if(irq_num >=(MXC_INTERNAL_IRQS + MAX_GPIO_PIN_NUM))
	{
		DEBUG_IRQ("irq out of range %s \n",__FUNCTION__);
		ret =-1;
		goto errout;
	}
	
	flag_irq = disable_interrupts();

	if(irq_num < MXC_INTERNAL_IRQS)
	{
		irq_chip_disable(irq_num);
		clr_irq_handler(irq_num);
	}
	if(flag_irq) 
		enable_interrupts();

errout:
	return ret;
}

void disable_all_interrupts(void)
{
	unsigned int irq_num=0;
	
	disable_interrupts();

	for(irq_num=0;irq_num < MXC_INTERNAL_IRQS;irq_num++) 
	{
		irq_chip_disable(irq_num);
		clr_irq_handler(irq_num);
	}
}

void list_active_irqs(void)
{
	int i=0;
	
	DEBUG_IRQ("---------- LIST IRQS ------------- \n");
	DEBUG_IRQ(" IRQ\t\tNAME\n");
	for(i=0;i< MXC_INTERNAL_IRQS;i++)
	{
		if(def_irq_handler[i].flag & IRQ_FLAG_INUSE)
		{
			DEBUG_IRQ("%04d\t\t%s\n",i,def_irq_handler[i].name);
		}
	}
}

int	arch_interrupt_init	(void)
{
/* 
	Before enable interrupt pls copy interrupt vector to 0x7801FFC0 
	ASIC means ARM Simple Interrupt Controllor
*/	
	int i;

	disable_all_interrupts();

	init_irq_default_handler();

	/* put the AVIC into the reset value with
	 * irq enable and fiq disable 
	 */
	__REG(IMX_ARM926_ASIC + AVIC_INTCNTL) = (1<<21);
	
	/* Disable all normal interrupts */
	__REG(IMX_ARM926_ASIC + AVIC_NIMASK) = 0x1f;

	/* disable all interrupts */
	__REG(IMX_ARM926_ASIC + AVIC_INTENABLEH) = 0;
	__REG(IMX_ARM926_ASIC + AVIC_INTENABLEL) = 0;

	/* all IRQ generated no FIQ */
	__REG(IMX_ARM926_ASIC + AVIC_INTTYPEH) = 0;
	__REG(IMX_ARM926_ASIC + AVIC_INTTYPEL) = 0;

	/* Set default priority value (0) for all IRQ's */
	for (i = 0; i < 8; i++)
		__REG(IMX_ARM926_ASIC + AVIC_NIPRIORITY(i)) = 0;
	
	return 0;
}

static void irq_handler_server(void)
{
	unsigned int irq = 0;
	
	irq = (__REG(IMX_ARM926_ASIC + AVIC_NIVECSR) >> 16);
	
	/* run irq handler */
	run_irq_handler(irq);
}

int interrupt_init (void)
{
	/*
	 * setup up stacks if necessary
	 */
	IRQ_STACK_START = gd->irq_sp - 4;
	IRQ_STACK_START_IN = IRQ_STACK_START + 8;
	FIQ_STACK_START = IRQ_STACK_START - CONFIG_STACKSIZE_IRQ;

	return arch_interrupt_init();
}

/* enable IRQ interrupts */
void enable_interrupts (void)
{
	unsigned long temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "bic %0, %0, #0x80\n"
			     "msr cpsr_c, %0"
			     : "=r" (temp)
			     :
			     : "memory");
}

/*
 * disable IRQ/FIQ interrupts
 * returns true if interrupts had been enabled before we disabled them
 */
int disable_interrupts (void)
{
	unsigned long old,temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "orr %1, %0, #0xc0\n"
			     "msr cpsr_c, %1"
			     : "=r" (old), "=r" (temp)
			     :
			     : "memory");
	return (old & 0x80) == 0;
}
#else
int interrupt_init (void)
{
	/*
	 * setup up stacks if necessary
	 */
	IRQ_STACK_START_IN = gd->irq_sp + 8;
	return 0;
}

void enable_interrupts (void)
{
	return;
}
int disable_interrupts (void)
{
	return 0;
}
#endif


void bad_mode(void)
{
	panic("Resetting CPU ...\n");
	reset_cpu(0);
}

#if 0//def  IRQ_DEBUG
void show_regs (struct pt_regs *regs)
{
	unsigned long flags;
	const char *processor_modes[] = {
	"USER_26",	"FIQ_26",	"IRQ_26",	"SVC_26",
	"UK4_26",	"UK5_26",	"UK6_26",	"UK7_26",
	"UK8_26",	"UK9_26",	"UK10_26",	"UK11_26",
	"UK12_26",	"UK13_26",	"UK14_26",	"UK15_26",
	"USER_32",	"FIQ_32",	"IRQ_32",	"SVC_32",
	"UK4_32",	"UK5_32",	"UK6_32",	"ABT_32",
	"UK8_32",	"UK9_32",	"UK10_32",	"UND_32",
	"UK12_32",	"UK13_32",	"UK14_32",	"SYS_32",
	};

	flags = condition_codes (regs);

	DEBUG_IRQ("pc : [<%08lx>]	   lr : [<%08lx>]\n"
		"sp : %08lx  ip : %08lx	 fp : %08lx\n",
		instruction_pointer (regs),
		regs->ARM_lr, regs->ARM_sp, regs->ARM_ip, regs->ARM_fp);
	DEBUG_IRQ("r10: %08lx  r9 : %08lx	 r8 : %08lx\n",
		regs->ARM_r10, regs->ARM_r9, regs->ARM_r8);
	DEBUG_IRQ("r7 : %08lx  r6 : %08lx	 r5 : %08lx  r4 : %08lx\n",
		regs->ARM_r7, regs->ARM_r6, regs->ARM_r5, regs->ARM_r4);
	DEBUG_IRQ("r3 : %08lx  r2 : %08lx	 r1 : %08lx  r0 : %08lx\n",
		regs->ARM_r3, regs->ARM_r2, regs->ARM_r1, regs->ARM_r0);
	DEBUG_IRQ("Flags: %c%c%c%c",
		flags & CC_N_BIT ? 'N' : 'n',
		flags & CC_Z_BIT ? 'Z' : 'z',
		flags & CC_C_BIT ? 'C' : 'c', flags & CC_V_BIT ? 'V' : 'v');
	DEBUG_IRQ("  IRQs %s  FIQs %s  Mode %s%s\n",
		interrupts_enabled (regs) ? "on" : "off",
		fast_interrupts_enabled (regs) ? "on" : "off",
		processor_modes[processor_mode (regs)],
		thumb_mode (regs) ? " (T)" : "");
	return;
}
#else
void show_regs (struct pt_regs *regs)
{
	return;
}
#endif

void do_undefined_instruction (struct pt_regs *pt_regs)
{
	DEBUG_IRQ("undefined instruction\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_software_interrupt (struct pt_regs *pt_regs)
{
	DEBUG_IRQ("software interrupt\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_prefetch_abort (struct pt_regs *pt_regs)
{
	DEBUG_IRQ("prefetch abort\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_data_abort (struct pt_regs *pt_regs)
{
	DEBUG_IRQ("data abort\n\n    MAYBE you should read doc/README.arm-unaligned-accesses\n\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_not_used (struct pt_regs *pt_regs)
{
	DEBUG_IRQ("not used\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_fiq (struct pt_regs *pt_regs)
{
	DEBUG_IRQ("fast interrupt request\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_irq(struct pt_regs *pt_regs)
{
	//DEBUG_IRQ("interrupt request\n");
	irq_handler_server();
}
