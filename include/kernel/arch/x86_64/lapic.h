#pragma once
#define LAPIC_ICRLO 0x0300
#define LAPIC_ICRHI 0x0310
#define LAPIC_ICDESTSHIFT 24

#define LAPIC_REGS 0xfee00000
#define LAPIC_EOI 0xb0
#define LAPIC_SIV 0xf0

#define LAPIC_TIMER_DIV 0x3E0
#define LAPIC_TIMER_LVT 0x320
#define LAPIC_TIMER_INITCNT 0x380
#define LAPIC_TIMER_DISABLE 0x10000
#define LAPIC_TIMER_CURCNT 0x390

#define CPUID_FEAT_EDX_APIC (1 << 9)

#include <stdint.h>
#include <stdbool.h>

void lapic_install(void);
void lapic_calibrate_timer(void);
void lapic_eoi(void);
void lapic_ipi(uint32_t id, uint8_t irq);
void lapic_oneshot(uint8_t vector, uint32_t ms);
void lapic_stop_timer(void);