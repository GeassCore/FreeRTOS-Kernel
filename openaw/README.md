# A733 E902 FreeRTOS (CPUS)

FreeRTOS firmware with a preemptive RV32E port for the Allwinner A733 **RISC-V E902** management core
(CPUS domain). This is **not** the closed ARISC/SCP power-management firmware;
OpenTina does not pack `scp.bin`, so E902 can run this RTOS instead.

## Memory map (E902 view)

| Region | Address | Notes |
|--------|---------|--------|
| Code/data | `0x4000A000` | SRAM window (ARM PA `0x0004A000`) |
| Stack top | `0x40021000` | Firmware limit, 4 KiB below BL31 |
| Linux telemetry | `0x40021000` | ARM PA `0x00061000`, last 4 KiB of reservation |
| Firmware limit | 92 KiB | `0x4000A000` … `0x40020FFF` |

Hardware start PC is programmed via `E902_RST_START_ADDR` (`0x07032000 + 0x204`)
after `RST_BUS_RISCV_CFG` is deasserted, while the E902 core is still held in
reset by bit 0 at `0x07050008`. The core is released only after the PC is set.

## Toolchain

Need a **RV32E** (or RV32I) bare-metal toolchain, for example:

```bash
# Debian/Ubuntu example (adjust to your install)
export CROSS_COMPILE=riscv64-unknown-elf-
# Build still uses -march=rv32emc -mabi=ilp32e
```

## Fetch FreeRTOS-Kernel

```bash
./scripts/fetch-freertos.sh
```

## Build

```bash
make                     # -> build/e902-rtos.elf + build/e902-rtos.bin
make clean
```

OpenTina integration: `./build.sh <BOARD> build rtos` (see repo `README.md`).

## Boot

**U-Boot** (after loading `.bin` to DRAM, then copy/start):

```
# load e902-rtos.bin to $loadaddr, then:
boote902 $loadaddr
```

**Linux remoteproc**: enable `e902_rproc` in DT, place firmware as
`/lib/firmware/e902-rtos.elf` (or the name in `firmware-name`).

## Observe from Linux (no E902 UART required)

Build the observer with the rootfs AArch64 compiler:

```sh
make linux-status LINUX_CC=/path/to/buildroot/output/host/bin/aarch64-linux-gcc
```

Install `build/e902-status` as `/usr/bin/e902-status` on Linux. Run as root:

```sh
e902-status                 # one snapshot
e902-status --watch         # one snapshot per second, Ctrl-C stops watching
e902-status --ping 0x12345678
```

The observer reads aligned 32-bit words from `/dev/mem` at ARM SRAM PA
`0x61000`. It requires the matching telemetry ABI and uses a sequence counter
for the task table. `running` is the remoteproc driver's state; increasing
`tick`/`timer_irq` and heartbeat prove execution. A stopped processor can retain
an old SRAM snapshot, so always check both the driver state and counter progress.
The task table is captured once per second by `monitor`; its running task is
therefore normally `monitor`. Stack free bytes are a high-water mark (minimum
unused stack since creation), not instantaneous stack usage. Counters wrap at
32 bits. This does not currently report per-task CPU utilization or task stacks.

The application creates six tasks plus FreeRTOS IDLE and timer-service tasks.
The `cpu-load` task never yields. A 1 kHz CPUS extended S_TIMER0 interrupt
(CLIC IRQ 20) preempts it; queue producer/consumer execute every 20 ms, heartbeat
every second, and a software timer every 250 ms. Timer frequency is derived from
RTC DCXO selection (24/19.2/26 MHz), not assumed to be 24 MHz. Interrupts are
non-nested; ISR queue APIs use the port's interrupt-mask helpers.

Faults are retained in telemetry, with stage `0xff`. Exceptions store raw
`mcause` and `mepc`; assertions store `0xdead0001` and the source line number;
`0xdead0005` means allocation failure, `0xdead0006` stack overflow (TCB address),
and `0xdead0020` timer reload timeout. This is a diagnostic interface, not a
Linux remoteproc crash/recovery notification.

Mailbox `--ping` uses a polling echo protocol on hardware channel 3. Linux
writes CPUS FIFO at `0x0709407c`; E902 replies through CPUX FIFO at `0x0300407c`,
with `reply = request XOR 0xe9020000`. Channel 0 remains reserved for the existing
Linux remoteproc mailbox. This is a hardware communication test, not RPMsg or
a production message transport. Do not assign channel 3 to another client while
using this diagnostic. The observer serializes its own tests using a lock file.

S_UART0 code remains available, but the application does not initialize or
print to UART. Do not connect or reconfigure the Linux console to observe E902.
