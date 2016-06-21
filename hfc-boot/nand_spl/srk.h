/*
 * =====================================================================================
 *
 *       Filename:  imx_srk.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年08月18日 17时47分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhongjun (zj), zhongjun@live.com
 *        Company:  zhongjun
 *
 * =====================================================================================
 */
#ifndef __SRK_H__ 
#define __SRK_H__

#define MAX_EXP_SIZE 4

typedef struct
{
	unsigned int barker; /* Barker for sanity check */
    unsigned int length; /* Device configuration structure length (not including preamble) */
} DCD_PREAMBLE_T;

typedef struct
{
	unsigned int type; /* Type of pointer (byte, halfword, word, wait/read) */
    unsigned int *addr; /* Address to write to */
    unsigned int data; /* Data to write */
} DCD_TYPE_ADDR_DATA_T;

typedef struct
{
	DCD_PREAMBLE_T preamble; /* Preamble */
	/* Type / Address / data elements */
	DCD_TYPE_ADDR_DATA_T type_addr_data[60]; /*where count would be some hardcoded value
	less than 60*/
} DCD_T;

typedef struct
{
	unsigned char rsa_exponent[MAX_EXP_SIZE]; /* RSA public exponent */
	unsigned char *rsa_modulus; /* RSA modulus pointer */
	unsigned short exponent_size; /* Exponent size in bytes */
	unsigned short modulus_size; /* Modulus size in bytes*/
	unsigned char init_flag; /* Indicates if key initialized */
} hab_rsa_public_key;

typedef struct
{
	unsigned int *app_code_jump_vector;
	unsigned int app_code_barker;
	unsigned int *app_code_csf;
	DCD_T **dcd_ptr_ptr;
	hab_rsa_public_key *super_root_key;
	DCD_T *dcd_ptr;
	unsigned int *app_dest_ptr;
} FLASH_HDR_T;

extern const hab_rsa_public_key hab_super_root_key[];

#endif /*__BOOT_IMX_SRK_H__*/
