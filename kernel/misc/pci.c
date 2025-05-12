#include <stdint.h>
#include <stddef.h>
#include <kernel/arch/x86_64/io.h>
#include <kernel/pci.h>
#include <kernel/printf.h>
#include <kernel/assert.h>

struct pci_device primary_bus[32];

uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
#ifdef __x86_64__
    uint32_t address = (uint32_t)((bus << 16) | (device << 11) |
        (function << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
#else
    unimplemented;
    return 0;
#endif
}

void pci_write(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
#ifdef __x86_64__
    uint32_t address = (uint32_t)((bus << 16) | (device << 11) |
        (function << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, value);
#else
    unimplemented;
#endif
}

uint8_t pci_get_irq_line(uint8_t bus, uint8_t device, uint8_t function) {
    return (uint8_t)pci_read(bus, device, function, 0x3C);
}

uint16_t pci_config_read_word(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    return pci_read(bus, device, function, offset) >> ((offset & 2) * 8);
}

uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device, uint8_t function) {
    return pci_config_read_word(bus, device, function, 0x02);
}

uint16_t pci_get_device_id(uint8_t bus, uint8_t device, uint8_t function) {
    return pci_config_read_word(bus, device, function, 0x00);
}

uint8_t pci_get_class(uint8_t bus, uint8_t device, uint8_t function) {
    return (uint8_t)(pci_config_read_word(bus, device, function, 0x0A) >> 8);
}

uint8_t pci_get_subclass(uint8_t bus, uint8_t device, uint8_t function) {
    return (uint8_t)(pci_config_read_word(bus, device, function, 0x0A));
}

uint8_t pci_get_header_type(uint8_t bus, uint8_t device, uint8_t function) {
    return (uint8_t)(pci_config_read_word(bus, device, function, 0x0E));
}

uint8_t pci_get_secondary_bus(uint8_t bus, uint8_t device, uint8_t function) {
    uint8_t header_type = pci_get_header_type(bus, device, function);

    if (header_type != 0x1) {
        return 0xff;
    }

    return (uint8_t)(pci_config_read_word(bus, device, function, 0x18) >> 8);
}

void pci_check_device(uint8_t bus, uint8_t device) {
    uint8_t function = 0;
    uint16_t vendor_id = pci_get_vendor_id(bus, device, 0);
    uint16_t device_id = pci_get_device_id(bus, device, 0);
    uint16_t class = pci_get_class(bus, device, 0);
    uint16_t subclass = pci_get_subclass(bus, device, 0);

    if (vendor_id == 0xFFFF) return;

    pci_check_function(bus, device, function);
    uint16_t header_type = pci_get_header_type(bus, device, function);
    
    if ((header_type & 0x80) != 0) {
        for (function = 1; function < 8; function++) {
            if (pci_get_vendor_id(bus, device, function) != 0xFFFF) {
                pci_check_function(bus, device, function);
            }
        }
    }

    if (bus == 0) {
        primary_bus[device].bus = 0;
        primary_bus[device].device = device;
        primary_bus[device].class = class;
        primary_bus[device].subclass = subclass;
        primary_bus[device].vendor_id = vendor_id;
        primary_bus[device].device_id = device_id;
    }
    
	dprintf("Vendor=0x%x,Device=0x%x,Class=0x%x,Subclass=0x%x\n", vendor_id, device_id, class, subclass);
}

void pci_check_bus(uint8_t bus) {
    uint8_t device;

    for (device = 0; device < 32; device++) {
        pci_check_device(bus, device);
    }
}

void pci_check_function(uint8_t bus, uint8_t device, uint8_t function) {
    uint8_t base_class = pci_get_class(bus, device, function);
    uint8_t sub_class = pci_get_subclass(bus, device, function);
    uint8_t secondary_bus;

    if ((base_class == 0x6) && (sub_class == 0x4)) {
        secondary_bus = pci_get_secondary_bus(bus, device, function);
        pci_check_bus(secondary_bus);
    }
}

struct pci_device *pci_get_device(uint16_t class, uint16_t subclass) {
	for (int i = 0; i < 32; i++) {
		if (primary_bus[i].class == class && primary_bus[i].subclass == subclass) {
			return &primary_bus[i];
		}
	}
	return NULL;
}

struct pci_device *pci_get_device_from_vendor(uint16_t vendor, uint16_t device) {
	for (int i = 0; i < 32; i++) {
		if (primary_bus[i].vendor_id == vendor && primary_bus[i].device_id == device) {
			return &primary_bus[i];
		}
	}
	return NULL;
}

void pci_scan(void) {
    uint8_t function;
    uint8_t bus;

    uint16_t header_type = pci_get_header_type(0, 0, 0);
    if ((header_type & 0x80) == 0) {
        /* single PCI host controller */
        pci_check_bus(0);
    } else {
        /* multiple PCI host controllers */
        for (function = 0; function < 8; function++) {
            if (pci_get_vendor_id(0, 0, function) != 0xFFFF) break;
            bus = function;
            pci_check_bus(bus);
        }
    }
    printf("\033[92m * \033[97mInitialized PCI\033[0m\n");
}
