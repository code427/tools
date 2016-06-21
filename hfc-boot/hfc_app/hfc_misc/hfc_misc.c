#include <common.h>
#include <command.h>
#include <linux/types.h>
#include <malloc.h>
#include <config.h>
#include <asm/arch-mx25/imx-regs.h>
#include <asm/arch-mx25/mx25_pins.h>
#include <asm/arch-mx25/iomux.h>
#include <asm/arch-mx25/mx25.h>
#include <asm/io.h>
#include <asm/spi.h>
#include <asm/gpio.h>
#include "hfc_misc.h"


#undef	HFC_MISC_DEBUG
//#define	HFC_MISC_DEBUG
#ifdef	HFC_MISC_DEBUG
#define	MISC_DBG(fmt, args...)	printf(fmt , ##args)
#else
#define MISC_DBG(fmt, args...)
#endif


#define BAT_AD_INPUT		0
#define RTC_AD_INPUT		1
#define ACIN_AD_INPUT		7




/* TSC General Config Register */
#define TGCR                  0x000
#define TGCR_IPG_CLK_EN       (1 << 0)
#define TGCR_TSC_RST          (1 << 1)
#define TGCR_POWERMODE_SHIFT  8
#define TGCR_POWER_SAVE       (0x1 << TGCR_POWERMODE_SHIFT)
#define TGCR_INTREFEN         (1 << 10)
#define TGCR_POWER_MASK       (0x3 << TGCR_POWERMODE_SHIFT)



#define IMX_ADC_FIFO_MAX            16

#define HC595_RTC_ADEN				21
#define HC4051_SELECT0              6
#define HC4051_SELECT1              3
#define HC4051_SELECT2              7

#define TSC_BASE_ADDR		0x50030000

enum {
	EXP_AD_VABT1_AD = 0,
	EXP_AD_VABT2_AD,
	EXP_AD_IABT_AD1,
	EXP_AD_IABT_AD2,
	EXP_AD_MACHINE_AD1,
	EXP_AD_MACHINE_AD2,
	EXP_AD_VERSION_AD2,
	EXP_AD_ACIN_AD,
}; 


static int hfc_adc_ready = 0;
static unsigned int tsc_base = TSC_BASE_ADDR;
unsigned short adc_buf[IMX_ADC_FIFO_MAX]={0};
static unsigned int batVoltage = 0;

extern void gpio_set_hc595_output(int num, int value);
extern void hfc_libat_measure_control(int val);
extern void hfc_hc595_data_latch_init(void);
extern void hfc_battery_charge_gpio_init(void);

void HC4051_opertions(unsigned int channel)
{       
	if(channel > 7)
		return;

	channel &= 0x07;
	gpio_set_hc595_output(HC4051_SELECT0,(channel&0x01)?1:0);
	udelay(2);
	gpio_set_hc595_output(HC4051_SELECT1,(channel&0x02)?1:0);
	udelay(2);
	gpio_set_hc595_output(HC4051_SELECT2,(channel&0x04)?1:0);
	udelay(5); 
} 


static int hfc_adc_init(void)
{
	//tsc clock enable
    unsigned int reg;
		    
	reg = __raw_readl(tsc_base + TGCR);
	reg |= TGCR_IPG_CLK_EN;
	__raw_writel(reg, tsc_base + TGCR);

	//tsc self reset
	reg = __raw_readl(tsc_base + TGCR);
	reg |= TGCR_TSC_RST;
	__raw_writel(reg, tsc_base + TGCR);

	while (__raw_readl(tsc_base + TGCR) & TGCR_TSC_RST)
		continue;

	/* external volt reference */
	reg = __raw_readl(tsc_base + TGCR);
	reg &= ~TGCR_INTREFEN;
	__raw_writel(reg, tsc_base + TGCR);

	/* Set power mode */
	reg = __raw_readl(tsc_base + TGCR) & ~TGCR_POWER_MASK;
	reg |= TGCR_POWER_SAVE;
	__raw_writel(reg, tsc_base + TGCR);
	
	hfc_adc_ready = 1;

	return 0;
}	

void enable_adc_clock(void)
{
	unsigned int reg = 0;

    reg = __raw_readl(IMX_CCM_BASE + 0x14);
	reg |=( 0x1 << 13);
	__raw_writel(reg, IMX_CCM_BASE + 0x14);
}

int hfc_adc_module_init(void)
{
	int ret = 0;

	if(hfc_adc_ready == 1)
		return 0;

	ret = hfc_adc_init();
	if(ret)
	{
		printf("adc init failed!\n");
	}

	enable_adc_clock();

	return ret;
}

#define CQCR_FQS              (1 << 2)
#define CQCR_LAST_ITEM_ID_SHIFT   4
#define CQCR_LAST_ITEM_ID_MASK    (0xf << CQCR_LAST_ITEM_ID_SHIFT)
#define CQCR_FIFOWATERMARK_SHIFT  8
#define CQCR_FIFOWATERMARK_MASK   (0xf << CQCR_FIFOWATERMARK_SHIFT)
#define TSC_GENERAL_ADC_GCC0   0x175c

#define GCQFIFO                0x800
#define GCQFIFO_ADCOUT_SHIFT   4 
/* GeneralADC Convert Queue Control Register */
#define GCQCR                  0x804
/* GeneralADC Convert Queue Status Register */
#define GCQSR                  0x808
#define CQSR_EOQ              (1 << 1)

#define GCC0                   0x840
#define CQSR_EMPT             (1 << 13)

#define CQCR_QSM_SHIFT        0
#define CQCR_QSM_FQS          (0x2 << CQCR_QSM_SHIFT)

#define CC_NOS_SHIFT          16
#define CC_SETTLING_TIME_SHIFT 24

int adc_read_general(unsigned short *result)
{
	unsigned int reg;
	unsigned int data_num = 0;
	reg = __raw_readl(tsc_base + GCQCR);
	reg |= CQCR_FQS;
	__raw_writel(reg, tsc_base + GCQCR);
	
	while(!(__raw_readl(tsc_base + GCQSR) & CQSR_EOQ));
	
	reg = __raw_readl(tsc_base + GCQCR);
	reg &= ~CQCR_FQS;
	__raw_writel(reg, tsc_base + GCQCR);
	
	reg = __raw_readl(tsc_base + GCQSR);
	reg |= CQSR_EOQ;
	__raw_writel(reg, tsc_base + GCQSR);

	while (!(__raw_readl(tsc_base + GCQSR) & CQSR_EMPT))
	{
		result[data_num] = __raw_readl(tsc_base + GCQFIFO) >> GCQFIFO_ADCOUT_SHIFT;
		data_num++;
	}
	return 0;
}

int imx_adc_convert(unsigned short *result)
{
	int reg = 0;

	reg= (0xf << CQCR_FIFOWATERMARK_SHIFT) | CQCR_QSM_FQS;
	__raw_writel(reg, tsc_base + GCQCR);
				   
	reg = TSC_GENERAL_ADC_GCC0;
	reg |= (4 << CC_NOS_SHIFT); 
	reg	|= (16 << CC_SETTLING_TIME_SHIFT);
	__raw_writel(reg, tsc_base + GCC0);
	
	adc_read_general(result);

	return 0;
}

static unsigned int calc_adc_voltage(unsigned short *adc_buf)
{
	int i;
	unsigned short max=0,min=0xFFFF;
	unsigned int sum=0;
	unsigned int val;

	for(i=0; i < IMX_ADC_FIFO_MAX; i++)
	{
		if(0 == adc_buf[i])
			break;
		
		max = max>adc_buf[i]?max:adc_buf[i];
		min = min<adc_buf[i]?min:adc_buf[i];
		sum += ((adc_buf[i]));
	}
	
	if(i < 4) 
		return 0;

	val = ((sum-max-min)/(i - 2))*1338 >> 12;
	
	return val;
}

void hfc_li_rtc_control(int ch,int val)
{
	if(0 == ch)
	{
		hfc_libat_measure_control(val);
	}
	else
	{
		gpio_set_hc595_output(HC595_RTC_ADEN,val);
	}

}

int get_bat_voltage(int index)
{
	if(hfc_adc_module_init())
		printf("ADC init failed \n");
    
	if(hfc_adc_ready)
	{
		memset(adc_buf,0,sizeof(adc_buf));
		HC4051_opertions(index);
		udelay(100);
		hfc_li_rtc_control(index,1);
		udelay(10);
		hfc_li_rtc_control(index,0);
		udelay(10);
		if(imx_adc_convert(adc_buf))
		{
			printf("ADC Sample failed \n");
			return -1;
		}
		udelay(220000);
		hfc_li_rtc_control(index,1);
		
		batVoltage = calc_adc_voltage(adc_buf);
		
		MISC_DBG("vol = %d \n",batVoltage);
		return batVoltage;
	}
	else
	{
		return -1;
	}
}


int get_external_supply_voltage(void)
{
	int avg=0;
	int ret = 0;
	
	if(hfc_adc_module_init())
		printf("ADC init failed \n");
	
	if(hfc_adc_ready)
	{
		HC4051_opertions(EXP_AD_ACIN_AD);
		udelay(5000);
		memset(adc_buf,0,sizeof(adc_buf));
		ret = imx_adc_convert(adc_buf);
		if(ret)
		{
			return -1;
		}
		avg = calc_adc_voltage(adc_buf);
		
		MISC_DBG("get ac avg:%d\n",avg);
	
		return avg;
	}
	return -1;
}

void hfc_power_latch_enable(void)
{
	mxc_request_iomux(MX25_PIN_A14, MUX_CONFIG_ALT5);
	mxc_iomux_set_pad(MX25_PIN_A14, (PAD_CTL_PKE_ENABLE));
	mxc_set_gpio_direction(MX25_PIN_A14, 0);
	mxc_set_gpio_dataout(MX25_PIN_A14, 1);
}

int do_misc(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned char channel;
	static int misc_init = 0;
	
	if(strncmp(argv[1],"init",4)==0)
	{
		hfc_hc595_data_latch_init();
		hfc_battery_charge_gpio_init();
		misc_init = 1;
	}
	
	if(!misc_init)
	{
		MISC_DBG("Misc no init\n");
		return 1;
	}

	if (argc < 2)
		return CMD_RET_USAGE;
	
	if(strncmp(argv[1],"pwr-lock",7)==0)
	{
		hfc_power_latch_enable();
	}
	else if(strncmp(argv[1],"hc595",5)==0)
	{
		channel = (unsigned char)simple_strtoul(argv[2], NULL, 10);
		printf("control hc595 %d on/off\n",channel);
		if(strncmp(argv[3],"on",2)==0)
		{
			gpio_set_hc595_output(channel,1);
		}
		else if(strncmp(argv[3],"off",3)==0)
		{
			gpio_set_hc595_output(channel,0);
		}
		else
		{
			printf("param error.\n");
		}
	}
	else if(strncmp(argv[1],"adc",3)==0)
	{
		if(strncmp(argv[2],"battery",5)==0)
		{
			get_bat_voltage(0);
		}
		else if(strncmp(argv[2],"rtc",5)==0)
		{
			get_bat_voltage(1);
		}

	}
	else if(strncmp(argv[1],"pwr",3)==0)
	{
		get_external_supply_voltage();
	}
	return 0;
}


U_BOOT_CMD(
	misc,	5,	1,	do_misc,
	"hfc misc device control",
	"misc power init|pwr-lock|hc595|adc|pwr\n"
);
