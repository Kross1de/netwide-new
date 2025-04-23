# Netwide
Netwide is a simple 64-BIT operating system for: RISC-V, x86_64 architectures

# TODO
1. Write a VFS (not completed)
2. Make framebuffer support (not completed)
3. Start working for RISC-V architecture (started work)

# FEATURES (x86_64)
1. multiboot2 support
2. 4-level paging
3. VGA, Serial drivers
4. ACPI table parsing
5. APIC support
6. HPET support

# FEATURES (RISC-V)
1. Virtio UART driver

# Build (x86_64)
TOOLS NEEDED:
  dev-essential
  nasm
  grub
  xorriso
  mtools
  qemu-system-x86_64

To build and run use 'make run'  
