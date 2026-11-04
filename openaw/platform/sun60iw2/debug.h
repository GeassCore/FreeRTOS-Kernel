/* SPDX-License-Identifier: MIT */
#ifndef E902_DEBUG_H
#define E902_DEBUG_H
#include <stdint.h>
#define E902_DEBUG_MAGIC 0xe902f001u
#define E902_DEBUG_TASKS 8u
struct e902_task_info { char name[16]; uint32_t state, priority, stack_words, number; };
struct e902_debug_info {
 uint32_t magic, abi, sequence, stage;
 uint32_t tick, heartbeat, timer_irq, software_timer;
 uint32_t queue_tx, queue_rx, busy_counter;
 uint32_t mailbox_rx, mailbox_tx, mailbox_last_rx, mailbox_last_tx;
 uint32_t fault, fault_pc, task_count;
 struct e902_task_info tasks[E902_DEBUG_TASKS];
 uint32_t os_id; /* ABI 2: 0 FreeRTOS, 1 Zephyr, 2 NuttX, 3 RT-Thread Nano */
};
#define e902_debug ((volatile struct e902_debug_info *)0x40021000u)
void platform_set_stage(uint32_t stage);
void platform_fault(uint32_t cause, uint32_t pc) __attribute__((noreturn));
#endif
