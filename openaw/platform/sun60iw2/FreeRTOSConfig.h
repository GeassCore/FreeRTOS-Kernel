/* SPDX-License-Identifier: MIT */
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>

#define configCPU_CLOCK_HZ			(200000000UL)
#define configTICK_RATE_HZ			((TickType_t)1000)
#define configMAX_PRIORITIES			(5)
#define configMINIMAL_STACK_SIZE		((uint16_t)256)
#define configTOTAL_HEAP_SIZE			((size_t)(48 * 1024))
#define configMAX_TASK_NAME_LEN			(16)
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			1
/* CPUS extended timer 0 supplies the scheduler tick. */
#define configUSE_PREEMPTION			1
#define configUSE_MUTEXES			1
#define configUSE_RECURSIVE_MUTEXES		0
#define configUSE_COUNTING_SEMAPHORES		1
#define configUSE_TIMERS			1
#define configUSE_TIME_SLICING 1
#define configUSE_TRACE_FACILITY 1
#define configTIMER_TASK_PRIORITY 3
#define configTIMER_QUEUE_LENGTH 8
#define configTIMER_TASK_STACK_DEPTH 256
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0
#define configUSE_MALLOC_FAILED_HOOK		1
#define configCHECK_FOR_STACK_OVERFLOW		2
#define configSUPPORT_STATIC_ALLOCATION		0
#define configSUPPORT_DYNAMIC_ALLOCATION	1

#define configASSERT(x) do { if (!(x)) { \
 extern void platform_fault(uint32_t, uint32_t); \
 platform_fault(0xdead0001u, __LINE__); } } while (0)

#define INCLUDE_vTaskDelay			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelete			1
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_xTaskGetSchedulerState		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_xTaskGetCurrentTaskHandle	1

#endif
