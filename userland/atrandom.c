#include <stdio.h>
#include <stdint.h>
#include <elf.h>

extern uintptr_t __stack_chk_guard;

/* Value mlibc falls back to when AT_RANDOM is absent, see initStackGuard. */
#define FALLBACK_CANARY (((uintptr_t)'\n' << 16) | ((uintptr_t)255 << 24))

int main(int argc, char **argv)
{
	unsigned char *rnd = NULL;

	char **e = argv + argc + 1;
	while (*e) e++;
	for (Elf32_auxv_t *it = (Elf32_auxv_t *)(e + 1); it->a_type != AT_NULL; it++)
		if (it->a_type == AT_RANDOM)
			rnd = (unsigned char *)it->a_un.a_val;

	if (!rnd) {
		printf("AT_RANDOM absent\n");
		return 1;
	}

	printf("AT_RANDOM at %p:", (void *)rnd);
	for (int i = 0; i < 16; i++)
		printf(" %02x", rnd[i]);
	printf("\n");

	printf("__stack_chk_guard = %08lx\n", (unsigned long)__stack_chk_guard);
	printf("expected from auxv = %02x%02x%02x%02x\n", rnd[3], rnd[2], rnd[1], rnd[0]);
	printf("fallback canary    = %08lx\n", (unsigned long)FALLBACK_CANARY);

	if (__stack_chk_guard == FALLBACK_CANARY)
		printf("=> AT_RANDOM NOT used\n");
	else if (__stack_chk_guard == *(uintptr_t *)rnd)
		printf("=> AT_RANDOM used\n");
	else
		printf("=> guard matches neither\n");
	return 0;
}
