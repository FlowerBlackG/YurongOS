// SPDX-License-Identifier: MulanPSL-2.0

/*

    内核守护进程

    创建于 2023年2月9日 江西省上饶市玉山县

*/

#include <task/IdleTask.h>

#include <machine/X86Assembly.h>
#include <task/TaskManager.h>
#include <crt/CRT.h>

#include <lib/stdio.h>

namespace IdleTask {

    void entrance() {
        
        while (true) {
            CRT::getInstance().write("[info] kerneld started.\n");
            int code = main();
            char s[64];
            sprintf(
                s, 
                "[warning] kerneld exited with errcode %d\n",
                code
            );
            CRT::getInstance().write(s);
        }

        x86asmUd2();
    }

    int main() {

        while (true) {

            Task* task = TaskManager::getCurrentTask();
            if (task->status == TaskStatus::READY) {
                // 处理一些事务，如清理已经挂掉的进程。
            }
            
            x86asmSti();
            x86asmHlt();
        }

        return 0;
    }

}
