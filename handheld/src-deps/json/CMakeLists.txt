cmake_minimum_required(VERSION 3.25)


add_library(json STATIC)

target_include_directories(json
PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(json
PRIVATE
src/json_reader.cpp
src/json_value.cpp
src/json_writer.cpp

PUBLIC
)

#target_link_libraries(json
#PUBLIC)

#target_compile_definitions(json
#    PRIVATE
#    MCPE_PLATFORM_WIN32
#    MCPE_PLATFORM_WINDOWS_FAMILY
#)