#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/arch/x86_64/idt.h>
#include <kernel/arch/x86_64/ioapic.h>
#include <kernel/printf.h>

__attribute__((aligned(0x10))) struct idt_entry idt_entries[256];
struct idtr idt_descriptor;
extern void *idt_int_table[];

void *irq_handlers[256];

const char *isr_errors[32] = {
    "division by zero",
    "debug",
    "non-maskable interrupt",
    "breakpoint",
    "detected overflow",
    "out-of-bounds",
    "invalid opcode",
    "no coprocessor",
    "double fault",
    "coprocessor segment overrun",
    "bad TSS",
    "segment not present",
    "stack fault",
    "general protection fault",
    "page fault",
    "unknown interrupt",
    "coprocessor fault",
    "alignment check",
    "machine check",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved"};

void idt_install(void)
{
    for (uint16_t i = 0; i < 256; i++)
    {
        idt_set_entry(i, (uint64_t)idt_int_table[i], 0x08, 0x8E);
    }

    idt_descriptor = (struct idtr){
        .size = sizeof(struct idt_entry) * 256 - 1,
        .offset = (uint64_t)idt_entries};

    asm volatile("lidt %0" ::"m"(idt_descriptor));
    dprintf("%s:%d: IDT address: 0x%lx\n", __FILE__, __LINE__, (uint64_t)&idt_descriptor);
}

void idt_set_entry(uint8_t index, uint64_t base, uint16_t selector, uint8_t type)
{
    idt_entries[index].base_low = base & 0xFFFF;
    idt_entries[index].selector = selector;
    idt_entries[index].zero = 0x00;
    idt_entries[index].type = type;
    idt_entries[index].base_mid = (base >> 16) & 0xFFFF;
    idt_entries[index].base_high = (base >> 32) & 0xFFFFFFFF;
    idt_entries[index].resv = 0;
}

void irq_register(uint8_t vector, void *handler)
{
    if (ioapic_enabled && vector <= 15)
        ioapic_redirect_irq(0, vector + 32, vector, false);
    irq_handlers[vector] = handler;
}

void irq_unregister(uint8_t vector)
{
    irq_handlers[vector] = (void *)0;
}

void isr_handler(struct registers *r)
{
    if (r->int_no == 0xff)
    {
        return;
    }

    uint64_t cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));

    dprintf("%s:%d: x86 Fault: \033[91m%s\033[0m\n"
            "rdi: 0x%lx rsi: 0x%lx rbp:    0x%lx\n"
            "rsp: 0x%lx rbx: 0x%lx rdx:    0x%lx\n"
            "rcx: 0x%lx rax: 0x%lx rip:    0x%lx\n"
            "r8:  0x%lx r9:  0x%lx r10:    0x%lx\n"
            "r11: 0x%lx r12: 0x%lx r13:    0x%lx\n"
            "r14: 0x%lx r15: 0x%lx cr2:    0x%lx\n"
            "cs:  0x%lx ss:  0x%lx rflags: 0x%lx\n",
            __FILE__, __LINE__, isr_errors[r->int_no], r->rdi, r->rsi, r->rbp,
            r->rsp, r->rbx, r->rdx, r->rcx, r->rax, r->rip, r->r8, r->r9,
            r->r10, r->r11, r->r12, r->r13, r->r14, r->r15, cr2, r->cs, r->ss,
            r->rflags);

    asm volatile("cli");
    for (;;)
        asm volatile("hlt");
}

void irq_handler(struct registers r)
{
    void (*handler)(struct registers *);
    handler = irq_handlers[r.int_no - 32];

    if (handler != NULL)
        handler(&r);
}