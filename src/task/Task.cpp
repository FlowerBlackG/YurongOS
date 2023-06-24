// SPDX-License-Identifier: MulanPSL-2.0

/*

    任务结构

    创建于2023年6月22日 上海市嘉定区安亭镇

*/

#include "./Task.h"

#include <interrupt/ClockInterrupt.h>

void Task::putToSleep(uint64_t milliseconds) {

    ClockInterrupt::putToSleep(this, milliseconds);

}

