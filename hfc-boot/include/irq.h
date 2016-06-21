#ifndef __IRQ_H__

#define __IRQ_H__



typedef void (*irq_handler_t)(unsigned int);
extern int irq_request(unsigned int irq_num, irq_handler_t func,char *name );
extern int irq_request_free(unsigned int irq_num);
extern void list_active_irqs(void);

#endif
