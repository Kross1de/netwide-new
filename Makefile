AS = nasm
CC = clang
LD = ld

KERNEL_S_SOURCES = $(shell cd kernel && find -L * -type f -name '*.S')
KERNEL_C_SOURCES = $(shell cd kernel && find -L * -type f -name '*.c')

KERNEL_OBJS := $(addprefix bin/kernel/, $(KERNEL_S_SOURCES:.S=.S.o) $(KERNEL_C_SOURCES:.c=.c.o))

ASFLAGS = -f elf64 -g -F dwarf
CCFLAGS = -m64 -std=gnu11 -g -ffreestanding -Wall -Wextra -nostdlib -Iboot/ -Ikernel/ -fno-stack-protector -Wno-unused-parameter -fno-stack-check -fno-lto -mno-red-zone
QEMUFLAGS = -serial stdio -cdrom bin/$(IMAGE_NAME).iso -boot d -M q35
LDFLAGS = -m elf_x86_64 -Tkernel/arch/x86_64/linker.ld -z noexecstack

IMAGE_NAME = image

all: boot kernel iso

run: all
	@qemu-system-x86_64 $(QEMUFLAGS)

run-gdb: all
	@qemu-system-x86_64 $(QEMUFLAGS) -S -s

bin/kernel/%.c.o: kernel/%.c
	@echo " CC $<"
	@mkdir -p "$$(dirname $@)"
	@$(CC) $(CCFLAGS) -c $< -o $@

bin/kernel/%.S.o: kernel/%.S
	@echo " AS $<"
	@mkdir -p "$$(dirname $@)"
	@$(AS) $(ASFLAGS) -o $@ $<

kernel: $(KERNEL_OBJS)
	@echo " LD kernel/*"
	@$(LD) $(LDFLAGS) $^ -o bin/kernel.elf

iso:
	@grub-file --is-x86-multiboot2 ./bin/kernel.elf; \
	if [ $$? -eq 1 ]; then \
		echo " error: kernel.elf is not a valid multiboot2 file"; \
		exit 1; \
	fi
	@mkdir -p iso_root/boot/grub/
	@cp bin/kernel.elf iso_root/boot/kernel.elf
	@cp boot/grub.cfg iso_root/boot/grub/grub.cfg
	@grub-mkrescue -o bin/$(IMAGE_NAME).iso iso_root/ -quiet 2>&1 >/dev/null | grep -v libburnia | cat
	@rm -rf iso_root/

clean:
	@rm -f $(BOOT_OBJS) $(KERNEL_OBJS)
	@rm -rf bin
