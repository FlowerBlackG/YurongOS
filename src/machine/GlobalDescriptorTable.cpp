// SPDX-License-Identifier: MulanPSL-2.0

/*
 * 全局描述符表相关内容。
 * 包含段选择子等。
 * 创建于 2022年7月15日。
 */

#include <machine/GlobalDescriptorTable.h>
#include <memory/MemoryManager.h>
#include <lib/string.h>
#include <lib/stdio.h>
#include <crt/CRT.h>

namespace GlobalDescriptorTable {

    Descriptors descriptors;
    TaskStateSegment taskStateSegment;

    void __initLongModeSegmentDescriptors(
        SegmentDescriptor& codeDesc,
        SegmentDescriptor& dataDesc,
        int privilegeLevel
    ) {

        const auto commonProcess = [] (SegmentDescriptor& desc, int privilegeLevel) {

            desc.limitHigh = 0;
            desc.limitLow = 0;
            
            desc.baseHigh = 0;
            desc.baseMid = 0;
            desc.baseLow = 0;

            desc.rw = 1;
            desc.access = 0;
            desc.dc = 0;

            desc.present = 1;
            desc.sizeFlag = 0;
            desc.granularity = 0;
            
            desc.descriptorType = SegmentDescriptor::SEGMENT_TYPE_CODE_DATA;
            desc.privilegeLevel = privilegeLevel & 3;
        };

        commonProcess(codeDesc, privilegeLevel);
        commonProcess(dataDesc, privilegeLevel);

        codeDesc.executable = 1;
        dataDesc.executable = 0;

        codeDesc.longMode = 1;
        dataDesc.longMode = 0;

        codeDesc.reserved = 0;
        dataDesc.reserved = 0;
    }

    void __initProtectedModeUserCodeSegmentDescriptor(
        SegmentDescriptor& desc
    ) {
        desc.limitHigh = 0xF;
        desc.limitLow = 0xFFFF;

        desc.baseHigh = 0;
        desc.baseMid = 0;
        desc.baseLow = 0;

        desc.executable = 1;
        desc.descriptorType = SegmentDescriptor::SEGMENT_TYPE_CODE_DATA;
        desc.rw = 1;
        desc.access = 0;
        desc.sizeFlag = 1;
        desc.privilegeLevel = 3;
        desc.present = 1;
        desc.dc = 0;
        desc.granularity = 1;
        desc.longMode = 0;
        desc.reserved = 0;
    }

    void init() {

        memset(&descriptors.zero, 0, sizeof(descriptors.zero));

        __initLongModeSegmentDescriptors(descriptors.kernelCode, descriptors.kernelData, 0);
        __initLongModeSegmentDescriptors(descriptors.userCode64, descriptors.userData, 3);
        __initProtectedModeUserCodeSegmentDescriptor(descriptors.userCode32);

        auto& sysSegmentDesc = descriptors.systemSegmentDescriptor;
        unsigned long tssAddr = (unsigned long) &taskStateSegment;
        const int tssLimit = sizeof(taskStateSegment) - 1;
        
        sysSegmentDesc.baseLow = tssAddr & 0xffff;
        sysSegmentDesc.baseMid = (tssAddr >> 16) & 0xff;
        sysSegmentDesc.baseHigh = (tssAddr >> 24) & 0xff;
        sysSegmentDesc.baseUltraHigh = ((tssAddr >> 16) >> 16) & 0xffffffffULL;

        sysSegmentDesc.limitLow = tssLimit & 0xffff;
        sysSegmentDesc.limitHigh = (tssLimit >> 16) & 0xf;

        sysSegmentDesc.systemSegmentType = SystemSegmentDescriptor::SYSTEM_SEGMENT_TYPE_64_BIT_AVAILABLE;
        sysSegmentDesc.descriptorType = SystemSegmentDescriptor::SEGMENT_TYPE_SYSTEM;
        sysSegmentDesc.privilegeLevel = 0;
        sysSegmentDesc.present = 1;
        sysSegmentDesc.longMode = 0;
        sysSegmentDesc.sizeFlag = 0;
        sysSegmentDesc.granularity = 0;

        sysSegmentDesc.reserved0 = 0;
        sysSegmentDesc.reserved1 = 0;

        // 将原来的 gdt 换成内核代码里设置的 gdt。

        GdtRegister gdtr;

        gdtr.baseAddress = uint64_t(&descriptors);
        gdtr.limit = sizeof(descriptors) - 1;

        GlobalDescriptorTable::loadGdt(gdtr);
        CRT::getInstance().write("[info] gdt reloaded.\n");

        initTaskStateSegment();
    }

    void initTaskStateSegment() {
        auto& tss = taskStateSegment;
        memset(&tss, 0, sizeof(tss));

        __asm (
            "ltr %%ax"
            :
            : "a" (TASK_STATE_SEGMENT_SELECTOR)
        );

        CRT::getInstance().write("[info] task state segment loaded.\n");
    }

    void storeGdt(GdtRegister& gdtr) {
        __asm (
            "sgdt %0"
            : "=m" (gdtr)
        );
    }

    void loadGdt(const GdtRegister& gdtr) {
        __asm (
            "lgdt %0"
            : /* null */
            : "m" (gdtr)
        );
    }

}
