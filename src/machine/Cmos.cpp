/*


    创建时间：2023年1月5日

*/

#include <yros/IO.h>
#include <yros/machine/Cmos.h>
#include <yros/machine/X86Assembly.h>

uint8_t Cmos::readByte(uint8_t offset) {
    uint8_t result;

    x86asmCli();
    IO::outByte(CMOS_ADDR_PORT, offset);
    result = IO::inByte(CMOS_DATA_PORT);
    x86asmSti();

    return result;

}

