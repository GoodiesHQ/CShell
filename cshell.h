#define _GNU_SOURCE
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define ARG uintptr_t
#define RET ssize_t

extern RET cshell1(unsigned long syscall_ord, ARG arg1);
extern RET cshell2(unsigned long syscall_ord, ARG arg1, ARG arg2);
extern RET cshell3(unsigned long syscall_ord, ARG arg1, ARG arg2, ARG arg3);
extern RET cshell4(unsigned long syscall_ord, ARG arg1, ARG arg2, ARG arg3, ARG arg4);
extern RET cshell5(unsigned long syscall_ord, ARG arg1, ARG arg2, ARG arg3, ARG arg4, ARG arg5);
extern RET cshell6(unsigned long syscall_ord, ARG arg1, ARG arg2, ARG arg3, ARG arg4, ARG arg5, ARG arg6);

#define cs_err(val) (val) < 0 ? -val : val

/* Macro definitions of ubiquitous syscalls that call no external libraries */
#define cs_read(fd, buf, count) cshell3(SYS_read, (ARG)fd, (ARG)buf, (ARG)count)
#define cs_write(fd, buf, count) cshell3(SYS_write, (ARG)fd, (ARG)buf, (ARG)count)
#define cs_open(filename, flags, mode) cshell3(SYS_open, (ARG)filename, (ARG)flags, (ARG)mode)
#define cs_close(fd) cshell1(SYS_close, (ARG)fd)
#define cs_stat(filename, statbuf) cshell2(SYS_stat, (ARG)filename, (ARG)statbuf)
#define cs_fstat(fd, statbuf) cshell2(SYS_fstat, (ARG)fd, (ARG)statbuf)
#define cs_exit(error_code) cshell1(SYS_exit, (ARG)error_code)
#define cs_dup2(oldfd, newfd) cshell2(SYS_dup2, (ARG)oldfd, (ARG)newfd)
#define cs_execve(name, argv, envp) cshell3(SYS_execve, (ARG)name, (ARG)argv, (ARG)envp)

/* sockets */
#define cs_socket(family, type, protocol) cshell3(SYS_socket, (ARG)family, (ARG)type, (ARG)protocol)
#define cs_connect(fd, uservaddr, addrlen) cshell3(SYS_connect, (ARG)fd, (ARG)uservaddr, (ARG)addrlen)
