#ifndef _IO_H_INCLUDED_
#define _IO_H_INCLUDED_

// u-boot-2017.09/arch/arm/include/asm/io.h に倣い、書き込み操作前および
// 読み出し操作後にメモリバリア命令を実行するようにしている。
// 何故この順番となっているのかについては分からないため、調査が必要。

static inline void writereg32(unsigned long addr, unsigned long data)
{
	asm volatile("dmb");
	*(volatile unsigned long *)addr = data;
}

static inline unsigned long readreg32(unsigned long addr)
{
	unsigned long data;

	data = *(volatile unsigned long *)addr;
	asm volatile("dmb");

	return data;
}

#endif
