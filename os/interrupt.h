#ifndef _INTERRUPT_H_INCLUDED_
#define _INTERRUPT_H_INCLUDED_

typedef short softvec_type_t;

typedef void (*softvec_handler_t)(softvec_type_t type, unsigned long sp);

/* 割り込みの許可・禁止 */
void intr_enable(void);		// -> startup.S
void intr_disable(void);	// -> startup.S
#define INTR_ENABLE  intr_enable()
#define INTR_DISABLE intr_disable()

/* 例外ハンドラ */
void exception_undef(unsigned long sp);
void exception_swi(unsigned long sp);
void exception_prefetch_abort(unsigned long sp);
void exception_data_abort(unsigned long sp);
void exception_irq(unsigned long sp);

#endif
