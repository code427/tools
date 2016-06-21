/*
 * (C) Copyright 2009
 * Magnus Lilja <lilja.magnus@gmail.com>
 *
 * (C) Copyright 2008
 * Maxim Artamonov, <scn1874 at yandex.ru>
 *
 * (C) Copyright 2006-2008
 * Stefan Roese, DENX Software Engineering, sr at denx.de.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <nand.h>
#include <serial.h>
#include <asm/arch/imx-regs.h>
#include <asm/io.h>
#include <fsl_nfc.h>

#undef NAND_READ_ID
#undef NAND_CHECK_ECC
#define NAND_CHECK_BAD_BLOCK

#define NAND_PAGE_MASK		0x7ff
#define NAND_PAGE_SHIFT 	11

static struct fsl_nfc_regs *const nfc = (void *)NFC_BASE_ADDR;

static void nfc_wait_ready(void)
{
	uint32_t tmp;

	while (!(readw(&nfc->config2) & NFC_INT))
		;

	/* Reset interrupt flag */
	tmp = readw(&nfc->config2);
	tmp &= ~NFC_INT;
	writew(tmp, &nfc->config2);
}


static void nfc_nand_init(void)
{
	int config1;

	/* Blocks to be unlocked */
	writew(0x0,&nfc->unlockstart_blkaddr);
	writew(0xffff,&nfc->unlockend_blkaddr);

	/* Unlock Block Command for given address range */
	writew(NFC_WPC_UNLOCK, &nfc->wrprot);
	
	writew(CONFIG_SYS_NAND_SPARE_SIZE / 2, &nfc->spare_area_size); //elinux tag: 64

	/* unlocking RAM Buff */
	writew(0x2, &nfc->config);

	/* disable spare enable */
	/* hardware ECC checking and correct */
	config1 = ((readw(&nfc->config1) & ~NFC_ECC_EN) & ~NFC_SP_EN) | NFC_INT_MSK;

	writew(config1, &nfc->config1);

	/* elinux set 2k page size */
	NFMS |= 0x00000100;
	NFMS &= ~0x00000200;
}

static void nfc_nand_command(unsigned short command)
{
	writew(command, &nfc->flash_cmd);
	writew(NFC_CMD, &nfc->config2);
	nfc_wait_ready();
}

static void nfc_nand_address(unsigned short address)
{
	writew(address, &nfc->flash_addr);
	writew(NFC_ADDR, &nfc->config2);
	nfc_wait_ready();
}

static void nfc_nand_page_address(unsigned int col,unsigned int page)
{
	nfc_nand_address((col) & 0xff);
	nfc_nand_address((col >> 8 ) & 0xff);	//colomn addr
	nfc_nand_address((page) & 0xff);
	nfc_nand_address((page >> 8) & 0xff); //row addr
	nfc_nand_address((page >> 16) & 0x01); //row addr
}

static void nfc_nand_data_output(void)
{
	writew(0, &nfc->buf_addr);
	writew(NFC_OUTPUT, &nfc->config2);
	nfc_wait_ready();
}

#ifdef NAND_CHECK_ECC
static int nfc_nand_check_ecc(void)
{
	u32 ecc_status = readl(&nfc->ecc_status_result);
	int ecc_per_page = CONFIG_SYS_NAND_PAGE_SIZE / 512;
	int err_limit = CONFIG_SYS_NAND_SPARE_SIZE / ecc_per_page > 16 ? 8 : 4;
	int subpages = CONFIG_SYS_NAND_PAGE_SIZE / 512;

	do {
		if ((ecc_status & 0xf) > err_limit)
			return 1;
		ecc_status >>= 4;
	} while (--subpages);

	return 0;
}
#endif


static int nfc_read_small_page(unsigned int address, unsigned char *buf)
{
	int i;
	u32 *src;
	u32 *dst;
	unsigned int col,page;
	
	col = address & NAND_PAGE_MASK;
	page = (address >> NAND_PAGE_SHIFT) & 0x1ffff;

	writew(0, &nfc->buf_addr); /* read in first 0 buffer */
	nfc_nand_command(NAND_CMD_READ0);
	nfc_nand_page_address(col,page);
	nfc_nand_command(NAND_CMD_READSTART);
	nfc_nand_data_output(); /* fill the main buffer 0 */

	src = (u32 *)&nfc->main_area[0][0];
	dst = (u32 *)buf;

	// main copy loop from NAND-buffer to SDRAM memory //
	for (i = 0; i < CONFIG_SYS_NAND_SMALL_PAGE / 4; i++) {
		writel(readl(src), dst);
		src++;
		dst++;
	}
	return 0;
}

static int nfc_read_page(unsigned int page, unsigned char *buf)
{
	int i;
	unsigned int real_addr = page << NAND_PAGE_SHIFT; 

	for(i = 0; i < CONFIG_SYS_NAND_PAGE_SIZE / CONFIG_SYS_NAND_SMALL_PAGE; i++)
	{
		nfc_read_small_page(real_addr,buf);
		buf += CONFIG_SYS_NAND_SMALL_PAGE;
		real_addr += (CONFIG_SYS_NAND_SMALL_PAGE + CONFIG_SYS_NAND_SPARE_SIZE / 4);
	}
	
	return 0;
}

#ifdef NAND_CHECK_BAD_BLOCK
static int is_badblock(int pagenumber)
{
	int page = pagenumber;
	u32 badblock;
	u32 *src;

	for (page = pagenumber; page < pagenumber + 2; page++) 
	{
		writew(0, &nfc->buf_addr); /* read in first 0 buffer */
		nfc_nand_command(NAND_CMD_READ0);
		nfc_nand_page_address(0,page);
		nfc_nand_command(NAND_CMD_READSTART);
		nfc_nand_data_output(); /* fill the main buffer 0 */

		src = (u32 *)&nfc->spare_area[0][0];

		/* Get the bad block marker */
		badblock = readl(&src[CONFIG_SYS_NAND_BAD_BLOCK_POS]);
		badblock &= 0xff;

		/* bad block marker verify */
		if (badblock != 0xff)
			return 1; /* potential bad block */
	}

	return 0;

}
#endif

static int nand_load(unsigned int from, unsigned int size, unsigned char *buf)
{
	int i;
	unsigned int page;
	unsigned int maxpages = CONFIG_SYS_NAND_SIZE / CONFIG_SYS_NAND_PAGE_SIZE;
	
	nfc_nand_init();

	/* Convert to page number, start at page 2, the spl in page0 and page1 */
	page = from / CONFIG_SYS_NAND_PAGE_SIZE;
	i = 0;

	while (i < (size / CONFIG_SYS_NAND_PAGE_SIZE)) 
	{
		if (nfc_read_page(page, buf) < 0)
		{
			return -1;
		}

		page++;
		i++;
		buf = buf + CONFIG_SYS_NAND_PAGE_SIZE;

		/*
		 * Check if we have crossed a block boundary, and if so
		 * check for bad block.
		 */
		if (!(page % CONFIG_SYS_NAND_PAGE_COUNT)) 
		{
			/*
			 * Yes, new block. See if this block is good. If not,
			 * loop until we find a good block.
			 */
			while (is_badblock(page)) 
			{
				page = page + CONFIG_SYS_NAND_PAGE_COUNT;
				/* Check i we've reached the end of flash. */
				if (page >= maxpages)
					return -1;
			}
		}
	}
	return 0;
}

#if defined(CONFIG_ARM)
extern int spl_serial_init(void);
void board_init_f(ulong bootflag)
{
}
#endif

/*
 * The main entry for NAND booting. It's necessary that SDRAM is already
 * configured and available since this code loads the main U-Boot image
 * from NAND into SDRAM and starts it from there.
 */
//extern int mxc_serial_getc(void);
extern void serial_puts(const char *s);

void nand_boot(void)  /*nand_boot_imx*/
{
	__attribute__((noreturn)) void (*uboot)(void);

	/*
	 * CONFIG_SYS_NAND_U_BOOT_OFFS and CONFIG_SYS_NAND_U_BOOT_SIZE must
	 * be aligned to full pages
	 */

	//load_logo data
	//nand_load(CONFIG_SYS_LOGO_OFFS, CONFIG_SYS_LOGO_SIZE,(unsigned char *)CONFIG_SYS_LOGO_DST);
		
	if (!nand_load(CONFIG_SYS_NAND_U_BOOT_OFFS, CONFIG_SYS_NAND_U_BOOT_SIZE,(unsigned char *)CONFIG_SYS_NAND_U_BOOT_DST)) 
	{
		/* Copy from NAND successful, start U-boot */
		uboot = (void *)CONFIG_SYS_NAND_U_BOOT_START;
		uboot();
	} else {
		/* Unrecoverable error when copying from NAND */
		hang();
	}
}

/*
 * Called in case of an exception.
 */
void hang(void)
{
	/* Loop forever */
	while (1) ;
}
