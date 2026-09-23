FILE(GLOB raknetCPPSources *.cpp)
add_library(raknet STATIC ${raknetCPPSources})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(raknet PRIVATE ws2_32)