
if(NOT TARGET string_id)
    add_subdirectory(string_id)
    target_include_directories(foonathan_string_id PUBLIC .)
    add_library(string_id ALIAS foonathan_string_id)
endif ()

