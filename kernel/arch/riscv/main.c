#include <kernel/printf.h>
#include <kernel/assert.h>
#include <kernel/version.h>

void generic_fatal(void)
{
    for (;;)
        asm volatile("wfi");
}

void generic_pause(void)
{
    return;
}

void kmain()
{
    dprintf("%s %d.%d %s %s %s\n",
            __kernel_name, __kernel_version_major, __kernel_version_minor,
            __kernel_build_date, __kernel_build_time, __kernel_arch);
    generic_fatal();
}