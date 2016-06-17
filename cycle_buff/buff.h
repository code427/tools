#ifndef __BUFF_H__
#define __BUFF_H__


#include <string.h>

#ifndef _CBUFFLEN_
#define _CBUFFLEN_ 0x400	/*cycle buff size 1K*/
#endif

typedef unsigned short WORD;
typedef unsigned int  LWORD;
typedef unsigned char  BYTE;

/* Circular buffer structure */
typedef struct
{	WORD len;		/* length of data  (must be first) */
	LWORD in;		/* Incoming data */
	LWORD out;	/* Outgoing data */
	LWORD trial;/* Outgoing data 'on trial' */
	BYTE data[_CBUFFLEN_];/* buffer */
}CBUFF;

void buff_init(CBUFF *bp, LWORD start);
void buff_clrtrial(CBUFF *bp);
WORD buff_dlen(CBUFF *bp);
WORD buff_freelen(CBUFF *bp);
WORD buff_in(CBUFF *bp, BYTE *data, WORD len);
WORD buff_instr(CBUFF *bp, char *str);
WORD buff_out(CBUFF *bp, BYTE *data, WORD maxlen);
WORD buff_trylen(CBUFF *bp);
WORD buff_untriedlen(CBUFF *bp);
WORD buff_try(CBUFF *bp, BYTE *data, WORD maxlen);
WORD buff_retry(CBUFF *bp, WORD len);
BYTE buff_inb(CBUFF *bp, BYTE b);
BYTE buff_outb(CBUFF *bp, BYTE *b);

extern CBUFF cbuff;

#endif

