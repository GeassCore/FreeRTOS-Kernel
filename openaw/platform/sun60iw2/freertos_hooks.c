/* SPDX-License-Identifier: MIT */
#include "FreeRTOS.h"
#include "task.h"
#include "platform.h"
void vApplicationMallocFailedHook(void) { platform_fault(0xdead0005,0); }
void vApplicationStackOverflowHook(TaskHandle_t task, char *name) { platform_fault(0xdead0006,(uint32_t)task); }
