// SPDX-License-Identifier: MulanPSL-2.0

/*


    创建时间：2023年1月5日

*/

#include <misc/io.h>
#include <machine/Cmos.h>
#include <machine/X86Assembly.h>

uint8_t Cmos::readByte(uint8_t offset) {
    uint8_t result;

    x86asmCli();
    io::outByte(CMOS_ADDR_PORT, offset);
    result = io::inByte(CMOS_DATA_PORT);
    x86asmSti();

    return result;

}

