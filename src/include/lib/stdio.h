/*
 * 标准输入输出头文件。
 * 创建于 2022年7月13日。
 * 
 * 参考：
 *   https://pubs.opengroup.org/onlinepubs/9699919799/
 *   https://cplusplus.com/reference/cstdio/sprintf/
 *   https://cplusplus.com/reference/cstdio/vsprintf/
 *   https://cplusplus.com/reference/cstdio/printf/
 */

#pragma once

#include <lib/sys/types.h>
#include <lib/stdarg.h>

// 文件结尾标示值。
#ifndef EOF
    #define EOF -1
#endif

/*
  格式化输出的格式：
    %[flags][width][.precision][length]specifier

  flag:
    -    空间内左对齐（宽度过大时）。默认右对齐。
    +    强制对数字加入正负号。
    空格  对不需要加符号的数字，输出一个空格。
    #    当输出格式为8/16进制时，决定是否前缀0/0x/0X。
         当输出格式为浮点时，强制输出点号。
    0    用前缀 0 填充多的位置。

  width:
    数字 最小字符输出长度。
    *   由当前指向参数（int）指定。下一个参数作为输出目标。

  .precision:
    .数字
    .*    由当前指向参数（int）指定。下一个参数作为输出目标。

  specifier:
    d/i 有符号10进制整数。
    u   无符号10进制整数。
    o   无符号8进制整数。
    x   无符号16进制整数（7fa）。
    X   无符号16进制整数（字母大写）。
    f   10进制浮点数。
    F   10进制浮点数（大写）。
    e   科学计数法（3.9265e+2）。
    E   科学计数法（大写）（3.9265E+2）。
    g   使用 %e 和 %f 中较短的。
    G   使用 %E 和 %F 中较短的。
    a   16进制浮点数。
    A   16进制浮点数（大写）。
    c   字符。
    s   字符串。
    p   指针地址（b8000）。
    n   不输出任何东西。对应参数必须是有符号整数（signed int）。
    %   输出百分号 % 本身。

  length specifier:
    可以改变数据长度
      hh + 整数类型/n   char
      h  + 整数类型/n   short
      l  + 整数类型/ncs long
      ll + 整数类型/n   long long
      j  + 整数类型/n   intmax_t / uintmax_t
      z  + 整数类型/n   size_t
      t  + 整数类型/n   ptrdiff_t         *暂未实现
      L  + 浮点类型     long double

*/

/**
 * 将格式化数据写入字符串。
 */
int sprintf(char* buffer, const char* format, ...);

/**
 * 将来自可变参数表的格式化数据写入字符串。
 * 
 * 格式错误处理逻辑：直接返回 -1。
 *     当最后一个字符是单独的 % 时，可能不会返回错误，但不输出 %。
 * 格式非法错误逻辑：
 *     width 超出 int 上界：未定义的行为（循环乘10，自然溢出）。
 * 
 * @todo 暂时无法处理浮点数据类型。
 * @todo 暂不支持 %n的处理。
 */
int vsprintf(char* buffer, const char* format, va_list args);
