/*
 * (C) Copyright 2000-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
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

#include <env_callback.h>

#ifdef DEFAULT_ENV_INSTANCE_EMBEDDED
env_t environment __PPCENV__ = {
	ENV_CRC,	/* CRC Sum */

#ifdef CONFIG_SYS_REDUNDAND_ENVIRONMENT
	1,		/* Flags: valid */
#endif
	{
#elif defined(DEFAULT_ENV_INSTANCE_STATIC)
static char default_environment[] = {
#else

const uchar default_environment[] = {
#endif

#ifdef	CONFIG_ENV_CALLBACK_LIST_DEFAULT
	ENV_CALLBACK_VAR "=" CONFIG_ENV_CALLBACK_LIST_DEFAULT "\0"
#endif
#ifdef	CONFIG_ENV_FLAGS_LIST_DEFAULT
	ENV_FLAGS_VAR "=" CONFIG_ENV_FLAGS_LIST_DEFAULT "\0"
#endif
#ifdef	CONFIG_BOOTARGS
	"bootargs="	CONFIG_BOOTARGS			"\0"
#endif
#ifdef	CONFIG_RAMBOOTCOMMAND
	"ramboot="	CONFIG_RAMBOOTCOMMAND		"\0"
#endif
#ifdef	CONFIG_NFSBOOTCOMMAND
	"nfsboot="	CONFIG_NFSBOOTCOMMAND		"\0"
#endif
/**********************************************************************/
#ifdef	CONFIG_BOOTCOMMAND
	"bootcmd="	CONFIG_BOOTCOMMAND		"\0"
#endif
#if defined(CONFIG_BOOTDELAY) && (CONFIG_BOOTDELAY >= 0)
	"bootdelay="	__stringify(CONFIG_BOOTDELAY)	"\0"
#endif
#if defined(CONFIG_BAUDRATE) && (CONFIG_BAUDRATE >= 0)
	"baudrate="	__stringify(CONFIG_BAUDRATE)	"\0"
#endif
#ifdef	CONFIG_IPADDR
	"ipaddr="	__stringify(CONFIG_IP_ADDR)	"\0"
#endif
#ifdef	CONFIG_SERVERIP
	"serverip="	__stringify(CONFIG_SERVERIP_ADDR)	"\0"
#endif
#ifdef	CONFIG_LOADADDR
	"loadaddr="	__stringify(CONFIG_LOADADDR)	"\0"
#endif
#ifdef	CONFIG_NETMASK
	"netmask="	__stringify(CONFIG_NETMASK_ADDR)	"\0"
#endif
#ifdef	CONFIG_ETHADDR
	"ethaddr="	__stringify(CONFIG_MAC_ADDR)	"\0"
#endif
/**********************************************************************/
#ifdef	CONFIG_LOADS_ECHO
	"loads_echo="	__stringify(CONFIG_LOADS_ECHO)	"\0"
#endif
#ifdef	CONFIG_ETH1ADDR
	"eth1addr="	__stringify(CONFIG_ETH1ADDR)	"\0"
#endif
#ifdef	CONFIG_ETH2ADDR
	"eth2addr="	__stringify(CONFIG_ETH2ADDR)	"\0"
#endif
#ifdef	CONFIG_ETH3ADDR
	"eth3addr="	__stringify(CONFIG_ETH3ADDR)	"\0"
#endif
#ifdef	CONFIG_ETH4ADDR
	"eth4addr="	__stringify(CONFIG_ETH4ADDR)	"\0"
#endif
#ifdef	CONFIG_ETH5ADDR
	"eth5addr="	__stringify(CONFIG_ETH5ADDR)	"\0"
#endif
#ifdef	CONFIG_ETHPRIME
	"ethprime="	CONFIG_ETHPRIME			"\0"
#endif
#ifdef	CONFIG_SYS_AUTOLOAD
	"autoload="	CONFIG_SYS_AUTOLOAD		"\0"
#endif
#ifdef	CONFIG_PREBOOT
	"preboot="	CONFIG_PREBOOT			"\0"
#endif
#ifdef	CONFIG_ROOTPATH
	"rootpath="	CONFIG_ROOTPATH			"\0"
#endif
#ifdef	CONFIG_GATEWAYIP
	"gatewayip="	__stringify(CONFIG_GATEWAYIP)	"\0"
#endif
#ifdef	CONFIG_HOSTNAME
	"hostname="	__stringify(CONFIG_HOSTNAME)	"\0"
#endif
#ifdef	CONFIG_BOOTFILE
	"bootfile="	CONFIG_BOOTFILE			"\0"
#endif
#ifdef	CONFIG_CLOCKS_IN_MHZ
	"clocks_in_mhz=1\0"
#endif
#if defined(CONFIG_PCI_BOOTDELAY) && (CONFIG_PCI_BOOTDELAY > 0)
	"pcidelay="	__stringify(CONFIG_PCI_BOOTDELAY)"\0"
#endif
#ifdef	CONFIG_ENV_VARS_UBOOT_CONFIG
	"arch="		CONFIG_SYS_ARCH			"\0"
	"cpu="		CONFIG_SYS_CPU			"\0"
	"board="	CONFIG_SYS_BOARD		"\0"
	"board_name="	CONFIG_SYS_BOARD		"\0"
#ifdef CONFIG_SYS_VENDOR
	"vendor="	CONFIG_SYS_VENDOR		"\0"
#endif
#ifdef CONFIG_SYS_SOC
	"soc="		CONFIG_SYS_SOC			"\0"
#endif
#endif
#ifdef	CONFIG_EXTRA_ENV_SETTINGS
	CONFIG_EXTRA_ENV_SETTINGS
#endif
	"\0"
#ifdef DEFAULT_ENV_INSTANCE_EMBEDDED
	}
#endif
};
