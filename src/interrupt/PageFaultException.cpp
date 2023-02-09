/*

    缺页异常处理器

    创建于 2023年2月9日 江西省上饶市玉山县

*/

#include <yros/interrupt/PageFaultException.h>
#include <yros/interrupt/ImplementHandlerMacros.h>

#include <yros/CRT.h>

#include <yros/memory/PageDirectories.h>
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

    void handler(
        InterruptSoftwareFrame* softwareRegs, 
        InterruptHardwareFrame* hardwareRegs
    ) {
        x86asmCli(); 
        uint64_t cr2, cr3; 

        if (hardwareRegs->cs & 0x3 == 0) {

            // 缺页发生在内核态。这是不可饶恕的。

            char s[256]; 
            
            CRT::getInstance().write(" ------------\n  "); 
            CRT::getInstance().write("page fault in kernel process"); 
            CRT::getInstance().write(" (with errcode)\n"); 
            
            __asm( 
                "movq %%cr2, %%rax \n\t" 
                "movq %%cr3, %%rbx \n\t" 
                : "=a" (cr2), "=b" (cr3) 
                : 
            ); 
            
            __decodeErrCode(hardwareRegs->errorCode);

            sprintf( 
                s, 
                "    errcode: 0x%llx\n" 
                "    rax: 0x%llx\n" 
                "    rbp: 0x%llx, rsp: 0x%llx\n" 
                "    cs: 0x%x, ds: 0x%x, es: 0x%x\n" 
                "    ss: 0x%x, fs: 0x%x, gs: 0x%x\n" 
                "    cr2: 0x%llx, cr3: 0x%llx\n", 
                hardwareRegs->errorCode, 
                softwareRegs->rax, 
                softwareRegs->rbp, 
                hardwareRegs->rsp, 
                hardwareRegs->cs, softwareRegs->ds, softwareRegs->es, 
                hardwareRegs->ss, softwareRegs->fs, softwareRegs->gs, 
                cr2, cr3 
            ); 
            
            CRT::getInstance().write(s); 
            CRT::getInstance().write(" ------------\n"); 
            
            Kernel::panic("[critical] kernel panic.\n"); 
        } else {

            // 缺页发生在用户态。

            int pml1idx = (cr2 >> 12) % 512;
            int pml2idx = (cr2 >> 21) % 512;
            int pml3idx = (cr2 >> 30) % 512;
            int pml4idx = (cr2 >> 39) % 512;

            unsigned long pml4addr = cr3;
            pml4addr += MemoryManager::ADDRESS_OF_PHYSICAL_MEMORY_MAP;

            auto pml4 = (PageMapLevel4) pml4addr;

        }

        x86asmSti();
    }

}
