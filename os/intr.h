#ifndef _INTR_H_INCLUDED_
#define _INTR_H_INCLUDED_

#include "interrupt.h"

/* ソフトウエア・割込みベクタの定義 */
#define SOFTVEC_TYPE_NUM     160

#define SOFTVEC_TYPE_SYSCALL 0				// SWI #0
#define SOFTVEC_TYPE_SOFTERR 1				// SWI #1
#define SOFTVEC_TYPE_SERINTR 32

/* 割り込みコントローラ操作部分 */
void intc_init(void);
void intc_enable(softvec_type_t type);
void intc_disable(softvec_type_t type);
void intc_sendeoi(softvec_type_t type);
softvec_type_t intc_vector(void);

#endif
