/*
 * 标准输入输出实现。
 * 创建于 2022年7月13日。
 * 
 * 参考：
 *   https://pubs.opengroup.org/onlinepubs/9699919799/
 *   https://cplusplus.com/reference/cstdio/sprintf/
 *   https://cplusplus.com/reference/cstdio/vsprintf/
 *   https://cplusplus.com/reference/cstdio/printf/
 */

#include <lib/stdio.h>
#include <lib/string.h>

int sprintf(char* buffer, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int res = vsprintf(buffer, format, args);
    va_end(args);
    return res;
}

int vsprintf(char* buffer, const char* format, va_list args) {
    
    // %[flags][width][.precision][length]specifier

    // flag 定义。
#define __VPF_FLAG_NULL 0
#define __VPF_FLAG_ZERO_BIT 1
#define __VPF_FLAG_SPACE_BIT 2
#define __VPF_FLAG_PLUS_BIT 4
#define __VPF_FLAG_MINUS_BIT 8
#define __VPF_FLAG_NUMBER_SIGN_BIT 16

    // 长度限定符定义。
#define __VPF_LENGTH_NULL 0
#define __VPF_LENGTH_CHAR 1
#define __VPF_LENGTH_SHORT 2
#define __VPF_LENGTH_LONG 3
#define __VPF_LENGTH_LONG_LONG 4
#define __VPF_LENGTH_INT_MAX 5
#define __VPF_LENGTH_SIZE_T 6
#define __VPF_LENGTH_PTRDIFF_T 7
#define __VPF_LENGTH_LONG_DOUBLE 8

    // 精度。
#define __VPF_PRECISION_NULL -1

    // 宽度。
#define __VPF_WIDTH_NULL -1


    char* pBuf = buffer;
    const char* pFmt = format;

    const char* pPrecisionSign;

    uint8_t lengthSpecifier;
    uint8_t flag;

    int width;
    int precision;

    // 工具闭包函数。
    // 拼接一个16进制数。自动补充对齐等。
    auto catHex = [&] (uint64_t hexVal, size_t nbytes, bool upper) {

        char tmpStr[32];
        char* pTmpStr = tmpStr;
        size_t nHalfBytes = 2 * nbytes;

        if (hexVal == 0) {
            *(pTmpStr++) = '0';
        } else while (hexVal > 0 && (nHalfBytes--) > 0) {
            uint8_t digit = hexVal & 0xf;
            hexVal >>= 4;

            if (digit <= 9) {
                *(pTmpStr++) = digit + '0';
            } else {
                *(pTmpStr++) = digit - 10 + 'a';
            }
        }

        int32_t len = pTmpStr - tmpStr;
        int32_t paddingLen = precision > width ? precision : width;
        if (paddingLen < 0) {
            paddingLen = 0;
        }

        paddingLen -= len;
        if (paddingLen < 0) {
            paddingLen = 0;
        }

        if (paddingLen > 0 && !(flag & __VPF_FLAG_MINUS_BIT)) {
            memset(
                pBuf,
                ((flag & __VPF_FLAG_ZERO_BIT) ? '0' : ' '),
                paddingLen
            );
            pBuf += paddingLen;
        }

        if (flag & __VPF_FLAG_NUMBER_SIGN_BIT) {
            *(pBuf++) = '0';
            *(pBuf++) = upper ? 'X' : 'x';
        }
        
        while (pTmpStr > tmpStr) {
            *(pBuf++) = *(--pTmpStr);
        }

        if (paddingLen > 0 && (flag & __VPF_FLAG_MINUS_BIT)) {
            memset(pBuf, ' ', paddingLen);
            pBuf += paddingLen;
        }
    };

    while (*pFmt != NULL) {

        /* ------------ 百分号 %. ------------ */

        if (*pFmt == '%') {
            pPrecisionSign = pFmt++;

            lengthSpecifier = __VPF_LENGTH_NULL;
            flag = __VPF_FLAG_NULL;
            precision = __VPF_PRECISION_NULL;
            width = __VPF_WIDTH_NULL;
        } else {
            *(pBuf++) = *(pFmt++);
            continue;
        }


        /* ------------ flag. ------------ */

        while (true) {
            if (*pFmt == '-') {
                flag |= __VPF_FLAG_MINUS_BIT;
                ++pFmt;
            } else if (*pFmt == '+') {
                flag |= __VPF_FLAG_PLUS_BIT;
                ++pFmt;
            } else if (*pFmt == ' ') {
                flag |= __VPF_FLAG_SPACE_BIT;
                ++pFmt;
            } else if (*pFmt == '#') {
                flag |= __VPF_FLAG_NUMBER_SIGN_BIT;
                ++pFmt;
            } else if (*pFmt == '0') {
                flag |= __VPF_FLAG_ZERO_BIT;
                ++pFmt;
            } else {
                break;
            }
        }


        /* ------------ width. ------------ */

        if (*pFmt == '*') {
            width = va_arg(args, int);
            ++pFmt;
        } else if (*pFmt >= '0' && *pFmt <= '9') {
            width = *(pFmt++) - '0';
            while (*pFmt >= '0' && *pFmt <= '9') {
                width *= 10;
                width += *(pFmt++) - '0';
            }

        }


        /* ------------ precision. ------------ */

        if (*pFmt == '.') {
            ++pFmt;

            if (*pFmt == '*') {
                precision = va_arg(args, int);
                ++pFmt;
            } else if (*pFmt >= '0' && *pFmt <= '9') {
                precision = *(pFmt++) - '0';
                while (*pFmt >= '0' && *pFmt <= '9') {
                    precision *= 10;
                    precision += *(pFmt++) - '0';
                }
            } else {
                // 格式解析异常。
                return -1;
            }
        }


        /* ------------ 长度限定符。 ------------ */

        if (*pFmt == 'h' && *(pFmt + 1) == 'h') {
            pFmt += 2;
            lengthSpecifier = __VPF_LENGTH_CHAR;
        } else if (*pFmt == 'h') {
            pFmt += 1;
            lengthSpecifier = __VPF_LENGTH_SHORT;
        } else if (*pFmt == 'l' && *(pFmt + 1) == 'l') {
            pFmt += 2;
            lengthSpecifier = __VPF_LENGTH_LONG_LONG;
        } else if (*pFmt == 'l') {
            pFmt += 1;
            lengthSpecifier = __VPF_LENGTH_LONG;
        } else if (*pFmt == 'j') {
            pFmt += 1;
            lengthSpecifier = __VPF_LENGTH_INT_MAX;
        } else if (*pFmt == 'z') {
            pFmt += 1;
            lengthSpecifier = __VPF_LENGTH_SIZE_T;
        } else if (*pFmt == 't') {
            pFmt += 1;
            lengthSpecifier = __VPF_LENGTH_PTRDIFF_T;
        } else if (*pFmt == 'L') {
            pFmt += 1;
            lengthSpecifier = __VPF_LENGTH_LONG_DOUBLE;
        }


        /* ------------ 类型限定符。 ------------ */

        char specifier = *(pFmt++);
        
        switch (specifier) {
            case '%':
                *(pBuf++) = '%';
                continue;

            // 整数系列。

            case 'd':
            case 'i': 
            case 'u':
            case 'o': {
                char tmp[64];
                char* pTmp = tmp;

                uint8_t base = (specifier == 'o' ? 8 : 10);
                uint64_t mask;
                uint64_t val;

                if (lengthSpecifier == __VPF_LENGTH_CHAR) {
                    mask = (1ULL << (sizeof(char) * 8)) - 1;
                    val = va_arg(args, int) & mask;
                } else if (lengthSpecifier == __VPF_LENGTH_SHORT) {
                    mask = (1ULL << (sizeof(short) * 8)) - 1;
                    val = va_arg(args, int) & mask;
                } else if (lengthSpecifier == __VPF_LENGTH_LONG) {
                    mask = 1ULL;
                    mask <<= sizeof(long) * 4ULL;
                    mask <<= sizeof(long) * 4ULL;
                    mask--;
                    val = va_arg(args, long) & mask;
                } else if (lengthSpecifier == __VPF_LENGTH_LONG_LONG) {
                    mask = 1ULL;
                    mask <<= sizeof(long long) * 4ULL;
                    mask <<= sizeof(long long) * 4ULL;
                    mask--;
                    
                    val = va_arg(args, long long) & mask;
                } else if (lengthSpecifier == __VPF_LENGTH_INT_MAX) {
                    
                    mask = 1ULL;
                    mask <<= sizeof(intmax_t) * 4ULL;
                    mask <<= sizeof(intmax_t) * 4ULL;
                    mask--;
                    val = va_arg(args, intmax_t) & mask;
                } else if (lengthSpecifier == __VPF_LENGTH_SIZE_T) {
                    mask = 1ULL;
                    mask <<= sizeof(size_t) * 4ULL;
                    mask <<= sizeof(size_t) * 4ULL;
                    mask--;
                    val = va_arg(args, size_t) & mask;
                } else {
                    mask = (1ULL << (sizeof(int) * 8)) - 1;
                    val = va_arg(args, int) & mask;
                }

                char sign = NULL;

                // 判断是否为负数。
                if (specifier == 'd' || specifier == 'i') {
                    if (
                        (lengthSpecifier == __VPF_LENGTH_CHAR && (char) val < 0)
                        || (lengthSpecifier == __VPF_LENGTH_SHORT && (short) val < 0)
                        || (lengthSpecifier == __VPF_LENGTH_LONG && (long) val < 0)
                        || (lengthSpecifier == __VPF_LENGTH_LONG_LONG && (long long) val < 0)
                        || (lengthSpecifier == __VPF_LENGTH_INT_MAX && (intmax_t) val < 0)
                        || (lengthSpecifier == __VPF_LENGTH_SIZE_T && (size_t) val < 0)
                        || (lengthSpecifier == __VPF_LENGTH_NULL && (int) val < 0)
                    ) {
                        sign = '-';
                    }
                }
                
                // 非负情况，决定是否要强制设置符号。
                if (sign == NULL) {
                    if (flag & __VPF_FLAG_SPACE_BIT) {
                        sign = ' ';
                    } else if (flag & __VPF_FLAG_PLUS_BIT) {
                        sign = '+';
                    }
                }

                // 提取各位数字。
                if (val == 0) {
                    *(pTmp++) = '0';
                } else while (val > 0) {
                    *(pTmp++) = (val % base) + '0';
                    val /= base;
                }

                // 8进制补充前缀0。
                if ((flag & __VPF_FLAG_NUMBER_SIGN_BIT) && specifier == 'o') {
                    *(pTmp++) = '0';
                }

                // 数字串长度。
                int32_t len = pTmp - tmp;

                // 填充串长度。符号算在填充串内。
                int32_t paddingLen = (precision > width ? precision : width);
                paddingLen = (paddingLen > 0 ? paddingLen : 0);

                paddingLen -= len;

                paddingLen = (paddingLen > 0 ? paddingLen : 0);

                // 输出前缀填充。
                if (paddingLen && (flag & __VPF_FLAG_ZERO_BIT || !(flag & __VPF_FLAG_MINUS_BIT)))
                {

                    memset(
                        pBuf,
                        (flag & __VPF_FLAG_ZERO_BIT ? '0' : ' '),
                        paddingLen
                    );

                    if (sign) {
                        if (flag & __VPF_FLAG_ZERO_BIT) {
                            *(pBuf++) = sign;
                        } else {
                            *(pBuf + paddingLen - 1) = sign;
                        }
                    }

                    pBuf += paddingLen;
                } else if (sign) {
                    *(pBuf++) = sign;
                }

                // 输出数字。
                while (pTmp > tmp) {
                    *(pBuf++) = *(--pTmp);
                }

                // 后缀填充空格。
                if (paddingLen && !(flag & __VPF_FLAG_ZERO_BIT) && flag & __VPF_FLAG_MINUS_BIT)
                {
                    int32_t len = paddingLen - !!(sign != NULL);
                    memset(pBuf, ' ', len);
                    pBuf += len;
                }

                break;
            }
        
            // 16 进制无符号整数。
            case 'x':
            case 'X': {
                if (lengthSpecifier == __VPF_LENGTH_CHAR) {
                    catHex(va_arg(args, int), sizeof(char), (specifier == 'X'));
                } else if (lengthSpecifier == __VPF_LENGTH_SHORT) {
                    catHex(va_arg(args, int), sizeof(short), (specifier == 'X'));
                } else if (lengthSpecifier == __VPF_LENGTH_LONG) {
                    catHex(va_arg(args, long), sizeof(long), (specifier == 'X'));
                } else if (lengthSpecifier == __VPF_LENGTH_LONG_LONG) {
                    catHex(va_arg(args, long long), sizeof(long long), (specifier == 'X'));
                } else if (lengthSpecifier == __VPF_LENGTH_INT_MAX) {
                    catHex(va_arg(args, intmax_t), sizeof(intmax_t), (specifier == 'X'));
                } else if (lengthSpecifier == __VPF_LENGTH_SIZE_T) {
                    catHex(va_arg(args, size_t), sizeof(size_t), (specifier == 'X'));
                } else {
                    catHex(va_arg(args, int), sizeof(int), (specifier == 'X'));
                }

                break;
            }


            // 浮点系列。
            
            // 浮点系列未实现。 todo.

            // char。
            // lc 形式未实现。todo.
            case 'c': {
                int ch = va_arg(args, int);

                if (!(precision & __VPF_FLAG_MINUS_BIT) && width > 1) {
                    memset(pBuf, ' ', width - 1);
                    pBuf += width - 1;
                }

                *(pBuf++) = (char) ch;

                if ((precision & __VPF_FLAG_MINUS_BIT) && width > 1) {
                    memset(pBuf, ' ', width - 1);
                    pBuf += width - 1;
                }

                break;
            }

            // 字符串。
            case 's': {
                char* spStr = va_arg(args, char*);
                int32_t sLen = strlen(spStr);

                int32_t sRealLen;
                if (precision == __VPF_PRECISION_NULL || sLen <= precision) {
                    sRealLen = sLen;
                } else {
                    sRealLen = precision;
                }

                int32_t sPaddingLen;
                if (width == __VPF_WIDTH_NULL || width <= sRealLen) {
                    sPaddingLen = 0;
                } else {
                    sPaddingLen = width - sRealLen;
                }

                if (sPaddingLen == 0 || (flag & __VPF_FLAG_MINUS_BIT)) {
                    memcpy(pBuf, spStr, sRealLen);
                    pBuf += sRealLen;
                }

                memset(pBuf, ' ', sPaddingLen);
                pBuf += sPaddingLen;

                if (sPaddingLen > 0 && !(flag & __VPF_FLAG_MINUS_BIT)) {
                    memcpy(pBuf, spStr, sRealLen);
                    pBuf += sRealLen;
                }

                break;
            }

            // 指针。
            case 'p': {
                void* pointer = va_arg(args, void*);
                uint64_t ui64pointer = (uint64_t) pointer;
                catHex(ui64pointer, sizeof(ui64pointer), false);  
                break;
            } 

            // 空。
            case 'n': //todo
                break;

            default:
                // 格式解析异常。
                return -1;

        } // switch (specifier)


    } // while (*pFmt != NULL)

    *pBuf = '\0';
    return pBuf - buffer;

    // 取消 flag 定义，防止干扰后续操作。
#undef __VPF_FLAG_NULL
#undef __VPF_FLAG_ZERO_BIT
#undef __VPF_FLAG_SPACE_BIT
#undef __VPF_FLAG_PLUS_BIT
#undef __VPF_FLAG_MINUS_BIT
#undef __VPF_FLAG_NUMBER_SIGN_BIT

    // 取消长度限定符定义。
#undef __VPF_LENGTH_NULL
#undef __VPF_LENGTH_CHAR
#undef __VPF_LENGTH_SHORT
#undef __VPF_LENGTH_LONG
#undef __VPF_LENGTH_LONG_LONG
#undef __VPF_LENGTH_INT_MAX
#undef __VPF_LENGTH_SIZE_T
#undef __VPF_LENGTH_PTRDIFF_T
#undef __VPF_LENGTH_LONG_DOUBLE

    // 取消精度定义。
#undef __VPF_PRECISION_NULL

    // 取消宽度定义。
#undef __VPF_WIDTH_NULL

} // int vsprintf(char* buffer, const char* format, va_list args)
