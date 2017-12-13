TARGET=cshell
CC=gcc
CFLAGS=-nostdlib -fno-reorder-functions -fno-align-functions -fno-align-jumps -fno-align-loops -fno-align-labels -fno-prefetch-loop-arrays
SRC=${TARGET}.c

all:
	@- echo "make <x86|x64|clean>"

x86:
	$(CC) -m32 -fno-stack-protector ${CFLAGS} ${SRC} syscall_x86.s -o ${TARGET}-x86

x64:
	$(CC) -fno-stack-protector ${CFLAGS} ${SRC} syscall_x64_2.s -o ${TARGET}-x64

clean:
	rm -f ${TARGET}-x86 ${TARGET}-x64
