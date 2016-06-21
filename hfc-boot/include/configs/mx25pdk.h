/*
 * (C) Copyright 2011 Freescale Semiconductor, Inc.
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
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/imx-regs.h>


//#define DEBUG 1

//#ifdef DEBUG
#define UBOOT_CMD_DEBUG
//#endif

#define CONFIG_SPL_TEXT_BASE		0x81f00000
#define SPL_IMAGE_SIZE			0x20000

/* High Level Configuration Options */

#define CONFIG_MX25
#define CONFIG_SYS_HZ			1000
#define CONFIG_SYS_TEXT_BASE		0x83F00000
#define UBOOT_IMAGE_SIZE		(512 * 1024) /* Size of RAM U-Boot image 256K*/

//System Vector
#define CONFIG_VECTOR_BASE		0x7801FFC0

#define CONFIG_MXC_GPIO
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#define CONFIG_MACH_TYPE		MACH_TYPE_MX25_3DS
/*********************Cramfs Setting***********************/
#define CONFIG_CRAMFS_MEM_ADDR		0x81000000

/*********************FDT Setting***********************/
#define CONFIG_OF_LIBFDT		/* Device Tree support */
#define CONFIG_FDT_MEM_ADDR		0x81F10000
#define CONFIG_FDT_NAND_ADDR		0x10000			//Flash addr
#define CONFIG_FDT_NAND_SIZE		0x8000			//64KB
/*********************IRQ********************************/
#define CONFIG_USE_IRQ			1

#ifdef 	CONFIG_USE_IRQ
#define IRQ_DESC_NAME_LEN		24
#define MAX_GPIO_PIN_NUM		128
#define MXC_INTERNAL_IRQS		64
#define CONFIG_STACKSIZE_IRQ		(8*1024)
#define CONFIG_STACKSIZE_FIQ		(4*1024)
#endif
/*******************************************************/

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 2 * 1024 * 1024)

/* Physical Memory Map */
#define CONFIG_NR_DRAM_BANKS		1
#define PHYS_SDRAM_1			0x80000000
#define PHYS_SDRAM_1_SIZE		(128 * 1024 * 1024)

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
#define CONFIG_SYS_INIT_RAM_ADDR	IMX_RAM_BASE
#define CONFIG_SYS_INIT_RAM_SIZE	IMX_RAM_SIZE
#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* Memory Test */
#define CONFIG_SYS_MEMTEST_START	(PHYS_SDRAM_1 + PHYS_SDRAM_1_SIZE/2)
#define CONFIG_SYS_MEMTEST_END		(PHYS_SDRAM_1 + PHYS_SDRAM_1_SIZE)

/* Serial Info */
#define CONFIG_MXC_UART
#define CONFIG_MXC_UART_BASE		UART1_BASE
#define CONFIG_CONS_INDEX		1		/* use UART0 for console */
#define CONFIG_BAUDRATE			115200		/* Default baud rate */

/* Nand Flash driver*/
#define	CONFIG_SYS_NAND_MAX_CHIP	1		/**/
#define CONFIG_NAND_MXC			1
#define CONFIG_SYS_MAX_NAND_DEVICE	CONFIG_SYS_NAND_MAX_CHIP
#define CONFIG_MXC_NAND_REGS_BASE	(NFC_BASE_ADDR)
#define CONFIG_SYS_NAND_BASE		(NFC_BASE_ADDR)

#define CONFIG_NAND_U_BOOT
#define CONFIG_SYS_NAND_U_BOOT_DST  	CONFIG_SYS_TEXT_BASE		/* NUB load-addr */
#define CONFIG_SYS_NAND_U_BOOT_START	CONFIG_SYS_NAND_U_BOOT_DST	/* NUB start-addr */
#define CONFIG_SYS_NAND_U_BOOT_OFFS	(128 * 1024)   		/* Offset to RAM U-Boot image */
#define CONFIG_SYS_NAND_U_BOOT_SIZE	UBOOT_IMAGE_SIZE	/* Size of RAM U-Boot image 1.5 MB*/

/* Nand Flash Param */
//#define CONFIG_MTD_DEBUG				
//#define CONFIG_MTD_DEBUG_VERBOSE		2
#define CONFIG_MXC_NAND_HWECC
#define CONFIG_SYS_NAND_LARGEPAGE
#define CONFIG_SYS_NAND_USE_FLASH_BBT
#define CONFIG_SYS_NAND_PAGE_SIZE		2048			/* NAND chip page size */	
#define CONFIG_SYS_NAND_SMALL_PAGE		512				/* in page 512 byte */	
#define CONFIG_SYS_MAX_FLASH_SECT		1024			/* block on one chip */
#define CONFIG_SYS_MAX_FLASH_BANKS		1
#define CONFIG_SYS_NAND_BLOCK_SIZE		(2048 * 64)		/* NAND chip block size */
#define CONFIG_SYS_NAND_PAGE_COUNT 		64				/* NAND chip page per block count  */
#define CONFIG_SYS_NAND_BAD_BLOCK_POS		0				/* Location of the bad-block label */
#define CONFIG_SYS_NAND_SPARE_SIZE		64				/* btt size */
#define CONFIG_SYS_NAND_SIZE			(2048 * 64 * 1024)
#define CONFIG_NAND_RES_SIZE			(512 * 1024)


/* Nand flash environment organization */
#define CONFIG_ENV_SIZE				(128 * 1024)
#define CONFIG_ENV_OFFSET			CONFIG_SYS_NAND_U_BOOT_SIZE
#define CONFIG_SYS_NO_FLASH
#define CONFIG_ENV_IS_NOWHERE			1
#define CONFIG_ENV_OVERWRITE

/* U-Boot general configuration */
#define CONFIG_SYS_PROMPT		"MX25PDK U-Boot > "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size  */

/* Print buffer sz */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_MAXARGS		16	/* max number of command args */

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LONGHELP

/* Ethernet */
#define CONFIG_FEC_MXC
#define CONFIG_FEC_MXC_PHYADDR		0x1f
#define CONFIG_MII
#define CONFIG_CMD_NET
#define CONFIG_IPADDR
#define CONFIG_SERVERIP
#define CONFIG_NETMASK
#define	CONFIG_ETHADDR
#define	CONFIG_IP_ADDR				172.23.2.25
#define	CONFIG_SERVERIP_ADDR		172.23.2.23
#define	CONFIG_NETMASK_ADDR			255.255.0.0
#define	CONFIG_MAC_ADDR				00:43:12:ff:ea:e0
#define CONFIG_CMD_PING

/* Cache control */
#define CONFIG_CMD_CACHE

/*uart Debug key*/
#define BOOT_UART_DBG_KEY

#define CONFIG_BOOTDELAY	1

#define CONFIG_LOADADDR			0x80800000	/* loadaddr env var */
#define CONFIG_SYS_LOAD_ADDR	CONFIG_LOADADDR

#define xstr(s) str(s)
#define str(s)  #s

#define NAND_SIZE						0x10000000

#define HFC_256MB_NFMAP_BOOT_ADDR		0x000000
#define HFC_256MB_NFMAP_BOOT_SIZE		0x200000

#define HFC_256MB_NFMAP_BKNL_ADDR		0x200000
#define HFC_256MB_NFMAP_BKNL_SIZE		0x400000

#define HFC_256MB_NFMAP_KERNEL_ADDR		0x600000
#define HFC_256MB_NFMAP_KERNEL_SIZE		0x400000

#define HFC_256MB_NFMAP_CRAMFS_ADDR     0x0A00000
#define HFC_256MB_NFMAP_CRAMFS_SIZE     0x0400000

#define HFC_256MB_NFMAP_SYSDAT_ADDR     0x0E00000
#define HFC_256MB_NFMAP_SYSDAT_SIZE     0x1E00000
    
#define HFC_256MB_NFMAP_SECURE_ADDR     0x2C00000
#define HFC_256MB_NFMAP_SECURE_SIZE     0x0800000
    
#define HFC_256MB_NFMAP_USRFS_ADDR      0x3400000
#define HFC_256MB_NFMAP_USERFS_SIZE     0xC700000

#define HFC_256MB_NFMAP_BCKP_ADDR		0xFB00000
#define	HFC_256MB_NFMAP_BCKP_SIZE		0x400000

#define CONFIG_MTDPARTS_DEFAULT_256MB "mtdparts=mxc_nand:"                                 \
	xstr(HFC_256MB_NFMAP_BOOT_SIZE)"@"xstr(HFC_256MB_NFMAP_BOOT_ADDR)"(boot)," 			\
    xstr(HFC_256MB_NFMAP_BKNL_SIZE)"@"xstr(HFC_256MB_NFMAP_BKNL_ADDR)"(bknl),"  	\
    xstr(HFC_256MB_NFMAP_KERNEL_SIZE)"@"xstr(HFC_256MB_NFMAP_KERNEL_ADDR)"(kernel),"  	\
    xstr(HFC_256MB_NFMAP_SYSDAT_SIZE)"@"xstr(HFC_256MB_NFMAP_SYSDAT_ADDR)"(sysdat),"  	\
    xstr(HFC_256MB_NFMAP_SECURE_SIZE)"@"xstr(HFC_256MB_NFMAP_SECURE_ADDR)"(secure),"  	\
    xstr(HFC_256MB_NFMAP_USERFS_SIZE)"@"xstr(HFC_256MB_NFMAP_USRFS_ADDR)"(userfs),"		\
	xstr(HFC_256MB_NFMAP_BCKP_SIZE)"@"xstr(HFC_256MB_NFMAP_BCKP_ADDR)"(backup)" 

#define CONFIG_EXTRA_ENV_SETTINGS \
	"uimage=uImage\0" \
	"dbg_knl=setenv bootargs console=ttymxc0,${baudrate} ${boot_mtd}\0" \
	"netargs=setenv bootargs console=ttymxc0,${baudrate} " \
	"root=/dev/nfs " \
	"ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp\0" \
	"bootcmd=run dbg_knl; bootm\0" \
	"bmtd=setenv bootargs console=ttymxc0,115200 ${boot_mtd}; tftp uImage; bootm\0" 

/* U-Boot commands */
#include <config_cmd_default.h>
#define CONFIG_CMD_NAND 
#define CONFIG_CMD_HFC
#define CONFIG_HFC_TFT

#ifdef CONFIG_USBDOWNLOAD_GADGET 
#define CONFIG_SYS_CACHELINE_SIZE	4096
#define CONFIG_G_DNL_PRODUCT_NUM	0x00ad
#define CONFIG_G_DNL_VENDOR_NUM		0x0001
#define CONFIG_G_DNL_MANUFACTURER 	"HFC Company"
#define CONFIG_DFU_FUNCTION
#endif	/* CONFIG_USBDOWNLOAD_GADGET */


#ifdef CONFIG_HFC_TFT

#define TFT_FRAMEBUFFER_BG_BASE    	0x82600000
#define TFT_FRAMEBUFFER_GW_BASE    	0x82800000
#define TFT_FRAMEBUFFER_BG_SIZE    	0x26000

#endif /* CONFIG_HFC_TFT */

#define KEY_DIGITAL0		'0'
#define KEY_DIGITAL1 	 	'1'
#define KEY_DIGITAL2   		'2'
#define KEY_DIGITAL3   		'3'
#define KEY_DIGITAL4   		'4'
#define KEY_DIGITAL5   		'5'
#define KEY_DIGITAL6   		'6'
#define KEY_DIGITAL7   		'7'
#define KEY_DIGITAL8   		'8'
#define KEY_DIGITAL9   		'9'

#define KEY_FF1				0xc0
#define KEY_FF2				0xc1
#define KEY_FF3				0xc2
#define KEY_FUNC			0xc3
#define KEY_SIGN			0xc4
#define KEY_ALPHA			0xc6
#define KEY_BCKSPC			0xc7
#define KEY_MULTTASK    	0xc8
#define KEY_FEED			0xc9
#define KEY_UUP				0xca
#define KEY_DDOWN			0xcb
#define KEY_LLEFT   		0xcc
#define KEY_RRIGHT			0xcd
#define KEY_HANGUP			0xce
#define KEY_CALL			0xcf

#define KEY_CANCEL			0xe0
#define KEY_CLEAR			0xe1
#define KEY_EENTER 			0xe2
#define KEY_MENU			0xe3
#define KEY_PPOWER			0xe4
#define KEY_DBG				0xe5
#define KEY_GOTO_MENU		0xe6
#define KEY_GOTO_DBG		0xe7
#define KEY_LONG_POWER		0xe8

#endif /* __CONFIG_H */
