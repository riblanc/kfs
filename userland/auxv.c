#include <stdio.h>
#include <elf.h>

int    main(int argc, char **argv, char **envp)
{
    // skip argv and NULL, we are now at envp in the user stack
    // we can't use the envp arg because mlibc's copy the env in his own vector and points envp to it
    char **e = argv + argc + 1;
    while (*e) e++;

    for (Elf32_auxv_t *it = (Elf32_auxv_t *)(e + 1); it->a_type != AT_NULL; it++) {
        switch (it->a_type) {
            case AT_EXECFN:
                printf("Type: %ld: Value: %s\n", it->a_type, (char *)it->a_un.a_val);
                break;
            default:
                printf("Type: %ld, Value: %lx\n", it->a_type, it->a_un.a_val);
                break;
        }
    }
    return 0;
}
