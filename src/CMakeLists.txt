set(
    sources
    "EC2Utils.cpp"
    "EC2InstanceManager.cpp"
)

add_library(EC2-Utils STATIC ${sources})

target_include_directories(EC2-Utils PUBLIC "${PROJECT_SOURCE_DIR}/include")
