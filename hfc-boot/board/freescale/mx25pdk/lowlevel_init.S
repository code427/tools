/*
 * Copyright (c) 2011  Freescale Semiconductor
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
 */

#include <asm/arch-mx25/imx-regs.h>

.globl lowlevel_init
lowlevel_init:
	/* MAX (Multi-Layer AHB Crossbar Switch) setup */
	/* MAX - priority for MX25 is (SDHC2/SDMA)>USBOTG>RTIC>IAHB>DAHB */
	ldr r0, =IMX_MAX_BASE
	ldr r1, =0x00002143
	str r1, [r0, #0x000]        /* for S0 */
	str r1, [r0, #0x100]        /* for S1 */
	str r1, [r0, #0x200]        /* for S2 */
	str r1, [r0, #0x300]        /* for S3 */
	str r1, [r0, #0x400]        /* for S4 */
	/* SGPCR - always park on last master */
	ldr r1, =0x10
	str r1, [r0, #0x010]        /* for S0 */
	str r1, [r0, #0x110]        /* for S1 */
	str r1, [r0, #0x210]        /* for S2 */
	str r1, [r0, #0x310]        /* for S3 */
	str r1, [r0, #0x410]        /* for S4 */
	/* MGPCR - restore default values */
	ldr r1, =0x0
	str r1, [r0, #0x800]        /* for M0 */
	str r1, [r0, #0x900]        /* for M1 */
	str r1, [r0, #0xA00]        /* for M2 */
	str r1, [r0, #0xB00]        /* for M3 */
	str r1, [r0, #0xC00]        /* for M4 */

	/* M3IF setup */
	ldr r1, =IMX_M3IF_CTRL_BASE
	ldr r0, =0x00000001
	str r0, [r1]  /* M3IF control reg */

	/* default CLKO to 1/32 of the ARM core */
	ldr r0, =CCM_MCR
	ldr r1, =CCM_MCR
	bic r1, r1, #0x00F00000
	bic r1, r1, #0x7F000000
	mov r2,     #0x5F000000
	add r2, r2, #0x00200000
	orr r1, r1, r2
	str r1, [r0]

	/* enable all the clocks */
	ldr r2, =0x1FFFFFFF
	ldr r0, =CCM_CGR0
	str r2, [r0]
	ldr r2, =0xFFFFFFFF
	ldr r0, =CCM_CGR1
	str r2, [r0]
	ldr r2, =0x000FDFFF
	ldr r0, =CCM_CGR2
	str r2, [r0]
	mov	pc, lr
