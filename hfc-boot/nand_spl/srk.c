/*
 * =====================================================================================
 *
 *       Filename:  srk_1024_release.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012��08��18�� 15ʱ03��24��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhongjun (zj), zhongjun@live.com
 *        Company:  zhongjun
 *
 * =====================================================================================
 */
#include "srk.h"  

 //super root key
const unsigned char hab_super_root_moduli[] = {

	0xD1, 0xD9, 0xA8, 0x5E, 0xCD, 0x83, 0x66, 0x27, 0xED, 0x17, 0xCE, 0xDE, 0x49, 0x44, 0x7F, 0xC4, 
	0x0E, 0xA3, 0x82, 0xF6, 0xBB, 0x94, 0x4B, 0xEA, 0x54, 0xDC, 0x97, 0x78, 0x98, 0x12, 0x2D, 0x58, 
	0x2D, 0xB6, 0x72, 0x96, 0xB5, 0xF6, 0xA6, 0xD0, 0x53, 0x74, 0xBB, 0x5A, 0x3A, 0x57, 0x22, 0x07, 
	0x07, 0x79, 0xE9, 0x97, 0x79, 0xE4, 0x88, 0xFB, 0xA1, 0xEC, 0xA2, 0x32, 0x15, 0xA8, 0x01, 0x91, 
	0x7B, 0x47, 0x56, 0x37, 0x04, 0x72, 0xAD, 0x4D, 0xB5, 0x70, 0x89, 0xBC, 0x9D, 0xC1, 0x90, 0x67, 
	0x31, 0xB8, 0x30, 0x95, 0x82, 0xFE, 0xA4, 0xA8, 0x77, 0x85, 0x89, 0xE1, 0x1F, 0x85, 0xF2, 0xC9, 
	0xC6, 0xC9, 0x15, 0x48, 0x66, 0xE2, 0x1C, 0xEF, 0x91, 0x17, 0x93, 0x40, 0x67, 0xCD, 0x67, 0x1C, 
	0x24, 0xEB, 0xB9, 0x88, 0x9F, 0x97, 0x15, 0xD0, 0x46, 0xB4, 0xE9, 0xF4, 0xB6, 0xA0, 0x36, 0x7C, 
	0x2D, 0x95, 0x63, 0x86, 0x8A, 0x4E, 0xF1, 0x46, 0x6D, 0x2A, 0xA0, 0xF3, 0xA3, 0x1A, 0xF3, 0xC9, 
	0xED, 0x58, 0x6A, 0xE7, 0x7D, 0x5E, 0x02, 0x96, 0x61, 0x00, 0x82, 0x0A, 0x78, 0x1B, 0x28, 0x83, 
	0x86, 0xA1, 0xD0, 0x32, 0x1D, 0x4E, 0x4A, 0xC2, 0xF8, 0x4C, 0x84, 0x13, 0x1A, 0x0A, 0x53, 0x7E, 
	0xFC, 0x73, 0x9B, 0x9F, 0xB4, 0xF1, 0x09, 0x90, 0xC8, 0x5D, 0x97, 0x98, 0xF5, 0x1E, 0xAF, 0x68, 
	0x8F, 0x1C, 0x2A, 0x15, 0xA3, 0x8D, 0x6F, 0x62, 0xC0, 0x3A, 0x12, 0xFE, 0xA8, 0xF7, 0x1E, 0xB8, 
	0x4D, 0x18, 0xF3, 0xC5, 0x5F, 0xE8, 0x8F, 0x30, 0xBE, 0x8D, 0xCD, 0xB0, 0xAF, 0xF2, 0xAA, 0xCD, 
	0x7A, 0x2B, 0x50, 0xC4, 0x71, 0xBE, 0xC8, 0x8E, 0xEE, 0xB5, 0x5B, 0x35, 0xB7, 0xE8, 0x0E, 0xCE, 
	0xF0, 0xE8, 0x9F, 0x0E, 0xF6, 0x22, 0x31, 0xD0, 0x75, 0xD8, 0x58, 0xFE, 0x89, 0x86, 0xEE, 0x17

};


/* Super Root key */
const hab_rsa_public_key hab_super_root_key[] = 
{
	{
	    { 
	    	/* RSA public exponent, right-padded */
	    	0x01, 0x00, 0x01, 0x00
	    },
	    /* pointer to modulus data */
	    (unsigned char *)&hab_super_root_moduli[0],
	    /* Exponent size in bytes */
	    0x03, 
	    /* Modulus size in bytes */
	    0x100, 
	    /* Key data valid */
	     1
	}
};
