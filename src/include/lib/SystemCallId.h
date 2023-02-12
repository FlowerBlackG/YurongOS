/*

    系统调用号。
    创建于 2023年2月12日 江西省上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>

enum class SystemCallId : int64_t {
    A, B, C, D, E
};


#if 0
static void __check_size() {
    sizeof(SystemCallId::C);
}
#endif
