#[[
    设备
    创建于 2023年6月22日 上海市嘉定区安亭镇
]]



file(GLOB_RECURSE cxx_files *.cpp)

add_library(
    device
    
    ${cxx_files}
)
