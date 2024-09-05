set(
    sources
    "${PROJECT_SOURCE_DIR}/src/EC2Utils.cpp"
)

add_library(EC2-Utils STATIC ${sources})

target_include_directories(EC2-Utils PUBLIC "${PROJECT_SOURCE_DIR}/include")
