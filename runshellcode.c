#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

const char shellcode[] = {

};

int main()
{
    printf("Shellcode is %zu bytes.\n", sizeof(shellcode));
    void *mem = mmap(NULL, sizeof(shellcode), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memcpy(mem, shellcode, sizeof(shellcode));
    mprotect(mem, sizeof(shellcode), PROT_READ | PROT_WRITE | PROT_EXEC);

    ((int(*)())mem)();

    return 0;
}
