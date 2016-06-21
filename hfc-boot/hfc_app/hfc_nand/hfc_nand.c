/*
 * (C) Copyright 2000-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
#include <asm/arch/imx-regs.h>
#include <fsl_nfc.h>
#include <asm/io.h>
#include <image.h>

#define NAND_PAGE_MASK		0x7ff
#define NAND_PAGE_SHIFT 	11


#undef HFC_NAND_DEBUG

#ifdef HFC_NAND_DEBUG
#define hfc_nand_dbg(x...) printf(x)
#else
#define hfc_nand_dbg(x...)
#endif
/********************************************************************/
static struct fsl_nfc_regs *const nfc = (void *)NFC_BASE_ADDR;

void hfc_nand_init(void)
{
	//int ecc_per_page = CONFIG_SYS_NAND_PAGE_SIZE / 512;    //elinux tag: 2048
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


