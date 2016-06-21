/*
 *  Copyright (c) 2009  Freescale Semiconductor
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


#include <config.h>
#include <version.h>

.extern reset
.extern hab_super_root_key
.extern _end

#define DCDGEN(i,type, addr, data) \
dcd_##i:			;\
    .long type			;\
    .long addr			;\
    .long data

.globl _initheader
_initheader:
.long reset
.long 0xB1
.long CONFIG_SPL_TEXT_BASE
.long hwcfg_ptr
.long hab_super_root_key
.long dcd_data
.long CONFIG_SPL_TEXT_BASE


	.org 0x400
app_code_jump_v:	.long reset
app_code_barker:	.long 0xB1
app_code_csf:		.long CONFIG_SPL_TEXT_BASE
hwcfg_ptr_ptr:		.long hwcfg_ptr
super_root_key:		.long hab_super_root_key
hwcfg_ptr:			.long dcd_data
app_dest_ptr:		.long CONFIG_SPL_TEXT_BASE
dcd_data:			.long 0xB17219E9
dcd_len:			.long 12*20

/* WEIM config-CS5 init -- CPLD */
DCDGEN( 0, 4, 0x53F80000, 0x000b2c01)
DCDGEN( 1, 4, 0x53F80008, 0x30034000) /* CLKCTL ARM=400 AHB=100 */
DCDGEN( 2, 4, 0x53F80020, 0x01010101) /* nfc 50MHz*/
DCDGEN( 3, 4, 0xB8002050, 0x0000D843) /* CS5_CSCRU */
DCDGEN( 4, 4, 0xB8002054, 0x22252521) /* CS5_CSCRL */
DCDGEN( 5, 4, 0xB8002058, 0x22220A00) /* CS5_CSCRA */
DCDGEN( 6, 4, 0xB8001010, 0x00000004) /* enable mDDR */
DCDGEN( 7, 4, 0xB8001000, 0x93210000) /* precharge command */
DCDGEN( 8, 1, 0x80000400, 0x12344321) /* precharge all dummy write */
DCDGEN( 9, 4, 0xB8001000, 0xA3210000) /* auto-refresh command */
DCDGEN(10, 4, 0x80000000, 0x12344321) /* dummy write for refresh */
DCDGEN(11, 4, 0x80000000, 0x12344321) /* dummy write for refresh */
DCDGEN(12, 4, 0xB8001000, 0xB3210000) /* Load Mode Reg command - cas=3 bl=8 */
DCDGEN(13, 1, 0x80000023, 0xda)	/* dummy write -- address has the mode bits */
DCDGEN(14, 1, 0x84000000, 0xff) /* dummy write -- address has the mode bits */
DCDGEN(15, 4, 0xB8001000, 0x83216880)   /*auto power counter 3<<10 */
DCDGEN(16, 4, 0x80000000, 0x12344321) /* dummy write for refresh */
DCDGEN(17, 4, 0x80000000, 0x12344321) /* dummy write for refresh */
DCDGEN(18, 4, 0xB8001004, 0x0029562F)	/*0x0029523F)*/
DCDGEN(19, 4, 0x80000000, 0x12344321) /* dummy write for refresh */

spl_size_cfg:	.long (SPL_IMAGE_SIZE)

