/*

    系统调用 api

    创建于 2023年2月7日 江西省上饶市玉山县

*/

#include <lib/syscalls.h>
#include <lib/sys/types.h>

__force_inline __omit_frame_pointer static int64_t __syscall0(...) {
    int64_t res;
    __asm (
        "syscall"
        : "=a" (res)
        :
    );
    return res;
}

__force_inline __omit_frame_pointer static int64_t __syscall1(...) {
    int64_t res;
    __asm (
        "syscall"
        : "=a" (res)
        :
    );
    return res;
}


__force_inline __omit_frame_pointer static int64_t __syscall2(...) {
    int64_t res;
    __asm (
        "syscall"
        : "=a" (res)
        :
    );
    return res;
}


__force_inline __omit_frame_pointer static int64_t __syscall3(...) {
    int64_t res;
    __asm (
        "syscall"
        : "=a" (res)
        :
    );
    return res;
}

__force_inline __omit_frame_pointer static int64_t __syscall4(...) {
    int64_t res;
    __asm (
        "movq %%rcx, %%r10 \n\t"
        "syscall"
        : "=a" (res)
        :
    );
    return res;
}

__force_inline __omit_frame_pointer static int64_t __syscall5(...) {
    int64_t res;
    __asm (
        "movq %%rcx, %%r10 \n\t"
        "syscall"
        : "=a" (res)
        :
    );
    return res;
}

__force_inline __omit_frame_pointer static int64_t __syscall6(...) {
    int64_t res;
    __asm (
        "movq %%rcx, %%r10 \n\t"
        "syscall"
        : "=a" (res)
        :
    );
    return res;
}


int64_t testCall() {
    __asm ("" :: "a" (SystemCallId::TEST_CALL));
    return __syscall0();
}


int64_t write(int64_t fd, const char* buffer, size_t count) {
    __asm ("" :: "a" (SystemCallId::WRITE));
    return __syscall3(fd, buffer, count);
}

int64_t sleep(int64_t milliseconds) {
    __asm ("" :: "a" (SystemCallId::SLEEP));
    return __syscall1(milliseconds);
}
