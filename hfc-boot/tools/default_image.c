/*
 * (C) Copyright 2008 Semihalf
 *
 * (C) Copyright 2000-2004
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 *
 * Updated-by: Prafulla Wadaskar <prafulla@marvell.com>
 *		default_image specific code abstracted from mkimage.c
 *		some functions added to address abstraction
 *
 * All rights reserved.
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

#include "mkimage.h"
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <image.h>
#include <u-boot/crc.h>

static image_header_t header;


#define FILE_PATH_MAX	80

static RSA *get_private_key(const char *file_name)
{
	RSA *key = RSA_new();
	
	OpenSSL_add_all_algorithms();
	
	BIO *BP = BIO_new_file(file_name,"rb");
	if(NULL == BP)
	{
		return NULL;
	}
	
	key = PEM_read_bio_RSAPrivateKey(BP,NULL,NULL,"hfctest");

	EVP_cleanup();
	
	return key;
}

static int image_check_image_types(uint8_t type)
{
	if (((type > IH_TYPE_INVALID) && (type < IH_TYPE_FLATDT)) ||
	    (type == IH_TYPE_KERNEL_NOLOAD))
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

static int image_check_params(struct mkimage_params *params)
{
	return	((params->dflag && (params->fflag || params->lflag)) ||
		(params->fflag && (params->dflag || params->lflag)) ||
		(params->lflag && (params->dflag || params->fflag)));
}

static int image_verify_header(unsigned char *ptr, int image_size,
			struct mkimage_params *params)
{
	uint32_t len;
	const unsigned char *data;
	uint32_t checksum;
	image_header_t header;
	image_header_t *hdr = &header;

	/*
	 * create copy of header so that we can blank out the
	 * checksum field for checking - this can't be done
	 * on the PROT_READ mapped data.
	 */
	memcpy(hdr, ptr, sizeof(image_header_t));

	if (be32_to_cpu(hdr->ih_magic) != IH_MAGIC) {
		fprintf(stderr,
			"%s: Bad Magic Number: \"%s\" is no valid image\n",
			params->cmdname, params->imagefile);
		return -FDT_ERR_BADMAGIC;
	}

	data = (const unsigned char *)hdr;
	len  = sizeof(image_header_t);

	checksum = be32_to_cpu(hdr->ih_hcrc);
	hdr->ih_hcrc = cpu_to_be32(0);	/* clear for re-calculation */

	if (crc32(0, data, len) != checksum) {
		fprintf(stderr,
			"%s: ERROR: \"%s\" has bad header checksum!\n",
			params->cmdname, params->imagefile);
		return -FDT_ERR_BADSTATE;
	}

	data = (const unsigned char *)ptr + sizeof(image_header_t);
	len  = image_size - sizeof(image_header_t) ;

	checksum = be32_to_cpu(hdr->ih_dcrc);
	if (crc32(0, data, len) != checksum) {
		fprintf(stderr,
			"%s: ERROR: \"%s\" has corrupted data!\n",
			params->cmdname, params->imagefile);
		return -FDT_ERR_BADSTRUCTURE;
	}
	return 0;
}


static inline int image_sign_file_verify(int start_off, char compare_c, const char *name, char *verify)
{
	unsigned char i,temp[16];

	for(i = start_off; i < sizeof(temp) + start_off; i++)
	{
		if(*(name+i) == compare_c)
			break;
	}

	/*Note:
	 *  setting the verify string could check the file name is correct or not
	 *  maybe we could add it in the future */

	return i;
}

static void image_set_sign(image_header_t *hdr, unsigned char *sha_sum, const char *name)
{
	unsigned int offset = 0;
	unsigned int start = 0;
	char *rename;
	unsigned char sign[256] = {0};
	RSA *key = NULL;

	rename = strrchr(name,'/') + 1;

	debug("file rename : %s\n",rename);

	offset = image_sign_file_verify(start,'_', rename,"ABC");
	if(offset)
		strncpy(image_get_issuer(hdr), rename+start, offset-start);

	start = ++offset;
	offset = image_sign_file_verify(start,'_', rename,"ABC");
	if(offset)
		strncpy(image_get_sign_user(hdr), rename+start, offset-start);
	
	start = ++offset;
	offset = image_sign_file_verify(start,'_', rename,"ABC");
	if(offset)
		strncpy(image_get_sign_algo(hdr), rename+start, offset-start);
	
	start = ++offset;
	offset = image_sign_file_verify(start,'_', rename,"ABC");
	if(offset)
		strncpy(image_get_sign_klen(hdr), rename+start, offset-start);
	
	start = ++offset;
	offset = image_sign_file_verify(start,'_', rename,"ABC");
	if(offset)
		strncpy(image_get_sign_model(hdr), rename+start, offset-start);
	
	start = ++offset;
	offset = image_sign_file_verify(start,'p', rename,"ABC");
	if(offset)
		strncpy(image_get_sign_ver(hdr), rename+start, offset-start);


	key = get_private_key(name);
	if(NULL == key)
	{
		return;
	}
	
	RSA_private_encrypt(32,sha_sum,sign,key,RSA_PKCS1_PADDING);

	memcpy(hdr->ih_sign_txt, (const char *)sign, IH_SIGN_TXT);
}

extern void sha256_csum_wd(const unsigned char *input, unsigned int ilen,
		unsigned char *output, unsigned int chunk_sz);

static void image_set_header(void *ptr, struct stat *sbuf, int ifd,
				struct mkimage_params *params)
{
	uint32_t checksum;
	unsigned char sha256sum[32];

	image_header_t * hdr = (image_header_t *)ptr;

	checksum = crc32(0,
			(const unsigned char *)(ptr +
				sizeof(image_header_t)),
			sbuf->st_size - sizeof(image_header_t));

	sha256_csum_wd((const unsigned char *)(ptr + sizeof(image_header_t)),
			(sbuf->st_size - sizeof(image_header_t)),sha256sum,0);
	
	printf("SHA256 SUM:\n");
	int i;
	for(i = 0; i < 32; i++)
	{
		printf("%02x",sha256sum[i]);
	}
	printf("\n");

	/* Build new header */
	image_set_magic(hdr, IH_MAGIC);
	image_set_time(hdr, sbuf->st_mtime);
	image_set_size(hdr, sbuf->st_size - sizeof(image_header_t));
	image_set_load(hdr, params->addr);
	image_set_ep(hdr, params->ep);
	image_set_dcrc(hdr, checksum);
	image_set_os(hdr, params->os);
	image_set_arch(hdr, params->arch);
	image_set_type(hdr, params->type);
	image_set_comp(hdr, params->comp);

	image_set_name(hdr, params->imagename);
	image_set_sign(hdr, sha256sum, params->imagename2);

	checksum = crc32(0, (const unsigned char *)hdr,
				sizeof(image_header_t));

	image_set_hcrc(hdr, checksum);
}

/*
 * Default image type parameters definition
 */
static struct image_type_params defimage_params = {
	.name = "Default Image support",
	.header_size = sizeof(image_header_t),
	.hdr = (void*)&header,
	.check_image_type = image_check_image_types,
	.verify_header = image_verify_header,
	.print_header = image_print_contents,
	.set_header = image_set_header,
	.check_params = image_check_params,
};

void init_default_image_type(void)
{
	mkimage_register(&defimage_params);
}
