// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Multiple APIC Description Table
 *
 * signature "APIC"
 *
 * 创建于 2023年7月3日 上海市嘉定区安亭镇
 */

#include "./madt.h"
#include <crt/CRT.h>
#include <lib/stdio.h>
#include <machine/X86Assembly.h>
#include <machine/apic/apic.h>
#include <memory/memory.h>

namespace device {
namespace acpi {

MultipleAPICDescriptionTable* madt = nullptr;


static void processMADTRecordType0(MADTRecordType0* record) {
    // todo
}


static void processMADTRecordType1(MADTRecordType1* record) {
    // io apic

    static bool processed = false;

    if (processed) {
        return; // todo: 暂时只支持1个ioapic.
    }

    processed = true;

    auto ioApic = new machine::apic::IOApic(
        record->ioApicId, record->ioApicAddr, record->globalSystemInterruptBase
    );

    machine::apic::ioApicList.push(ioApic);

}

static void processMADTRecordType2(MADTRecordType2* record) {
    // todo
}


void initMultipleAPICDescriptionTable(SystemDescriptorTableHeader* table) {
    madt = reinterpret_cast<decltype(madt)>(table);

    if (!madt) {
        return;
    }

    char s[128];

    for (auto& record : *madt) {

        Kernel::printLog(
            Kernel::LogColor::YELLOW,
            "(0x%llx) MADT Record: len=%d, type=%d\n",
            &record, record.length, record.entryType
        );

        if (record.entryType == 0) {
            auto t0 = (MADTRecordType0*) &record;
            sprintf(s, "    -> procId=%d, apicId=%d\n",
                t0->acpiProcessorId, t0->apicId
            );
            processMADTRecordType0(t0);
        } else if (record.entryType == 1) {
            auto t1 = (MADTRecordType1*) &record;
            sprintf(s, "    -> ioAPICId=%d, ioAPICAddr=%x, gsib=%x\n",
                t1->ioApicId, t1->ioApicAddr, t1->globalSystemInterruptBase
            );
            processMADTRecordType1(t1);
        } else if (record.entryType == 2) {
            auto t2 = (MADTRecordType2*) &record;
            sprintf(s, "    -> busSrc=%d, irqSrc=%d, gsi=%x\n",
                t2->busSource, t2->irqSource, t2->globalSystemInterrupt
            );
            processMADTRecordType2(t2);
        } else {
            sprintf(s, "    -> ignored\n");
        }

        CRT::getInstance().write(s);

    }

    machine::apic::initLocalAPIC(
        madt->localAPICAddress
    );
}

}
}

