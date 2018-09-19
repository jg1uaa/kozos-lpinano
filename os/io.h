#ifndef _IO_H_INCLUDED_
#define _IO_H_INCLUDED_

static inline void writereg32(unsigned long addr, unsigned long data)
{
	*(volatile unsigned long *)addr = data;
}

static inline unsigned long readreg32(unsigned long addr)
{
	unsigned long data;

	data = *(volatile unsigned long *)addr;

	return data;
}

#endif
