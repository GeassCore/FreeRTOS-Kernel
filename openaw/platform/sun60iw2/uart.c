/* SPDX-License-Identifier: MIT */
#include "platform.h"

static inline void mmio_write32(uintptr_t addr, uint32_t val)
{
	*(volatile uint32_t *)addr = val;
}

static inline uint32_t mmio_read32(uintptr_t addr)
{
	return *(volatile uint32_t *)addr;
}

void uart_init(void)
{
	uintptr_t base = SUART0_BASE;
	uint32_t div = UART_CLK_HZ / (16u * UART_BAUD);

	mmio_write32(base + SUART_IER, 0);
	mmio_write32(base + SUART_LCR, SUART_LCR_DLAB);
	mmio_write32(base + SUART_DLL, div & 0xffu);
	mmio_write32(base + SUART_DLH, (div >> 8) & 0xffu);
	mmio_write32(base + SUART_LCR, SUART_LCR_8N1);
	mmio_write32(base + SUART_FCR, SUART_FCR_FIFO_EN);
}

void uart_putc(char c)
{
	uintptr_t base = SUART0_BASE;

	if (c == '\n')
		uart_putc('\r');
	uint32_t timeout = 100000;
	while ((mmio_read32(base + SUART_LSR) & SUART_LSR_THRE) == 0) {
		if (!--timeout)
			return;
	}
	mmio_write32(base + SUART_THR, (uint32_t)(uint8_t)c);
}

void uart_puts(const char *s)
{
	while (s && *s)
		uart_putc(*s++);
}
