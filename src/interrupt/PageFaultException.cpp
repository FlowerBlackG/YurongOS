// SPDX-License-Identifier: MulanPSL-2.0


/*

    缺页异常处理器

    创建于 2023年2月9日 江西省上饶市玉山县

*/

#include <interrupt/PageFaultException.h>
#include <interrupt/ImplementHandlerMacros.h>

#include <crt/CRT.h>

#include <memory/PageDirectories.h>
#include <lib/stdio.h>

namespace PageFaultException {

    IMPLEMENT_EXCEPTION_WITH_ERRCODE_ENTRANCE(entrance, PageFaultException::handler);

    // https://wiki.osdev.org/Exceptions#Error_code

    static __force_inline bool __errcodePresent(uint64_t errcode) {
        return errcode & 1;
    }
    
    static __force_inline bool __errcodeWrite(uint64_t errcode) {
        return errcode & 2;
    }
    
    static __force_inline bool __errcodeUser(uint64_t errcode) {
        return errcode & 4;
    }
    
    static __force_inline bool __errcodeReservedWrite(uint64_t errcode) {
        return errcode & 8;
    }
    
    static __force_inline bool __errcodeInstructionFetch(uint64_t errcode) {
        return errcode & 16;
    }
    
    static __force_inline bool __errcodeProtectionKey(uint64_t errcode) {
        return errcode & 32;
    }
    
    static __force_inline bool __errcodeShadowStack(uint64_t errcode) {
        return errcode & 64;
    }
    
    static __force_inline bool __errcodeSoftwareGuardExtensions(uint64_t errcode) {
        return errcode & 128;
    }

    static void __decodeErrCode(uint64_t errorcode) {
        
        CRT::getInstance().write("    err: ");

        if (__errcodePresent(errorcode)) {
            CRT::getInstance().write("Present ");
        }
        
        if (__errcodeWrite(errorcode)) {
            CRT::getInstance().write("Write ");
        }
        
        if (__errcodeUser(errorcode)) {
            CRT::getInstance().write("Usr ");
        }
        
        if (__errcodeReservedWrite(errorcode)) {
            CRT::getInstance().write("RsvW ");
        }
        
        if (__errcodeInstructionFetch(errorcode)) {
            CRT::getInstance().write("Ins ");
        }
        
        if (__errcodeProtectionKey(errorcode)) {
            CRT::getInstance().write("Key ");
        }
        
        if (__errcodeShadowStack(errorcode)) {
            CRT::getInstance().write("Ss ");
        }

        CRT::getInstance().write("\n");
    }

    static void __solvePageFaultInKernelProcess(
        InterruptSoftwareFrame* softwareRegs, 
        InterruptHardwareFrame* hardwareRegs,
        uint64_t cr2, uint64_t cr3
    ) {
        // 缺页发生在内核态。这是不可饶恕的。

        char s[256]; 
        
        CRT::getInstance().write(" ------------\n  "); 
        CRT::getInstance().write("page fault in kernel process"); 
        CRT::getInstance().write(" (with errcode)\n"); 
        
        __decodeErrCode(hardwareRegs->errorCode);

        sprintf( 
            s, 
            "    errcode: 0x%llx\n" 
            "    rax: 0x%llx\n" 
            "    rbp: 0x%llx, rsp: 0x%llx\n" 
            "    cs: 0x%x, ds: 0x%x, es: 0x%x\n" 
            "    ss: 0x%x, fs: 0x%x \n" 
            "    rip: 0x%llx\n"
            "    cr2: 0x%llx, cr3: 0x%llx\n", 
            hardwareRegs->errorCode, 
            softwareRegs->rax, 
            softwareRegs->rbp, 
            hardwareRegs->rsp, 
            hardwareRegs->cs, softwareRegs->ds, softwareRegs->es, 
            hardwareRegs->ss, softwareRegs->fs, 
            hardwareRegs->rip,
            cr2, cr3 
        ); 
        
        CRT::getInstance().write(s); 
        CRT::getInstance().write(" ------------\n"); 
        
        Kernel::panic("[critical] kernel panic.\n"); 
    }

    static void __solvePageFaultInUserProcess(
        InterruptSoftwareFrame* softwareRegs, 
        InterruptHardwareFrame* hardwareRegs,
        uint64_t cr2, uint64_t cr3
    ) {
        
        char s[64]; 
        
        CRT::getInstance().write(s); 
        
        if (cr2 >> 16 >> 16 >> 16 >> 15) {
            
            Kernel::panic("[error] page fault: bad access.\n");
        }

        // 缺页发生在用户态。

        unsigned long pml4addr = cr3;
        auto pml4 = (PageMapLevel4) (pml4addr + memory::MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP);

        uintptr_t newPageAddr = memory::MemoryManager::allocPage();
        if (!newPageAddr) {
            Kernel::panic("[error] memory ran out (d91a).\n");
        }

        int resCode = memory::MemoryManager::mapPage(pml4, cr2, newPageAddr, true, true);
        
        if (resCode) {
            sprintf(s, "[error] user memory allocation failed (%d).\n", resCode);
            Kernel::panic(s);
        }

    }

    void handler(
        InterruptSoftwareFrame* softwareRegs, 
        InterruptHardwareFrame* hardwareRegs
    ) {
        x86asmCli(); 
        uint64_t cr2, cr3; 

        __asm( 
            "movq %%cr2, %%rax \n\t" 
            "movq %%cr3, %%rbx \n\t" 
            : "=a" (cr2), "=b" (cr3) 
            : 
        ); 

        if ( hardwareRegs->cs & 0x3 ) {
            __solvePageFaultInUserProcess(softwareRegs, hardwareRegs, cr2, cr3);
        } else {
            __solvePageFaultInKernelProcess(softwareRegs, hardwareRegs, cr2, cr3);
        }

        x86asmSti();
    }

}
