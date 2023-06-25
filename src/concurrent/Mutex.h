// SPDX-License-Identifier: MulanPSL-2.0

/*

    互斥量。

    创建于 2023年6月22日 上海市嘉定区安亭镇

*/

#pragma once

#include <lib/collections/LinkedList.h>

namespace concurrent {

class Mutex {

public:
    Mutex();
    
    void lock();
    
    void unlock();
    
    bool isLocked();

    bool isUnlocked() {
        return !this->isLocked();
    };

protected:

    /**
     * false 表示未被占用。
     * true 表示被占用。
     */
    bool occupied;
    LinkedList waiters;
};



}