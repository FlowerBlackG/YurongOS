/*
 * 内核头文件。
 * 创建于 2022年7月1日
 */

#pragma once

class Kernel {
public:
    Kernel();
    static Kernel& getInstance() {
        return Kernel::instance;
    }
private:
    static Kernel instance;
};
