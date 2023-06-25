// SPDX-License-Identifier: MulanPSL-2.0

/*

    互斥量。

    创建于 2023年6月22日 上海市嘉定区安亭镇

*/

#include "./Mutex.h"

#include <machine/Machine.h>
#include <lib/stddef.h>
#include <task/Task.h>
#include <task/TaskManager.h>

namespace concurrent {


Mutex::Mutex() {
    this->occupied = false;
    // waiters 链表自己会初始化。
}

void Mutex::lock() {
    auto irq = Machine::getAndSetInterruptState(false);

    while (this->occupied) {
        auto pTask = TaskManager::getCurrentTask();
        this->waiters.append(&(pTask->linkedListNode));
        pTask->state = TaskState::BLOCKED;
        TaskManager::schedule();
        Machine::setInterruptState(false);
    }

    this->occupied = true;

    Machine::setInterruptState(irq);
}

void Mutex::unlock() {
    auto irq = Machine::getAndSetInterruptState(false);

    this->occupied = false;

    // 将等待队列里的所有进程设为就绪状态。
    for (auto& it : this->waiters) {
        auto pTask = (Task*) (intptr_t(&it) - offsetof(Task, linkedListNode));
        pTask->state = TaskState::READY;
    }

    this->waiters.clear();

    Machine::setInterruptState(irq);
}

bool Mutex::isLocked() {
    return this->occupied;
}

    
}