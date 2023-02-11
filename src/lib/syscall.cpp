/*

    系统调用 api

    创建于 2023年2月7日 江西省上饶市玉山县

*/

#include <lib/syscall.h>
#include <lib/sys/types.h>

__force_inline __omit_frame_pointer static void __syscall0(...) {
    __asm (
        "syscall"
    );
}

__force_inline __omit_frame_pointer static void __syscall1(...) {
    __asm (
        "syscall"
    );
}


__force_inline __omit_frame_pointer static void __syscall2(...) {
    __asm (
        "syscall"
    );
}


__force_inline __omit_frame_pointer static void __syscall3(...) {
    __asm (
        "syscall"
    );
}

__force_inline __omit_frame_pointer static void __syscall4(...) {
    __asm (
        "movq %rcx, %r10 \n\t"
        "syscall"
    );
}

__force_inline __omit_frame_pointer static void __syscall5(...) {
    __asm (
        "movq %rcx, %r10 \n\t"
        "syscall"
    );
}

__force_inline __omit_frame_pointer static void __syscall6(...) {
    __asm (
        "movq %rcx, %r10 \n\t"
        "syscall"
    );
}


