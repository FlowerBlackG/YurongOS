/*
 * 系统内核。
 * 创建于 2022年7月1日
 */



char msg[] = "hello os!";

/**
 * 内核初始化。
 * 为防止 C++ 编译器将函数重命名，导致链接失败。
 * 
 */
extern "C" void kernel_init() {
    char* video = (char*) 0xb8000;
    for (int i = 0; i < sizeof(msg); i++) {
        video[i * 2] = msg[i];
    }
}
