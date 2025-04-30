#include <kernel/vfs.h>
#include <kernel/printf.h>
#include <kernel/string.h>

int tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
	{
		return c + ('a' - 'A');
	}
	return c;
}

int isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

uint64_t hex_to_long(const char *str)
{
	uint64_t result = 0;
	while (*str)
	{
		char c = *str++;
		result = (result << 4) + (isdigit(c) ? (c - '0') : (tolower(c) - 'a' + 10));
	}
	return result;
}

void debugger_task_entry(void)
{
	char input[128];
	for (;;)
	{
		dprintf("NDS> ");
		
		vfs_read(stdin, input, sizeof(input));
		if (!strncmp(input, "list ", 5))
		{
			uint64_t addr = hex_to_long(input + 5);
			int i, len = 16;
			
			for (i = 0; i < len; i++)
			{
				dprintf("0x%x ", *(uint64_t *)(addr));
				if ((i % 4) == 3)
				{
					dprintf("\n");
				}
				addr += 4;
			}
			continue;
		}
		if (!strncmp(input, "cls", 4) || !strncmp(input, "clear", 6))
		{
			dprintf("\033[2J\033[H");
			continue;
		}
		if (!strncmp(input, "help", 5))
		{
			dprintf("NDS - Netwide Debugger Shell (%s)\n", stdout->name);
			dprintf("Built-in commands: list, cls/clear, help\n");
			continue;
		}
	}
}