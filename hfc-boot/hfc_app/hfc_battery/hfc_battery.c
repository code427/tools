#include <asm/io.h>
#include <common.h>
#include <image.h>
#include <asm/errno.h>
#include <linux/types.h>
#include <malloc.h>
#include <lmb.h>
#include "hfc_battery.h"


#undef	HFC_BATTERY_DEBUG
//#define	HFC_BATTERY_DEBUG
#ifdef	HFC_BATTERY_DEBUG
#define	BAT_DBG(fmt, args...)	printf(fmt , ##args)
#else
#define BAT_DBG(fmt, args...)
#endif

#define AD_SAMPLE_SIZE	10

#define BAT_DIS_LEVEL1_LOW	608
#define BAT_DIS_LEVEL1_HIG	729
#define BAT_DIS_LEVEL2_LOW	731
#define BAT_DIS_LEVEL2_HIG	749
#define BAT_DIS_LEVEL3_LOW	751
#define BAT_DIS_LEVEL3_HIG	769
#define BAT_DIS_LEVEL4_LOW	771
#define BAT_DIS_LEVEL4_HIG	820

#define BAT_DETECT_FULL		810
#define BAT_DIS_FULL		840

#define BAT_LEVEL1		5
#define BAT_LEVEL2		4
#define BAT_LEVEL3		3
#define BAT_LEVEL4		2
#define BAT_LEVEL5		1


unsigned int ad_value[AD_SAMPLE_SIZE] = {0};
	
static unsigned char level = 0;
static unsigned char counter = 120;
volatile unsigned char bat_loop;

volatile unsigned int bat_voltage;

struct bat_charge_stat
{
	int supply;			// 1 ext , 0 bat
	int status;			// 0 no charge ,1 lite ,2 large,3  full
	int battery;		// 1 bat exist, 0 NE.
	int voltage;		// average voltage for displayed 
};

extern void tft_fill_battery_img(void);
extern void tft_display_battery_icon(unsigned char icon);
extern int get_bat_voltage(int index);
extern void lcd_display_voltage(int volt);
	

static int battery_sample(void)
{
	return get_bat_voltage(0);
}

static void battery_charge_process(void)
{
}

static unsigned char battery_level_confirm(int vol)
{
	if(vol < BAT_DIS_LEVEL1_LOW)
	{
		BAT_DBG("volt too low\n");
		level = BAT_LEVEL1;
	}
	else if((vol >= BAT_DIS_LEVEL1_LOW) && (vol <= BAT_DIS_LEVEL1_HIG))
	{
		level = BAT_LEVEL2;
	}
	else if((vol >= BAT_DIS_LEVEL2_LOW) && (vol <= BAT_DIS_LEVEL2_HIG))
	{
		level = BAT_LEVEL2;
	}
	else if((vol >= BAT_DIS_LEVEL3_LOW) && (vol <= BAT_DIS_LEVEL3_HIG))
	{
		level = BAT_LEVEL3;
	}
	else if((vol >= BAT_DIS_LEVEL4_LOW) && (vol <= BAT_DIS_LEVEL4_HIG))
	{
		level = BAT_LEVEL4;
	}
	else if(vol > BAT_DIS_LEVEL4_HIG)
	{
		level = BAT_LEVEL5;
	}
	else
	{ 
		level = BAT_LEVEL2;
	}

	return level;
}

static void battery_dis_level(int level)
{
	int dicon;

	tft_fill_battery_img();
	tft_display_battery_icon(0);
	if(BAT_LEVEL5 == level)
	{
		tft_display_battery_icon(4);
		return;
	}
	else if(BAT_LEVEL1 == level)
	{
		return;
	}
	else
	{
		counter--;
		if(!counter)
		{
			counter = 120;
		}
		BAT_DBG("counter %d level %d\n",counter,level);
		
		dicon = counter%level;
		dicon = (~dicon)&0x3;
		
		BAT_DBG("dicon is %d\n",dicon);
		tft_display_battery_icon(dicon);
	}
}

static void battery_icon_display(int volt)
{
	lcd_display_voltage(volt);
	
	level = battery_level_confirm(volt);
	
	if(BAT_LEVEL5 == level)
	{
		lcd_display_voltage(BAT_DIS_FULL);
	}
	else if(BAT_LEVEL1 == level)
	{
		lcd_display_voltage(0);
	}
	
	BAT_DBG("Bat level %d\n",level);

	battery_dis_level(level);
}


int get_expower_supply_stat(void)
{
	
	return 0;
}

int hfc_battery_manager(void)
{
	bat_voltage = battery_sample();
	battery_charge_process();
	
	bat_loop++;
	if(bat_loop > 5) //1 sec per times
	{
		bat_loop = 0;
		battery_icon_display(bat_voltage);
	}
	return 0;
}

