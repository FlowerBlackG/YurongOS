// SPDX-License-Identifier: MulanPSL-2.0

/*
 * 为 C 语言提供 bool 数据类型支持。
 */

#pragma once

#ifndef __cplusplus
    #ifndef __bool_true_false_are_defined
        #define bool _Bool
        #define true 1
        #define false 0
        #define __bool_true_false_are_defined 1
    #endif
#endif
