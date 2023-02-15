/*

    系统调用号。
    创建于 2023年2月12日 江西省上饶市玉山县

*/

#pragma once

#include <lib/sys/types.h>

/**
 * 系统调用编号定义。
 */
enum class SystemCallId : int64_t {
    TEST_CALL = 0,
    WRITE = 1, 
    SLEEP = 2
};


#if 0
static void __check_size() {
    sizeof(SystemCallId::C);
}
#endif
