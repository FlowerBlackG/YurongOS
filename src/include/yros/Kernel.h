/*
 * 内核头文件。
 * 创建于 2022年7月1日
 */

#pragma once

class Kernel {
private:
    Kernel();
public:
    static Kernel& getInstance() {
        return Kernel::instance;
    }

    void main();

    void panic(const char* s);
private:
    static Kernel instance;
};
