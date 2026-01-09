set(target_name particle_test)
project(${target_name})

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=4)
add_executable(${target_name} main.cpp
        main.cpp)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${target_name} PRIVATE glm::glm)

find_package(magic_enum CONFIG REQUIRED)
target_link_libraries(${target_name} PRIVATE magic_enum::magic_enum)