// SPDX-License-Identifier: MulanPSL-2.0

/*

    时钟中断

    创建于2023年1月21日 江西省上饶市玉山县

*/


#include <interrupt/ClockInterrupt.h>
#include <interrupt/ImplementHandlerMacros.h>
#include <crt/CRT.h>
#include <misc/io.h>
#include <machine/Machine.h>
#include <task/TaskManager.h>
#include <machine/apic/apic.h>

#include <lib/collections/LinkedList.h>
#include <lib/stddef.h>

namespace ClockInterrupt {

uint64_t jiffyCounter = 0;
uint64_t timePassedSinceBoot = 0;
int64_t interruptFrequency = WANTED_CLOCK_INTERRUPT_HZ;


/**
 * 休眠列表。
 */
LinkedList sleepingTaskList;

IMPLEMENT_EXCEPTION_ENTRANCE(entrance, ClockInterrupt::handler)

void wakeupSleptEnoughTasks() {
    LinkedListNode* currNode = sleepingTaskList.getFirst();
    while (currNode) {
        Task* task = (Task*) ((intptr_t) currNode - offsetof(Task, linkedListNode));
        if (task->wakeupMoment <= timePassedSinceBoot) {
            task->status = TaskStatus::READY;
            sleepingTaskList.removeFirst();
            currNode = sleepingTaskList.getFirst();
        } else {
            break;
        }
    }
}

void handler(
    InterruptSoftwareFrame* softwareRegs,
    InterruptHardwareFrame* hardwareRegs
) {
    jiffyCounter++;
    timePassedSinceBoot += 1000 / interruptFrequency;

    wakeupSleptEnoughTasks();

    // io::outByte(Machine::PIC_MASTER_CTRL, Machine::PIC_EOI); // 8259 chip
    machine::apic::localApicEOI();

    TaskManager::schedule();
}


void putToSleep(Task* task, uint64_t milliseconds) {
    if (milliseconds == 0) {
        return; // 逗我呢？不给睡！
    }

    task->wakeupMoment = timePassedSinceBoot + milliseconds;
    task->status = TaskStatus::BLOCKED;

    const auto insertJudge = [] (
        const int64_t cargo, LinkedListNode* before, LinkedListNode* after
    ) -> int {

        const auto targetTime = cargo;
        auto taskBefore = (Task*) ((intptr_t) before - offsetof(Task, linkedListNode));
        auto taskAfter = (Task*) ((intptr_t) after - offsetof(Task, linkedListNode));

        if (before && after) {

            if (taskBefore->wakeupMoment <= targetTime
                && taskAfter->wakeupMoment >= targetTime
            ) {
                return 0;
            } else {
                return 1;
            }

        } else if (before) {

            return 0;

        } else if (after) {

            if (taskAfter->wakeupMoment >= targetTime) {
                return 0;
            } else {
                return 1;
            }

        } else {
            return 0;
        }

    };

    sleepingTaskList.insertWhen(
        task->linkedListNode,
        task->wakeupMoment,
        insertJudge
    );

}


}

