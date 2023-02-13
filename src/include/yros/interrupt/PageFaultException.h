/*

    缺页异常处理器

    创建于 2023年2月9日 江西省上饶市玉山县

*/

#pragma once

#include <yros/machine/X86Assembly.h>
#include <lib/sys/types.h>

/**
 * 缺页异常。
 */
namespace PageFaultException {

    void entrance();

    void handler(
        InterruptSoftwareFrame* softwareRegs, 
        InterruptHardwareFrame* hardwareRegs
    );

}
