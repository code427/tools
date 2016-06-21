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

/*blue led reg param*/
#define BLUE_MUX_REG		0x43fac03c
#define BLUE_IO_GIDR		0x53fd0004
#define BLUE_IO_DR			0x53fd0000

/*red led reg param*/
#define RED_MUX_REG			0x43fac008
#define RED_IO_GIDR			0x53f9c004
#define RED_IO_DR			0x53f9c000

/*yellow led reg param*/
#define YELLOW_MUX_REG		0x43fac064
#define YELLOW_IO_GIDR		0x53fa4004
#define YELLOW_IO_DR		0x53fa4000

/*green led reg param*/
#define GREEN_MUX_REG		0x43fac02c
#define GREEN_IO_GIDR		0x53fd0004
#define GREEN_IO_DR			0x53fd0000

/*blue led cfg*/
#define BLUE_MSK			0x00000800
#define BLUE_ON				0x00000800
#define BLUE_OFF			0x00000000


.globl config_leds_output_mode
config_leds_output_mode:
	ldr r0,=BLUE_MUX_REG	/*set blue led pin mux mode*/
	ldr r1,=0x00000005
	str	r1,[r0]

	ldr r0,=GREEN_MUX_REG	/*set green led pin mux mode*/
	ldr r1,=0x00000005
	str	r1,[r0]

	ldr r0,=BLUE_IO_GIDR	/*set blue and green led ouput mode*/
	ldr r1,=0x00000880
	str	r1,[r0]

	ldr r0,=BLUE_IO_DR		/*blue and green led off*/
	ldr r1,=0x0
	str	r1,[r0]
	
	ldr r0,=YELLOW_MUX_REG	/*set yellow led pin mux mode*/
	ldr r1,=0x00000005
	str	r1,[r0]

	ldr r0,=YELLOW_IO_GIDR	/*set yellow led ouput mode*/
	ldr r1,=0x01000000
	str	r1,[r0]

	ldr r0,=YELLOW_IO_DR	/*yellow led off*/
	ldr r1,=0x0
	str	r1,[r0]
	
	ldr r0,=RED_MUX_REG		/*set red led pin mux mode*/
	ldr r1,=0x00000005
	str	r1,[r0]

	ldr r0,=RED_IO_GIDR		/*set red led ouput mode*/
	ldr r1,=0x1
	str	r1,[r0]

	ldr r0,=RED_IO_DR		/*red led off*/
	ldr r1,=0x1
	str	r1,[r0]
	
	mov pc, lr

.globl led_all_off
led_all_off:
	ldr r0,=BLUE_IO_DR		/*blue and green led off*/
	ldr r1,=0x0
	str	r1,[r0]

	ldr r0,=YELLOW_IO_DR	/*yellow led off*/
	ldr r1,=0x0
	str	r1,[r0]
	
	ldr r0,=RED_IO_DR		/*red led off*/
	ldr r1,=0x1
	str	r1,[r0]
	mov pc, lr

.globl red_on
red_on:
	ldr r0,=RED_IO_DR		/*red led on*/
	ldr r1,=0x0
	str	r1,[r0]
	mov pc, lr

.globl blue_on
blue_on:
	ldr r0,=BLUE_IO_DR
	ldr r1,[r0]
	orr r1,r1,#0x800
	str	r1,[r0]
	mov pc, lr

.globl yellow_on
yellow_on:
	ldr r0,=YELLOW_IO_DR	/*yellow led on*/
	ldr r1,=0x01000000
	str	r1,[r0]
	mov pc, lr

.globl green_on
green_on:
	ldr r0,=GREEN_IO_DR
	ldr	r1,[r0]
	orr r1,r1,#0x80
	str r1,[r0]
	mov pc, lr

.globl red_off
red_off:
	ldr r0,=RED_IO_DR		/*red led off*/
	ldr r1,=0x1
	str	r1,[r0]
	mov pc, lr

.globl blue_off
blue_off:
	ldr r0,=BLUE_IO_DR
	ldr r1,[r0]
	bic r1,r1,#0x800
	str r1,[r0]
	mov pc, lr

.globl yellow_off
yellow_off:
	ldr r0,=YELLOW_IO_DR	/*yellow led off*/
	ldr r1,=0x0
	str	r1,[r0]
	mov pc, lr

.globl green_off
green_off:
	ldr r0,=GREEN_IO_DR
	ldr r1,[r0]
	bic r1,r1,#0x80
	str r1,[r0]
	mov pc, lr

.globl extern_setting
extern_setting:

	mov	pc, lr


