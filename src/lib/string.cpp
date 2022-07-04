/*
 * 字符串处理函数。
 * 创建于 2022年7月4日。
 */

#include <string.h>

char* strcpy(char* dest, const char* src) {
    char* pDest = dest;
    const char* pSrc = src;
    while ((*pDest++ = *pSrc++) != '\0')
        ;

    return dest;
}

char* strncpy(char* dest, const char* src, size_t count) {
    char* pDest = dest;
    const char* pSrc = src;

    while (pDest - (char*) dest < count) {
        *pDest = *pSrc;
        if (*pDest == '\0') {
            break;
        } else {
            ++pDest;
            ++pSrc;
        }
    }

    *pDest = '\0';

    return dest;
}

char* strcat(char* dest, const char* src) {
    strcpy(dest + strlen(dest), src);
    return dest;
}

char* strncat(char* dest, const char* src, size_t count) {
    strncpy(dest + strlen(dest), src, count);
    return dest;
}

size_t strlen(const char* str) {
    // 参考 glibc 的实现方式。
    // 解释见：https://blog.csdn.net/m0_62405272/article/details/125600719
    
    /* 先将指针与字节对齐。
       将指针转换成 long 这种操作在有些平台会报错。注意，不等号的优先级高于与运算。 */
    const char* charPtr = str;
    while (((long) charPtr & (sizeof(long) - 1)) != 0) {
        if (*charPtr == '\0') {
            return charPtr - str;
        } else {
            ++charPtr;
        }
    }

    const long* longPtr = (const long*) charPtr;

    long highMagic = 0x80808080L;
    long lowMagic = 0x01010101L;
    if (sizeof(long) > 4) {
        highMagic = ((highMagic << 16) << 16) | highMagic;
        lowMagic = ((lowMagic << 16) << 16) | lowMagic;
    }
    // 暂不考虑 long 的宽度超过 8 字节的情况。

    while (true) {
        const long& longWords = *longPtr++;
        if (((longWords - lowMagic) & ~longWords & highMagic) != 0) {
            const char* p = (const char*) longPtr;
            while (p - (const char*) longPtr < sizeof(long)) {
                if (*p == '\0') {
                    return p - str;
                } else {
                    ++p;
                }
            }
        }
    }
}

int strcmp(const char* lhs, const char* rhs) {
    const char* pLhs = lhs;
    const char* pRhs = rhs;

    while (*pLhs == *pRhs) {
        if (*pLhs == '\0') {
            return 0;
        } else {
            ++pLhs;
            ++pRhs;
        }
    }

    return *(const unsigned char*) pLhs - *(const unsigned char*) pRhs;
}

int strncmp(const char* lhs, const char* rhs, size_t count) {
    const char* pLhs = lhs;
    const char* pRhs = rhs;

    while (pLhs - lhs < count) {
        if (*pLhs != *pRhs) {
            return *(const unsigned char*) pLhs - *(const unsigned char*) pRhs;
        } 
        else if (*pLhs == '\0') {
            return 0;
        } 
        else {
            ++pLhs;
            ++pRhs;
        }
    }

    return 0;
}

char* strchr(const char* str, int ch) {
    char b = (char) ch;
    const char* p = str;
    while (*p != '\0') {
        if (b == *p) {
            return (char*) p;
        } else {
            ++p;
        }
    }

    return NULL;
}

char* strrchr(const char* str, int ch) {
    char b = (char) ch;
    const char* p = str + strlen(str) - 1;

    while (str <= p) {
        if (b == *p) {
            return (char*) p;
        } else {
            --p;
        }
    }

    return NULL;
}

size_t strspn(const char* dest, const char* src) {
    bool tar[256]; // 暂不考虑对 C 语言的兼容问题。
    memset(tar, 0, sizeof(tar));

    const unsigned char* p = (const unsigned char*) src;
    while (*p != '\0') {
        tar[*p++] = true;
    }

    p = (const unsigned char*) dest;
    while (tar[*p++])
        ;
    
    return p - (const unsigned char*) dest - 1;
}

size_t strcspn(const char* dest, const char* src) {
    bool tar[256]; // 暂不考虑对 C 语言的兼容问题。
    memset(tar, 0xff, sizeof(tar));

    const unsigned char* p = (const unsigned char*) src;
    while (*p != '\0') {
        tar[*p++] = false;
    }

    p = (const unsigned char*) dest;
    while (tar[*p++])
        ;
    
    return p - (const unsigned char*) dest - 1;
}

char* strpbrk(const char* dest, const char* breakset) {
    bool tar[256];
    memset(tar, 0, sizeof(tar));
;
    const unsigned char* p = (const unsigned char*) breakset;
    while (*p != '\0') {
        tar[*p++] = true;
    }

    p = (const unsigned char*) dest;
    while (*p != '\0') {
        if (tar[*p]) {
            return (char*) p;
        } else {
            ++p;
        }
    }

    return NULL;
}

char* strstr(const char* str, const char* substr) {
    size_t lenOfSubstr = strlen(substr);
    size_t lenOfStr = strlen(str);
    const char* pStr = str;
    while (lenOfSubstr + (pStr - str) <= lenOfStr) {
        if (strncmp(pStr, substr, lenOfSubstr) == 0) {
            return (char*) pStr;
        } else {
            ++pStr;
        }
    }

    // 注意：现在这种算法效率很低。可以考虑研究更好的算法。
}

char* strtok(char* str, const char* delim) {
    bool tar[256];
    memset(tar, 0, sizeof(tar));

    const unsigned char* p = (const unsigned char*) delim;
    while (*p != 0) {
        tar[*p++] = true;
    }

    static unsigned char* searchPtr = (unsigned char*) str;
    if (str != NULL) {
        searchPtr = (unsigned char*) str;
    }

    char* pToken = NULL;
    while (*str != '\0') {
        if (p[*str]) {
            if (pToken != NULL) {
                *str = '\0';
                ++str;
                return pToken;
            }
        } else if (pToken == NULL) {
            pToken = (char*) str;
        }

        ++str;
    }

    return NULL;
}

void* memchr(const void* ptr, int ch, size_t count) {
    const unsigned char* cPtr = (const unsigned char*) ptr;
    unsigned char b = ch;
    while (cPtr - (const unsigned char*) ptr < count) {
        if (b == *cPtr) {
            return (void*) cPtr;
        } else {
            ++cPtr;
        }
    }

    return NULL;
}

int memcmp(const void* lhs, const void* rhs, size_t count) {
    const unsigned char* pLhs = (const unsigned char*) lhs;
    const unsigned char* pRhs = (const unsigned char*) rhs;
    while (pRhs - (const unsigned char*) rhs < count) {
        if (*pLhs != *pRhs) {
            return *pLhs - *pRhs;
        } else {
            ++pLhs;
            ++pRhs;
        }
    }

    return 0;
}

void* memset(void* dest, int ch, size_t count) {
    char b = (char) ch;
    char* p = (char*) dest;
    while (p - (char*) dest < count) {
        *p++ = b;
    }

    return dest;
}

void* memcpy(void* dest, const void* src, size_t count) {
    char* pDest = (char*) dest;
    char* pSrc = (char*) src;
    while (pSrc - (char*) src < count) {
        *pDest++ = *pSrc++;
    }

    return dest;
}
