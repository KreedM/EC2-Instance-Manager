set(
    sources
    "${PROJECT_SOURCE_DIR}/include/EC2Utils.h"
    "EC2Utils.cpp"
    "${PROJECT_SOURCE_DIR}/include/EC2InstanceManager.h"
    "EC2InstanceManager.cpp"
)

add_library(EC2-Utils STATIC ${sources})

target_include_directories(EC2-Utils PRIVATE "${PROJECT_SOURCE_DIR}/include")
