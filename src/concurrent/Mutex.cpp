// SPDX-License-Identifier: MulanPSL-2.0

/*

    互斥量。

    创建于 2023年6月22日 上海市嘉定区安亭镇

*/

#include "./Mutex.h"

#include <machine/Machine.h>

namespace concurrent {


Mutex::Mutex() {
    this->value = false;
    // waiters 链表自己会初始化。
}

void Mutex::lock() {
    auto irq = Machine::getAndSetInterruptState(false);

    // todo

    Machine::setInterruptState(irq);
}

void Mutex::unlock() {
    auto irq = Machine::getAndSetInterruptState(false);

    this->value = false;
    this->waiters.forEach([] (LinkedListNode* node) {
      //todo  
    });

   // for (auto it : this->waiters) {

    //}

    // todo

    Machine::setInterruptState(irq);
}

bool Mutex::isLocked() {
    return this->value;
}

    
}