#ifndef _SHA256_H
#define _SHA256_H

#define SHA256_SUM_LEN	32

/* Reset watchdog each time we process this many bytes */
#define CHUNKSZ_SHA256	(64 * 1024)

typedef struct {
	unsigned int total[2];
	unsigned int state[8];
	unsigned char buffer[64];
} sha256_context;

void sha256_starts(sha256_context * ctx);
void sha256_update(sha256_context *ctx, const unsigned char *input, unsigned int length);
void sha256_finish(sha256_context * ctx, unsigned char digest[SHA256_SUM_LEN]);

void sha256_csum_wd(const unsigned char *input, unsigned int ilen,
		unsigned char *output, unsigned int chunk_sz);

#endif /* _SHA256_H */
