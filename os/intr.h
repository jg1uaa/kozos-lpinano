#ifndef _INTR_H_INCLUDED_
#define _INTR_H_INCLUDED_

#include "interrupt.h"

/* ソフトウエア・割込みベクタの定義 */
#define SOFTVEC_TYPE_NUM     64

#define SOFTVEC_TYPE_SYSCALL 60				// SWI #60
#define SOFTVEC_TYPE_SOFTERR 61				// SWI #61
#define SOFTVEC_TYPE_SERINTR 1

/* 割り込みコントローラ操作部分 */
void intc_init(void);
void intc_enable(softvec_type_t type);
void intc_disable(softvec_type_t type);
void intc_sendeoi(softvec_type_t type);
softvec_type_t intc_vector(void);

#endif
