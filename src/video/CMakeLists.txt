#[[
    视频
    创建于 2023年7月4日 上海市嘉定区安亭镇
]]


file(GLOB_RECURSE cxx_files *.cpp)


add_library(
    video

    ${cxx_files}

)


