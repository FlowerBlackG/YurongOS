#[[
    启动引导
    创建于 2023年6月22日 上海市嘉定区安亭镇
]]

set(BOOT_OUTPUT_DIR "${CMAKE_BINARY_DIR}/../target/objs/boot")
file(MAKE_DIRECTORY ${BOOT_OUTPUT_DIR})

set(CMAKE_ASM_NASM_LINK_EXECUTABLE "cp <OBJECTS> ${BOOT_OUTPUT_DIR}/<TARGET>.bin")
set(CMAKE_ASM_NASM_OBJECT_FORMAT bin)
enable_language(ASM_NASM)

add_executable(boot boot.asm)
add_executable(kernel_loader kernel_loader.asm)
