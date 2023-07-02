#[[
    YROS 顶层构造文件
    创建于 2023年6月22日 上海市嘉定区安亭镇

    makefile 太痛苦了... 试试看 CMake —— GTY
]]


#cmake_minimum_required(VERSION 3.26.4)
cmake_minimum_required(VERSION 3.24.1)

set(YROS_VERSION_NAME "0.0.1")
set(YROS_VERSION_CODE 1)

project(YurongOS VERSION ${YROS_VERSION_NAME})


# 指定使用 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


# 设置 nasm 汇编
# 实际需要使用 nasm 时，
# 需要在子 CMakeLists.txt 内设置 enable_language
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS asm)
set(CMAKE_ASM_NASM_FLAGS "-g")
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "echo -n")


# 头文件路径。
include_directories(${PROJECT_SOURCE_DIR})


# GCC(g++) 编译选项。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostartfiles")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-builtin")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -no-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdlib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-threadsafe-statics")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-use-cxa-atexit")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1")  # 适当优化。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-red-zone")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-mmx")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-sse")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-sse2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcmodel=large")


# 子目录。
add_subdirectory(boot)
add_subdirectory(crt)
add_subdirectory(concurrent)
add_subdirectory(interrupt)
add_subdirectory(lib)
add_subdirectory(machine)
add_subdirectory(memory)
add_subdirectory(misc)
add_subdirectory(serial)
add_subdirectory(task)
add_subdirectory(device)


# 构建 kernel.exe
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--whole-archive,-m,elf_x86_64,-T,${CMAKE_SOURCE_DIR}/kernel.link.ld,-static")

set(NULL_CPP_FILE_PATH "${CMAKE_BINARY_DIR}/_yros_cmake_hack/null.cpp")
file(WRITE ${NULL_CPP_FILE_PATH} "")

set(CMAKE_CXX_LINK_EXECUTABLE "ld --whole-archive -m elf_x86_64 -static <LINK_LIBRARIES> -o <TARGET> -T ${CMAKE_SOURCE_DIR}/kernel.link.ld")

add_executable(kernel ${NULL_CPP_FILE_PATH})

target_link_libraries(
    kernel

    kernel_entry
    misc
    lib
    crt
    interrupt
    task
    machine
    memory 
    serial
    concurrent
    device
)

set_target_properties(kernel PROPERTIES SUFFIX ".exe")


set(TARGET_DIR "${CMAKE_BINARY_DIR}/../target")
set(OBJS_DIR "${TARGET_DIR}/objs")
set(KERNEL_EXE_TARGET "${OBJS_DIR}/kernel.exe")
set(KERNEL_BIN_TARGET "${OBJS_DIR}/kernel.bin")
set(ASM_DUMP_DIR "${TARGET_DIR}/asm-dump")

# copy kernel.exe to target directory
add_custom_command(
    TARGET kernel POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy 
            $<TARGET_FILE:kernel> 
            ${KERNEL_EXE_TARGET}
)

# dump kernel.exe to assembly
file(MAKE_DIRECTORY ${ASM_DUMP_DIR})
add_custom_command(
    TARGET kernel POST_BUILD
    COMMAND
        objdump -d ${KERNEL_EXE_TARGET} > ${ASM_DUMP_DIR}/kernel.bin.text.asm
)
add_custom_command(
    TARGET kernel POST_BUILD
    COMMAND
        objdump -D ${KERNEL_EXE_TARGET} > ${ASM_DUMP_DIR}/kernel.bin.full.asm
)

# convert kernel.exe to binary format
add_custom_command(
    TARGET kernel POST_BUILD
    COMMAND
        objcopy -O binary ${KERNEL_EXE_TARGET} ${KERNEL_BIN_TARGET}
)
