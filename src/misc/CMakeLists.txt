#[[
    杂项
    创建于 2023年6月23日 上海市嘉定区安亭镇
]]

file(GLOB_RECURSE cxx_files *.cpp)
file(GLOB_RECURSE nasm_files *.asm)

set(kernel_entry_file "${CMAKE_CURRENT_SOURCE_DIR}/kernel_entry.asm")

list(REMOVE_ITEM nasm_files ${kernel_entry_file})

enable_language(ASM_NASM)

add_library(
    misc

    ${cxx_files}
    ${nasm_files}

)

# kernel entry 要作为整个内核代码的入口。需要单独构建。
# 链接时，它会被指定放到开头位置。
add_library(
    kernel_entry

    ${kernel_entry_file}
)

set_target_properties(kernel_entry PROPERTIES PREFIX "")
set_target_properties(kernel_entry PROPERTIES SUFFIX ".o")

