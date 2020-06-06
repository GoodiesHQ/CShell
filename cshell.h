#ifndef CSHELL_H
#define CSHELL_H

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <bits/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#define ARG unsigned long
#define RET ssize_t

/* linked externally to syscall_x{ARCH}.s assembly wrapper */
extern RET cshell1(unsigned long syscall_ord, ARG arg1);
extern RET cshell2(unsigned long syscall_ord, ARG arg1, ARG arg2);
extern RET cshell3(unsigned long syscall_ord, ARG arg1, ARG arg2, ARG arg3);
extern RET cshell4(unsigned long syscall_ord, ARG arg1, ARG arg2, ARG arg3, ARG arg4);
extern RET cshell5(unsigned long syscall_ord, ARG arg1, ARG arg2, ARG arg3, ARG arg4, ARG arg5);
extern RET cshell6(unsigned long syscall_ord, ARG arg1, ARG arg2, ARG arg3, ARG arg4, ARG arg5, ARG arg6);

#define cs_err(val) (val) < 0 ? -val : val

/* Macro definitions for ubiquitous syscalls that call no external libraries */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* file management */
#define cs_close(fd) cshell1(SYS_close, (ARG)(fd))
#define cs_fstat(fd, statbuf) cshell2(SYS_fstat, (ARG)(fd), (ARG)(statbuf))
#define cs_open(filename, flags, mode) cshell3(SYS_open, (ARG)(filename), (ARG)(flags), (ARG)(mode))
#define cs_stat(filename, statbuf) cshell2(SYS_stat, (ARG)(filename), (ARG)(statbuf))

/* streams and file descriptors */
#define cs_dup2(oldfd, newfd) cshell2(SYS_dup2, (ARG)(oldfd), (ARG)(newfd))
#define cs_read(fd, buf, count) cshell3(SYS_read, (ARG)(fd), (ARG)(buf), (ARG)(count))
#define cs_write(fd, buf, count) cshell3(SYS_write, (ARG)(fd), (ARG)(buf), (ARG)(count))

/* process management */
#define cs_fork() cshell1(SYS_fork, (ARG)0)
#define cs_wait4(pid, status, options, rusage) cshell4(SYS_wait4, (ARG)(pid), (ARG)(status), (ARG)(options), (ARG)(rusage))
#define cs_exit(error_code) cshell1(SYS_exit, (ARG)(error_code))
#define cs_execve(name, argv, envp) cshell3(SYS_execve, (ARG)(name), (ARG)(argv), (ARG)(envp))
#define cs_nanosleep(req, rem) cshell2(SYS_nanosleep, (ARG)(req), (ARG)(rem))

/* memory management */
#ifdef SYS_mmap2
#define cs_mmap2(addr, len, prot, flags, fd, off) cshell6(SYS_mmap2, (ARG)(addr), (ARG)(len), (ARG)(prot), (ARG)(flags), (ARG)(fd), (ARG)(off))
#else
#define cs_mmap2 cs_mmap
#endif
#define cs_mmap(addr, len, prot, flags, fd, off) cshell6(SYS_mmap, (ARG)(addr), (ARG)(len), (ARG)(prot), (ARG)(flags), (ARG)(fd), (ARG)(off))
#define cs_mprotect(addr, len, prot) cshell3(SYS_mprotect, (ARG)(addr), (ARG)(len), (ARG)(prot))
#define cs_munmap(addr, len) cshell2(SYS_munmap, (ARG)(addr), (ARG)(len))

/* sockets */
#define cs_socket(family, type, protocol) cshell3(SYS_socket, (ARG)(family), (ARG)(type), (ARG)(protocol))
#define cs_connect(fd, uservaddr, addrlen) cshell3(SYS_connect, (ARG)(fd), (ARG)(uservaddr), (ARG)(addrlen))

#endif
