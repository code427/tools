#include "buff.h"


CBUFF cbuff;

WORD minw(WORD a, WORD b)
{	return (a<b ? a : b);
}

void buff_clrtrial(CBUFF *bp)
{  bp->trial = bp->out;
}

/* Return total length of data in buffer */
WORD buff_dlen(CBUFF *bp)
{	return ((WORD)((bp->in - bp->out) & (bp->len-1)));
}

/* Return length of untried (i.e. unsent) data in buffer */
WORD buff_untriedlen(CBUFF *bp)
{	return((WORD)((bp->in - bp->trial) & (bp->len-1)));
}

/* Return length of trial data in buffer (i.e. data send but unacked) */
WORD buff_trylen(CBUFF *bp)
{	return ((WORD)((bp->trial - bp->out) & (bp->len -1)));
}

/* Return length of free space in buffer */
WORD buff_freelen(CBUFF *bp)
{	return (bp->len ? bp->len-1 - buff_dlen(bp) : 0);
}

/* Set all the buffer pointers to a starting value */
void buff_init(CBUFF *bp, LWORD start)
{	bp->out = bp->in = bp->trial = start;
	cbuff.len = _CBUFFLEN_;
}

/* Load data into buffer, return num of bytes that could be accepted
** If data pointer is null. adjust pointers but don't transfer data */
WORD buff_in(CBUFF *bp, BYTE *data, WORD len)
{	WORD in, n, n1, n2;
	in = (WORD)bp->in & (bp->len-1);	 /* Mask len to buffer area */
	n  = minw(len, buff_freelen(bp));	 /* Get max allowable length */
	n1 = minw(n,(WORD)(bp->len - in));	 /* Length up to end of buff */
	n2 = n - n1;						 /* Length from start of buff */
	if(n1 && data)						 /* If anything to copy ... */
		memcpy (&bp->data[in], data, n1); /* ..copy up to end of buffer ..*/
	if(n2 && data)						 /* and maybe also.. */
		memcpy (&bp->data, &data[n1], n2); /* ..copy into start of buffer */
	bp->in += n;							 /* Bump point */
	return (n);
}

BYTE buff_inb(CBUFF *bp, BYTE b)
{	WORD in;
	in = (WORD)bp->in & (bp->len-1);
	bp->data[in] = b;
	bp->in++;
	return 1;	
}

/* Load string into buffer, return num of chars that could be accepted */
WORD buff_instr(CBUFF *bp, char *str)
{	return(buff_in(bp, (BYTE *)str, (WORD)strlen(str)));
}

/* Remove waiting data from buffer, return number of bytes 
** If data pointer is null, adjust pointers but don't transfer data */
WORD buff_try(CBUFF *bp, BYTE *data, WORD maxlen)
{	WORD trial, n, n1, n2;
	trial = (WORD)bp->trial & (bp->len-1);		 /* Mask trial len to buffer area */
	n  = minw(maxlen, buff_untriedlen(bp));		 /* Get max allowable length */
	n1 = minw(n,(WORD)(bp->len -trial));		 /* Length up to end of buff */
	n2 = n - n1;								 /* Length from start of buff */
	if(n1 && data)								 /* If anything to copy.. */
		memcpy(data, &bp->data[trial], n1);		 /* ..copy up to end of buffer.. */
	if(n2 && data)								 /* ..and maybe also.. */
		memcpy(&data[n1], bp->data, n2);		 /* ..copy from start of buffer */
	bp->trial += n;								 /* Bump trial pointer */
	return(n);
}

/* Remove data from buffer, return number of bytes
** If data pointer is null, adjust pointers but don't transfer data */
WORD buff_out(CBUFF *bp, BYTE *data, WORD maxlen)
{	
	WORD out, n, n1, n2;
	out = (WORD)bp->out & (bp->len-1);			/* Mask len to buffer area */
	n  = minw(maxlen, buff_dlen(bp));			/* Get max allowable length */
	n1 = minw(n, (WORD)(bp->len - out));		/* Length up to end of buff */
	n2 = n - n1;								/* Length from start of buff */
	if(n1 && data)								/* If anything to copy.. */
		memcpy(data, &bp->data[out], n1);		/* ..copy up to end of buffer.. */
	if(n2 && data)								/* ..and maybe also.. */
		memcpy(&data[n1], bp->data, n2);		/* ..copy from start of buffer */
	bp->out +=n;								/* Bump O/P pointer */
	if(buff_untriedlen(bp))						/* ..and maybe trial pointer */
		bp->trial = bp->out;
	return(n);
}

BYTE buff_outb(CBUFF *bp, BYTE *b)
{	WORD out;
	out = (WORD)bp->out & (bp->len-1);
	*b = bp->data[out];
	bp->out++;
	return (1);
}

/* Rewind the trial pointer by the given byte count, return actual count */
WORD buff_retry(CBUFF *bp, WORD len)
{	len = minw(len, buff_trylen(bp));
	bp->trial -= len;
	return(len);
}
