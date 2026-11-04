# A733 E902 FreeRTOS — RV32E / ilp32e
ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
REPO := $(abspath $(ROOT)/..)
BUILD ?= $(REPO)/build/openaw
PLATFORM := $(ROOT)/platform/sun60iw2
APP := $(ROOT)/app
FREERTOS := $(REPO)

CROSS_COMPILE ?= riscv64-unknown-elf-
CC      := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE    := $(CROSS_COMPILE)size

ARCH_FLAGS := -march=rv32emc -mabi=ilp32e -mcmodel=medany
CFLAGS := $(ARCH_FLAGS) -Os -g -ffunction-sections -fdata-sections \
	-ffreestanding -fno-builtin -MMD -MP -Wall -Wextra -Wno-unused-parameter \
	-I$(PLATFORM)/libc -I$(PLATFORM) -I$(APP) \
	-I$(FREERTOS)/include \
	-D__riscv_xlen=32
ASFLAGS := $(ARCH_FLAGS) -g -I$(PLATFORM)
LDFLAGS := $(ARCH_FLAGS) -T$(PLATFORM)/link.ld -nostdlib \
	-Wl,--gc-sections -Wl,-Map=$(BUILD)/e902-rtos.map

TARGET := $(BUILD)/e902-rtos

FREERTOS_SRCS := \
	$(FREERTOS)/tasks.c \
	$(FREERTOS)/queue.c \
	$(FREERTOS)/list.c \
	$(FREERTOS)/timers.c \
	$(FREERTOS)/event_groups.c \
	$(FREERTOS)/stream_buffer.c \
	$(FREERTOS)/portable/MemMang/heap_4.c

PLATFORM_SRCS := \
	$(PLATFORM)/startup.S \
	$(PLATFORM)/freertos_portASM.S \
	$(PLATFORM)/uart.c \
	$(PLATFORM)/libc.c \
	$(PLATFORM)/platform.c \
	$(PLATFORM)/freertos_hooks.c \
	$(PLATFORM)/freertos_port.c

APP_SRCS := $(APP)/main.c $(APP)/resource_table.c

SRCS := $(PLATFORM_SRCS) $(APP_SRCS) $(FREERTOS_SRCS)
OBJS := $(patsubst $(REPO)/%.c,$(BUILD)/%.o,$(filter %.c,$(SRCS)))
OBJS += $(patsubst $(REPO)/%.S,$(BUILD)/%.o,$(filter %.S,$(SRCS)))

.PHONY: all clean check-freertos

all: $(TARGET).elf $(TARGET).bin
	@$(SIZE) $(TARGET).elf

check-freertos:
	@if [ ! -f "$(FREERTOS)/include/FreeRTOS.h" ]; then \
		echo "Missing FreeRTOS-Kernel. Run: ./scripts/fetch-freertos.sh"; \
		exit 1; \
	fi

$(TARGET).elf: $(OBJS) $(PLATFORM)/link.ld
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lgcc

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/%.o: $(REPO)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(REPO)/%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD)


-include $(OBJS:.o=.d)
