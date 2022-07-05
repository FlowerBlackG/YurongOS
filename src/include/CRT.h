/*
 * CRT 显示器控制头文件。
 * 创建于 2022年7月2日。
 */

#pragma once

class CRT {
public:


private:
    static const int ADDR_REG = 0x3d4;
    static const int DATA_REG = 0x3d5;
    struct Reg {
        static const int CURSOR_HIGH = 0xe;
        static const int CURSOR_LOW = 0xf;
    };
};
