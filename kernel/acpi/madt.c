#include <stdint.h>
#include <stddef.h>
#include <kernel/acpi.h>
#include <kernel/printf.h>

struct acpi_madt *madt = NULL;
struct madt_ioapic *madt_ioapic_list[16];
struct madt_iso *madt_iso_list[16];
uint32_t madt_lapics = 0;
uint32_t madt_ioapics = 0;
uint32_t madt_isos = 0;

struct madt_lapic_addr *lapic_addr;

__attribute__((no_sanitize("alignment"))) void madt_init()
{
    madt = (struct acpi_madt *)acpi_find_table("APIC");
    dprintf("%s:%d: MADT address: 0x%x\n", __FILE__, __LINE__, (uintptr_t)madt);

    uint32_t i = 0;
    while (i <= madt->length - sizeof(madt))
    {
        struct madt_entry *entry = (struct madt_entry *)(madt->table + i);

        switch (entry->type)
        {
        case 0:
            madt_lapics++;
            break;
        case 1:
            madt_ioapic_list[madt_ioapics++] = (struct madt_ioapic *)entry;
            break;
        case 2:
            madt_iso_list[madt_isos++] = (struct madt_iso *)entry;
            break;
        case 5:
            lapic_addr = (struct madt_lapic_addr *)entry;
            break;
        case 9:
            madt_lapics++;
            break;
        }

        i += entry->length;
    }
    dprintf("%s:%d: system has %d Local APIC%s and %d I/O APIC%s\n", __FILE__, __LINE__, madt_lapics, madt_lapics == 1 ? "" : "s", madt_ioapics, madt_ioapics == 1 ? "" : "s");
}
