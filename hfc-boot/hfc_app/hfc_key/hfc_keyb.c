/*
 * boot keypad driver
 */

#include <asm/io.h>
#include <common.h>
#include <asm/errno.h>
#include <linux/types.h>
#include <malloc.h>
#include <asm/arch-mx25/mx25_pins.h>
#include <asm/arch-mx25/imx-regs.h>
#include <asm/arch-mx25/iomux.h>
#include "hfc_keyb.h"

#define CONFIG_SYS_HZ				1000
#define CONFIG_MXC_KPD_COLMAX 		4
#define CONFIG_MXC_KPD_ROWMAX 		5

#define LONG_KEY_COUNTER			300
/*!
 * Comment KPP_DEBUG to disable debug messages
 */
int setup_mxc_kpd(void)
{
    mxc_request_iomux(MX25_PIN_KPP_ROW0, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_KPP_ROW1, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_KPP_ROW2, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_KPP_ROW3, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_KPP_COL0, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_KPP_COL1, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_KPP_COL2, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_KPP_COL3, MUX_CONFIG_FUNC);
    mxc_request_iomux(MX25_PIN_GPIO_A, MUX_CONFIG_ALT3);

#define KPP_PAD_CTL_ROW (PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_PUD | \
             PAD_CTL_100K_PU)
#define KPP_PAD_CTL_COL (PAD_CTL_PKE_ENABLE | PAD_CTL_PUE_PUD | \
             PAD_CTL_100K_PU | PAD_CTL_ODE_OpenDrain)

    mxc_iomux_set_pad(MX25_PIN_KPP_ROW0, KPP_PAD_CTL_ROW);
    mxc_iomux_set_pad(MX25_PIN_KPP_ROW1, KPP_PAD_CTL_ROW);
    mxc_iomux_set_pad(MX25_PIN_KPP_ROW2, KPP_PAD_CTL_ROW);
    mxc_iomux_set_pad(MX25_PIN_KPP_ROW3, KPP_PAD_CTL_ROW);
    mxc_iomux_set_pad(MX25_PIN_KPP_COL0, KPP_PAD_CTL_COL);
    mxc_iomux_set_pad(MX25_PIN_KPP_COL1, KPP_PAD_CTL_COL);
    mxc_iomux_set_pad(MX25_PIN_KPP_COL2, KPP_PAD_CTL_COL);
    mxc_iomux_set_pad(MX25_PIN_KPP_COL3, KPP_PAD_CTL_COL);
	mxc_iomux_set_pad(MX25_PIN_GPIO_A, KPP_PAD_CTL_ROW);
    mxc_iomux_set_input(MUX_IN_KPP_IPP_IND_ROW_4, INPUT_CTL_PATH1);

     return 0;
}


#undef	KPP_DEBUG
//#define	KPP_DEBUG
#ifdef	KPP_DEBUG
#define	KPP_PRINTF(fmt, args...)	printf(fmt , ##args)
#else
#define KPP_PRINTF(fmt, args...)
#endif

/*!
 * This structure holds the keypad private data structure.
 */
static struct keypad_priv kpp_dev;

/*! Indicates if the key pad device is enabled. */

/*! This static variable indicates whether a key event is pressed/released. */
static unsigned short KPress;

/*! cur_rcmap and prev_rcmap array is used to detect key press and release. */
static unsigned short *cur_rcmap;	/* max 64 bits (8x8 matrix) */
static unsigned short *prev_rcmap;

static unsigned char k_KeyV[17];
volatile unsigned int rtkey=0;
volatile unsigned int long_key_count=0;
volatile unsigned int mxc_long_key_flag=0;
volatile unsigned int special_key = 0;

/*!
 * These arrays are used to store press and release scancodes.
 */
static short **press_scancode;
static short **release_scancode;

//static const unsigned short *mxckpd_keycodes;
static unsigned short mxckpd_keycodes_size;

/*!
 * These functions are used to configure and the GPIO pins for keypad to
 * activate and deactivate it.
 */
static u32 key_tremble_times = 0;

/*!
 * This function is called to scan the keypad matrix to find out the key press
 * and key release events. Make scancode and break scancode are generated for
 * key press and key release events.
 *
 * The following scanning sequence are done for
 * keypad row and column scanning,
 * -# Write 1's to KPDR[15:8], setting column data to 1's
 * -# Configure columns as totem pole outputs(for quick discharging of keypad
 * capacitance)
 * -# Configure columns as open-drain
 * -# Write a single column to 0, others to 1.
 * -# Sample row inputs and save data. Multiple key presses can be detected on
 * a single column.
 * -# Repeat steps the above steps for remaining columns.
 * -# Return all columns to 0 in preparation for standby mode.
 * -# Clear KPKD and KPKR status bit(s) by writing to a 1,
 *    Set the KPKR synchronizer chain by writing "1" to KRSS register,
 *    Clear the KPKD synchronizer chain by writing "1" to KDSC register
 *
 * @result    Number of key pressed/released.
 */

static int mxc_kpp_scan_matrix(void)
{
	
	unsigned short reg_val;
	int col, row;
	short scancode = 0;
	int keycnt = 0;		/* How many keys are still pressed */

	/* save cur keypad matrix to prev */
	memcpy(prev_rcmap, cur_rcmap, kpp_dev.kpp_rows * sizeof(prev_rcmap[0]));
	memset(cur_rcmap, 0, kpp_dev.kpp_rows * sizeof(cur_rcmap[0]));	

	//***************************************************************************
	/* 2. Write 1.s to KPDR[15:8] setting column data to 1.s */
	reg_val = __raw_readw(KPDR);
	reg_val |= 0xff00;
	__raw_writew(reg_val, KPDR);

	/*
	 * 3. Configure columns as totem pole outputs(for quick
	 * discharging of keypad capacitance)
	 */
	reg_val = __raw_readw(KPCR);
	reg_val &= 0x00ff;
	__raw_writew(reg_val, KPCR);

	udelay(2);

	/*
	 * 4. Configure columns as open-drain
	 */
	reg_val = __raw_readw(KPCR);
	reg_val |= ((1 << kpp_dev.kpp_cols) - 1) << 8;
	__raw_writew(reg_val, KPCR);

	/*
	 * 5. Write a single column to 0, others to 1.
	 * 6. Sample row inputs and save data. Multiple key presses
	 * can be detected on a single column.
	 * 7. Repeat steps 2 - 6 for remaining columns.
	 */
	 
	/* Col bit starts at 8th bit in KPDR */
	reg_val = __raw_readw(KPDR);
	reg_val |= 0xff00;
	__raw_writew(reg_val, KPDR);//

	/* Delay added to avoid propagating the 0 from column to row
	 * when scanning. */

	
	udelay(5);

	//*****************************************************************************
	for (col = 0; col < kpp_dev.kpp_cols; col++) {	/* Col */
		/* 2. Write 1.s to KPDR[15:8] setting column data to 1.s */
		reg_val = __raw_readw(KPDR);
		reg_val |= 0xff00;
		__raw_writew(reg_val, KPDR);

		/*
		 * 3. Configure columns as totem pole outputs(for quick
		 * discharging of keypad capacitance)
		 */
		reg_val = __raw_readw(KPCR);
		reg_val &= 0x00ff;
		__raw_writew(reg_val, KPCR);

		udelay(2);

		/*
		 * 4. Configure columns as open-drain
		 */
		reg_val = __raw_readw(KPCR);
		reg_val |= ((1 << kpp_dev.kpp_cols) - 1) << 8;
		__raw_writew(reg_val, KPCR);

		/*
		 * 5. Write a single column to 0, others to 1.
		 * 6. Sample row inputs and save data. Multiple key presses
		 * can be detected on a single column.
		 * 7. Repeat steps 2 - 6 for remaining columns.
		 */

		/* Col bit starts at 8th bit in KPDR */
		reg_val = __raw_readw(KPDR);
		reg_val &= ~(1 << (8 + col));
		__raw_writew(reg_val, KPDR);

		/* Delay added to avoid propagating the 0 from column to row
		 * when scanning. */

		udelay(5);

		/* Read row input */
		reg_val = __raw_readw(KPDR);
		for (row = 0; row < kpp_dev.kpp_rows; row++) {	/* sample row */
			if (TEST_BIT(reg_val, row) == 0) 
			{
				cur_rcmap[row] = BITSET(cur_rcmap[row], col);
				keycnt++;
			}
		}
	}

	/*
	 * 8. Return all columns to 0 in preparation for standby mode.
	 * 9. Clear KPKD and KPKR status bit(s) by writing to a .1.,
	 * set the KPKR synchronizer chain by writing "1" to KRSS register,
	 * clear the KPKD synchronizer chain by writing "1" to KDSC register
	 */
	reg_val = 0x00;
	__raw_writew(reg_val, KPDR);
	
	reg_val = __raw_readw(KPSR);
	reg_val |= KBD_STAT_KPKD | KBD_STAT_KPKR | KBD_STAT_KRSS | KBD_STAT_KDSC;
	
	__raw_writew(reg_val, KPSR);

	/*
	 * prev_rcmap array will contain the previous status of the keypad
	 * matrix.  cur_rcmap array will contains the present status of the
	 * keypad matrix. If a bit is set in the array, that (row, col) bit is
	 * pressed, else it is not pressed.
	 *
	 * XORing these two variables will give us the change in bit for
	 * particular row and column.  If a bit is set in XOR output, then that
	 * (row, col) has a change of status from the previous state.  From
	 * the diff variable the key press and key release of row and column
	 * are found out.
	 *
	 * If the key press is determined then scancode for key pressed
	 * can be generated using the following statement:
	 *    scancode = ((row * 8) + col);
	 *
	 * If the key release is determined then scancode for key release
	 * can be generated using the following statement:
	 *    scancode = ((row * 8) + col) + MXC_KEYRELEASE;
	 */
	for (row = 0; row < kpp_dev.kpp_rows; row++) {
		unsigned char diff;

		/*
		 * Calculate the change in the keypad row status
		 */
		diff = prev_rcmap[row] ^ cur_rcmap[row];

		for (col = 0; col < kpp_dev.kpp_cols; col++) {
			if ((diff >> col) & 0x1)
			{		
				/* There is a status change on col */
				if ((prev_rcmap[row] & BITSET(0, col)) == 0) {
		
					/*
					 * Previous state is 0, so now
					 * a key is pressed
					 */
					scancode = ((row * kpp_dev.kpp_cols) + col);
					KPress = 1;
					kpp_dev.iKeyState = KStateUp;

					KPP_PRINTF("Press   (%d, %d) scan=%d " "Kpress=%d\n", row, col, scancode, KPress);
					press_scancode[row][col] =  (short)scancode;
				} 
			}else{
				long_key_count++;
				if(long_key_count > LONG_KEY_COUNTER)
				{
					long_key_count = 0;
					mxc_long_key_flag = 1;
				}
			}
		}
	}

	return keycnt;
}

int mxc_kpp_getc(void)
{	
	int col, row;	
	int i,key_cnt = 0;
	unsigned short reg_val;
	short scancode = 0;	
	unsigned char h = 1;	
	unsigned int new_key = 0;
	memset(k_KeyV,0,17);	
	
	reg_val = __raw_readw(KPSR);
	if (!(reg_val & KBD_STAT_KPKD))
	{
		return 0;
	}

	key_cnt = mxc_kpp_scan_matrix();	

	if(key_cnt == 0 )
	{
		KPP_PRINTF("key release\n");
		for (i = 0; i < kpp_dev.kpp_rows; i++) 
		{
			memset(press_scancode[i], -1,
		    	   sizeof(press_scancode[0][0]) * kpp_dev.kpp_cols);
			memset(release_scancode[i], -1,
		    	   sizeof(release_scancode[0][0]) * kpp_dev.kpp_cols);
		}
		memset(prev_rcmap, 0, kpp_dev.kpp_rows * sizeof(prev_rcmap[0]));
		memset(cur_rcmap, 0, kpp_dev.kpp_rows * sizeof(cur_rcmap[0]));	
		long_key_count = 0;
		key_tremble_times = 0;
		mxc_long_key_flag = 0;
		special_key = 0;
		return 0;
	}
	else 
	{
		if(1 == mxc_long_key_flag)
		{
			mxc_long_key_flag = 0;
			if(KEY_PPOWER == special_key)
			{
				KPP_PRINTF("long key %d\n",KEY_LONG_POWER);
				return KEY_LONG_POWER;
			}
			else if(KEY_MENU == special_key)
			{
				KPP_PRINTF("long key %d\n",KEY_GOTO_MENU);
				return KEY_GOTO_MENU;
			}
		}

		for (row = 0; row < kpp_dev.kpp_rows; row++)
		{
			for (col = 0; col < kpp_dev.kpp_cols; col++) 
			{
				if ((press_scancode[row][col] != -1)) 
				{
					/* Still Down, so add scancode */
					scancode = press_scancode[row][col];
				
					k_KeyV[0]++;	
					k_KeyV[h ++] = scancode + 1;

					KPP_PRINTF("KStateFirstDown: scan=%d", scancode);
					press_scancode[row][col] = -1;				
			
				}
			}
		}
	
		if(k_KeyV[0]>0)
		{			
			if(key_tremble_times == 0)
			{
				memcpy(prev_rcmap,0,kpp_dev.kpp_rows * sizeof(prev_rcmap[0]));
				memset(cur_rcmap, 0, kpp_dev.kpp_rows * sizeof(cur_rcmap[0]));	
				key_tremble_times ++;
			}
			else if(key_tremble_times == 1)//10ms
			{
				KPP_PRINTF("count:%d",k_KeyV[0]);
				if(k_KeyV[0] == 4)
				{
					if((k_KeyV[4] == 4) && (k_KeyV[3] == 3) && (k_KeyV[2] == 2) && (k_KeyV[1] == 1))
					{
						rtkey = KEY_PPOWER;
						special_key = KEY_PPOWER;
					}
				}
				else if(k_KeyV[0] == 12)
				{		
					if((k_KeyV[12] == 16) && (k_KeyV[11] == 15) && (k_KeyV[10] == 14) && (k_KeyV[9] == 13) &&\
						(k_KeyV[8] == 8) && (k_KeyV[7] == 7) && (k_KeyV[6] == 6) && (k_KeyV[5] == 5) &&\
						(k_KeyV[4] == 4) && (k_KeyV[3] == 3) && (k_KeyV[2] == 2) && (k_KeyV[1] == 1))	
					{
						rtkey = KEY_MENU;
						special_key = KEY_MENU;
					}
				
				}
				else
				{
					new_key = (k_KeyV[0] << 24)|(k_KeyV[3] << 16)|(k_KeyV[2] << 8)|(k_KeyV[1]);
					rtkey = new_key;
				}
			
#ifdef KPP_DEBUG
				KPP_PRINTF("Key Return:%d->",k_KeyV[0]);
				for(i = 1; i <= k_KeyV[0]; i++)
				{
					KPP_PRINTF("%x ",k_KeyV[i]);
				}
				KPP_PRINTF("\n");
#endif
				key_tremble_times = 0;
			}
		}
		return key_cnt;
	}
}
/*!
 * This function is called to free the allocated memory for local arrays
 */
static void mxc_kpp_free_allocated(void)
{
	int i;

	if (press_scancode) {
		for (i = 0; i < kpp_dev.kpp_rows; i++) {
			if (press_scancode[i])
				free(press_scancode[i]);
		}
		free(press_scancode);
	}

	if (release_scancode) {
		for (i = 0; i < kpp_dev.kpp_rows; i++) {
			if (release_scancode[i])
				free(release_scancode[i]);
		}
		free(release_scancode);
	}

	if (cur_rcmap)
		free(cur_rcmap);

	if (prev_rcmap)
		free(prev_rcmap);
}


unsigned int mxc_getkey(void)
{
		u32 key = 0;
		u32 keynum;
		u32 key1;

		key = rtkey;
		printf("rtkey = %x\n",rtkey);
		if((special_key == KEY_LONG_POWER) || (special_key == KEY_GOTO_MENU))
		{
			rtkey = 0;  
			return special_key;
		}
		if(key > 0)
		{
			rtkey = 0;
			keynum = key >> 24;
			key1 = key & 0xff;       
			if (keynum == 1)
			{
				return key1;
			}else{
				return 0;
			}
		}
		else{
			return 0;
		}
}


/*!
 * This function is called during the driver binding process.
 *
 * @param   pdev  the device structure used to store device specific
 *                information that is used by the suspend, resume and remove
 *                functions.
 *
 * @return  The function returns 0 on successful registration. Otherwise returns
 *          specific error code.
 */
int mxc_kpp_init(void)
{
	int i;
	int retval;
	unsigned int reg_val;
	KPP_PRINTF("####mxc_kpp_init init\n");
	kpp_dev.kpp_cols = CONFIG_MXC_KPD_COLMAX;
	kpp_dev.kpp_rows = CONFIG_MXC_KPD_ROWMAX;

	/* clock and IOMUX configuration for keypad */
	setup_mxc_kpd();

	/* Enable number of rows in keypad (KPCR[7:0])
	 * Configure keypad columns as open-drain (KPCR[15:8])
	 *
	 * Configure the rows/cols in KPP
	 * LSB nibble in KPP is for 8 rows
	 * MSB nibble in KPP is for 8 cols
	 */
	reg_val = __raw_readw(KPCR);
	reg_val |= (1  << kpp_dev.kpp_rows) - 1;	/* LSB row as key press detect*/
	reg_val |= ((1 << kpp_dev.kpp_cols) - 1) << 8;	/* MSB col as opern drain*/
	__raw_writew(reg_val, KPCR);

	/* Write 0's to KPDR[15:8] */
	reg_val = __raw_readw(KPDR);
	reg_val &= 0x00ff;
	__raw_writew(reg_val, KPDR);

	/* Configure columns as output,
	 * rows as input (KDDR[15:0]) */
	reg_val = __raw_readw(KDDR);
	reg_val |= 0xff00;
	reg_val &= 0xff00;
	__raw_writew(reg_val, KDDR);

	/* Clear the KPKD Status Flag
	 * and Synchronizer chain. */
	reg_val = __raw_readw(KPSR);
	reg_val &= ~(KBD_STAT_KPKR | KBD_STAT_KPKD);
	reg_val |= KBD_STAT_KPKD;
	reg_val |= KBD_STAT_KRSS | KBD_STAT_KDSC;
	__raw_writew(reg_val, KPSR);
	
	/* Set the KDIE control bit, and clear the KRIE
	 * control bit (avoid false release events). */
	reg_val |= KBD_STAT_KDIE;
	reg_val &= ~KBD_STAT_KRIE;
	__raw_writew(reg_val, KPSR);

	mxckpd_keycodes_size = kpp_dev.kpp_cols * kpp_dev.kpp_rows;

	if (mxckpd_keycodes_size == 0)
	{
		retval = -ENODEV;
		goto err;
	}

	/* allocate required memory */
	press_scancode   = (short **)malloc(kpp_dev.kpp_rows * sizeof(press_scancode[0]));
	release_scancode = (short **)malloc(kpp_dev.kpp_rows * sizeof(release_scancode[0]));

	if (!press_scancode || !release_scancode) {
		retval = -ENOMEM;
		goto err;
	}

	for (i = 0; i < kpp_dev.kpp_rows; i++) {
		press_scancode[i] = (short *)malloc(kpp_dev.kpp_cols
					    * sizeof(press_scancode[0][0]));
		release_scancode[i] =
		    (short *)malloc(kpp_dev.kpp_cols * sizeof(release_scancode[0][0]));

		if (!press_scancode[i] || !release_scancode[i]) {
			retval = -ENOMEM;
			goto err;
		}
	}

	cur_rcmap =
	    (unsigned short *)malloc(kpp_dev.kpp_rows * sizeof(cur_rcmap[0]));
	prev_rcmap =
	    (unsigned short *)malloc(kpp_dev.kpp_rows * sizeof(prev_rcmap[0]));

	if (!cur_rcmap || !prev_rcmap) {
		retval = -ENOMEM;
		goto err;
	}

	for (i = 0; i < kpp_dev.kpp_rows; i++) {
		memset(press_scancode[i], -1,
		       sizeof(press_scancode[0][0]) * kpp_dev.kpp_cols);
		memset(release_scancode[i], -1,
		       sizeof(release_scancode[0][0]) * kpp_dev.kpp_cols);
	}
	memset(cur_rcmap, 0, kpp_dev.kpp_rows * sizeof(cur_rcmap[0]));
	memset(prev_rcmap, 0, kpp_dev.kpp_rows * sizeof(prev_rcmap[0]));

	return 0;

err:
	mxc_kpp_free_allocated();
	return retval;
}

void keypad_clear(void)
{
	rtkey = 0;
}


