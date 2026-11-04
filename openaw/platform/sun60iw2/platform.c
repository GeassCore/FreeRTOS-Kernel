/* SPDX-License-Identifier: MIT */
#include "platform.h"
#define R32(a) (*(volatile uint32_t *)(a))
#define R8(a) (*(volatile uint8_t *)(a))
#define TIMER 0x07091000u
#define CLIC 0xe0800000u
#define CPUX_BOX 0x03004000u
#define CPUS_BOX 0x07094000u
#define CHANNEL 3u
volatile uint32_t e902_boot_stage;
volatile uint32_t e902_heartbeat_count;
void platform_set_stage(uint32_t stage) { e902_boot_stage = stage; e902_debug->stage = stage; }
void platform_fault(uint32_t cause, uint32_t pc)
{
 __asm__ volatile("csrci mstatus, 8" ::: "memory");
 e902_debug->fault = cause; e902_debug->fault_pc = pc;
 platform_set_stage(0xff); for (;;) __asm__ volatile("nop");
}
void platform_early_init(void)
{
 volatile uint32_t *v = (volatile uint32_t *)e902_debug;
 for (unsigned i = 0; i < sizeof(*e902_debug)/4; i++) v[i] = 0;
 e902_debug->abi = 1; e902_debug->magic = E902_DEBUG_MAGIC; platform_set_stage(1);
}
void platform_init_tick(uint32_t hz)
{
 R32(0x0701011c) |= 0x10001; R32(0x07010100) = 1;
 /* RTC DCXO mux matches Linux ccu-sun60i-a733-rtc: 24M, 19.2M, 26M, 24M. */
 static const uint32_t dcxo_hz[] = {24000000u,19200000u,26000000u,24000000u};
 uint32_t source_hz = dcxo_hz[(R32(0x07090160)>>14)&3];
 R32(TIMER+0x20) = 0; R32(TIMER+0x24) = source_hz/hz; R32(TIMER+0x2c) = 0;
 R32(TIMER+0x20) = 2;
 unsigned n = 100000; while ((R32(TIMER+0x20)&2) && --n) ;
 if (!n) platform_fault(0xdead0020, R32(TIMER+0x20));
 R32(TIMER+4) = 1; R32(TIMER) |= 1;
 R32(CLIC+8) = 0; R8(CLIC+0x1002+4*20) = 0;
 R8(CLIC+0x1003+4*20) = 0xff; R8(CLIC+0x1001+4*20) = 1;
 R32(TIMER+0x20) = 1;
}
void platform_ack_tick(void) { R32(TIMER+4) = 1; }
void platform_mailbox_init(void) { R32(0x0701017c) |= 0x10001; R32(0x02002744) |= 0x10001; }
int platform_mailbox_echo(void)
{
 if (!(R32(CPUS_BOX+0x60+4*CHANNEL)&15) || (R32(CPUX_BOX+0x60+4*CHANNEL)&15)>=8) return 0;
 uint32_t word = R32(CPUS_BOX+0x70+4*CHANNEL);
 e902_debug->mailbox_rx++; e902_debug->mailbox_last_rx = word;
 word ^= 0xe9020000u; R32(CPUX_BOX+0x70+4*CHANNEL) = word;
 e902_debug->mailbox_tx++; e902_debug->mailbox_last_tx = word;
 R32(CPUS_BOX+0x24) = 1u<<(CHANNEL*2); return 1;
}
uint32_t platform_get_mtime(void) { uint32_t v; __asm__ volatile("csrr %0, mcycle":"=r"(v)); return v; }
