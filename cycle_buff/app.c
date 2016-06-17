#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "buff.h"

int main (void) 
{
	BYTE tmp;
	
	printf("cbuff test start\n");
	
	/* cycle buffer init */
	buff_init(&cbuff,0);
   
	/* push 1 byte data to cbuff */
	printf("out 1 byte: 'a'\n");
	buff_inb(&cbuff,'a');
	
	printf("cbuff size %d\n",buff_dlen(&cbuff));

	/* out 1 byte data from cbuff*/
	printf("out 1 byte: %c\n",tmp);
	buff_outb(&cbuff,&tmp);
	
	printf("cbuff size %d\n",buff_dlen(&cbuff));

	return 0;
}

