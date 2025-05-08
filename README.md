# Netwide
Netwide is a 64-bit operating system targeting x86_64 and RISC-V

# Features on x86_64
- Supports the multiboot2 protocol
- 4-level paging with 48-bit addressing
- VGA text mode and serial driver
- ACPI table parsing
- LAPIC & IOAPIC support
- HPET timer support
- PIT support

# Features on riscv
- Virtio UART driver

# Building (x86_64)
To build, you need to install the following packages:
- dev-essential
- nasm
- grub
- xorriso
- mtools
- qemu-system-x86

Then, you can run `make run` and the kernel will run in QEMU.

# TODO
- [X] `panic()` function
- [X] ANSI support in the VGA driver
- [X] Write a scheduler 
- [X] Write a VFS
- [X] FADT cleanup
- [x] PCI
- [X] SMP 
- [ ] ATAPIO/NVMe driver
- [ ] Proper ext2 driver
- [x] Framebuffer support
- [ ] PS/2 drivers
    - [ ] Keyboard
    - [ ] Mouse
- [ ] TSS and ring3
- [ ] ELF loading using the MMU
- [ ] Symbol table
- [ ] Initial filesystem
- [X] `unimplemented` macro
