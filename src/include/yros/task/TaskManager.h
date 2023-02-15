/*

    任务管理器

    创建于2023年2月6日 江西省上饶市玉山县

*/

#pragma once

#include <yros/task/Task.h>

namespace TaskManager {

    const int TASK_TABLE_SIZE = 256;

    extern Task* taskTable[TASK_TABLE_SIZE];

    void init();

    Task* getCurrentTask();
    
    void schedule();

    void loadTaskToCargo(Task* task);

    void stage(Task* task);

    /**
     * 从当前任务切换到下一任务。
     * 函数实现见 TaskManagerSwitchTo.asm
     * 
     * @param next 下一任务。
     */
    void switchTo(Task* next);

    Task* create(
        void (* target) (), 
        const char* name, 
        bool createTaskFrame = true,
        bool kernelProcess = false
    );

    void putToSleep(Task* task, uint64_t milliseconds);
}

#if 0

static void __mini_lab() {
    auto x = TaskManager::schedule;
}

#endif
