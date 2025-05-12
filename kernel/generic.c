#include <kernel/sched.h>
#include <kernel/pci.h>
#include <kernel/ata.h>
#include <kernel/vfs.h>
#include <kernel/printf.h>
#include <kernel/version.h>

void generic_startup(void) {
    vfs_install();
	pci_scan();
	ata_install();
	sched_install();
}

void generic_main(void) {
    printf("\nWelcome to \033[93mnetwide\033[0m!\n%s %d.%d %s %s %s\n\n",
        __kernel_name, __kernel_version_major,__kernel_version_minor,
        __kernel_build_date, __kernel_build_time, __kernel_arch);

	sched_start_all_cores();
}
