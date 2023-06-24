#[[
    中断
    创建于 2023年6月23日 上海市嘉定区安亭镇
]]

file(GLOB_RECURSE cxx_files *.cpp)
file(GLOB_RECURSE nasm_files *.asm)

enable_language(ASM_NASM)

add_library(
    interrupt

    ${cxx_files}
    ${nasm_files}

)

