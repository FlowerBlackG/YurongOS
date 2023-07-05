// SPDX-License-Identifier: MulanPSL-2.0

/*
 * High Precision Event Timer
 *
 * signature "HPET"
 *
 * 创建于 2023年7月5日 上海市嘉定区安亭镇
 */

#include "./hpet.h"
#include <memory/memory.h>
#include <misc/Kernel.h>
#include <interrupt/ClockInterrupt.h>

namespace device {
namespace acpi {


HighPrecisionEventTimer* hpet = nullptr;


void initHighPrecisionEventTimer(SystemDescriptorTableHeader* table) {
    hpet = reinterpret_cast<decltype(hpet)>(table);

    if (!hpet) {
        return;
    }

    intptr_t ctrlAddr = memory::addrOfPhysicalMemoryMap + hpet->address.address;
    auto gcr = (HPETGeneralCapabilitiesAndIdRegister*) (ctrlAddr + HPETGeneralCapabilitiesAndIdRegister::regOffset);

    auto frequency = 1000000000000000LL / gcr->counterClkPeriod;
    Kernel::printLog(
        Kernel::LogColor::CYAN,
        "hpet frequency: %lld\n",
        frequency
    );

    // config timer0
    auto timer0Addr = ctrlAddr + 0x100;
    auto timer0Cfg = *(HPETTimerConfigAndCapabilityRegister*) timer0Addr;
    Kernel::printLog(
        Kernel::LogColor::CYAN,
        "hpet t0 cap: int route: 0x%x\n",
        timer0Cfg.intRouteCap
    );

    timer0Cfg.typeCnf = 1; // periodic timer
    timer0Cfg.intEnbCnf = 1;
    timer0Cfg.valSetCnf = 1;
    timer0Cfg.intRouteCnf = 16;

    *(HPETTimerConfigAndCapabilityRegister*) timer0Addr = timer0Cfg;



    // set timer0 comparator value reg
    auto compValue = frequency / ClockInterrupt::interruptFrequency;
    auto compValue0Addr = ctrlAddr + 0x108;
    *(int64_t volatile*) compValue0Addr = compValue;

    Kernel::printLog(
        Kernel::LogColor::CYAN,
        "hpet t0 comp: %lld\n",
        compValue
    );

    // 启用 hpet
    auto generalConfRegAddr = ctrlAddr + 0x10;
    *(int64_t volatile*) generalConfRegAddr = 0;
    *(int64_t*) (ctrlAddr + 0xF0) = 0;
    int64_t generalConfValue = 0x1; // 0b11: enable legacy replacement + enable cnf
    *(int64_t volatile*) generalConfRegAddr = generalConfValue;

}

}
}
