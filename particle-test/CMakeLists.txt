set(target_name particle_test)
project(${target_name})

# 使用file(GLOB)自动收集所有源文件
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
        "*.cpp"
        "*.cxx"
        "*.cc"
        "*.c"
)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
        "*.h"
        "*.hpp"
        "*.hxx"
        "*.hh"
)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=4)
add_executable(${target_name} main.cpp
        ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${target_name} PRIVATE string_id)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${target_name} PRIVATE glm::glm)

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(${target_name} PRIVATE magic_enum::magic_enum)