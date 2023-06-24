#[[
    内核代码支持库
    创建于 2023年6月23日 上海市嘉定区安亭镇
]]


file(GLOB_RECURSE cxx_files *.cpp)


add_library(
    lib

    ${cxx_files}

)


