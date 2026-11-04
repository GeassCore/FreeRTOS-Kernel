/* SPDX-License-Identifier: MIT */
#ifndef PLATFORM_SUN60IW2_H
#define PLATFORM_SUN60IW2_H

#include <stdint.h>
#include "debug.h"

/* CPUS-domain S_UART0 */
#define SUART0_BASE		0x07080000UL
#define SUART_RBR		0x00
#define SUART_THR		0x00
#define SUART_DLL		0x00
#define SUART_DLH		0x04
#define SUART_IER		0x04
#define SUART_FCR		0x08
#define SUART_LCR		0x0c
#define SUART_LSR		0x14
#define SUART_USR		0x7c

#define SUART_LCR_DLAB		(1u << 7)
#define SUART_LCR_8N1		0x03u
#define SUART_LSR_THRE		(1u << 5)
#define SUART_FCR_FIFO_EN	0x01u

/* E902 CFG (for self-info / future PM) */
#define E902_CFG_BASE		0x07032000UL
#define E902_RST_START_ADDR	0x0204u

#ifndef UART_BAUD
#define UART_BAUD		115200u
#endif
/* APB clock for S_UART — board-dependent; default 24 MHz DCXO path */
#ifndef UART_CLK_HZ
#define UART_CLK_HZ		24000000u
#endif

extern volatile uint32_t e902_boot_stage;
extern volatile uint32_t e902_heartbeat_count;
void platform_early_init(void);
void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
void platform_init_tick(uint32_t hz);
void platform_ack_tick(void);
void platform_mailbox_init(void);
int platform_mailbox_echo(void);
uint32_t platform_get_mtime(void);

#endif
