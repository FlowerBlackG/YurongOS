// SPDX-License-Identifier: MulanPSL-2.0

/*
 * Multiple APIC Description Table
 *
 * signature "APIC"
 *
 * 创建于 2023年7月3日 上海市嘉定区安亭镇
 */

#pragma once

#include "./acpi.h"

namespace device {
namespace acpi {

struct MADTRecord {
    uint8_t entryType;
    uint8_t length;

} __packed;

struct MADTRecordIterator {
    typedef MADTRecordIterator Self;
    typedef MADTRecord ElementType;

    MADTRecord* currentRecord;
    int64_t remainingLength;

    MADTRecordIterator(MADTRecord* firstRecord, int64_t recordsLength) {
        this->currentRecord = firstRecord;
        this->remainingLength = recordsLength;
    }

    ElementType& operator * () const { return *currentRecord; };

    Self& operator ++ () {
        auto addr = (intptr_t) currentRecord;
        addr += currentRecord->length;
        remainingLength -= currentRecord->length;
        currentRecord = (MADTRecord*) addr;
        return *this;
    };

    Self operator ++ (int) {
        auto tmp = *this;

        auto addr = (intptr_t) currentRecord;
        addr += currentRecord->length;
        remainingLength -= currentRecord->length;
        currentRecord = (MADTRecord*) addr;

        return tmp;
    }

    bool haveNext() {
        if (this->remainingLength <= 0) {
            return false;
        }

        return this->currentRecord->length != this->remainingLength;
    }

    friend bool operator == (const Self& a, const Self& b) {
        return a.currentRecord == b.currentRecord;
    }

    friend bool operator != (const Self& a, const Self& b) {
        return a.currentRecord != b.currentRecord;
    }

};

struct MADTRecordType0 : MADTRecord {
    uint8_t acpiProcessorId;
    uint8_t apicId;
    uint32_t flags;
} __packed;

typedef MADTRecordType0 MADTProcessorLocalAPICRecord;

struct MADTRecordType1 : MADTRecord {
     uint8_t ioApicId;
     uint8_t reserved;
     uint32_t ioApicAddr;
     uint32_t globalSystemInterruptBase;
} __packed;

typedef MADTRecordType1 MADTIOApicRecord;

struct MADTRecordType2 : MADTRecord {
    uint8_t busSource;
    uint8_t irqSource;
    uint32_t globalSystemInterrupt;
    uint16_t flags;
} __packed;

typedef MADTRecordType2 MADTIOApicInteruptSourceOverrideRecord;

struct MADTRecordType3 : MADTRecord {
    uint8_t nmiSource;
    uint8_t reserved;
    uint16_t flags;
    uint32_t globalSystemInterrupt;
} __packed;

typedef MADTRecordType3 MADTIOApicNMISourceRecord;

struct MADTRecordType4 : MADTRecord {
    uint8_t acpiProcessorId;
    uint16_t flags;
    uint8_t lint;
} __packed;

typedef MADTRecordType4 MADTLocalApicNMIsRecord;

struct MADTRecordType5 : MADTRecord {
    uint16_t reserved;
    uint64_t physicalAddrOfLocalApic;
} __packed;

typedef MADTRecordType5 MADTLocalApicAddrOverrideRecord;

struct MADTRecordType9 : MADTRecord {
    uint16_t reserved;
    uint32_t processorLocalX2ApicId;
    uint32_t flags;
    uint32_t acpiId;
} __packed;

typedef MADTRecordType9 MADTProcessorLocalX2ApicRecord;

struct MultipleAPICDescriptionTable {
    SystemDescriptorTableHeader sdtHeader;

    uint32_t localAPICAddress;
    uint32_t flags;

    // records followed.

    // ------ 迭代相关 ------

    MADTRecordIterator begin() const {
        intptr_t recordAddr = (intptr_t) this;
        int64_t lengthBeforeRecords = sizeof(sdtHeader) + sizeof(localAPICAddress) + sizeof(flags);
        recordAddr += lengthBeforeRecords;
        return MADTRecordIterator((MADTRecord*) recordAddr, sdtHeader.length - lengthBeforeRecords);
    }

    MADTRecordIterator end() const {
        MADTRecordIterator itr = this->begin();

        while (itr.haveNext()) {
            ++itr;
        }

        return ++itr;
    }

} __packed;


extern MultipleAPICDescriptionTable* madt;

void initMultipleAPICDescriptionTable(SystemDescriptorTableHeader* table);

}
}

