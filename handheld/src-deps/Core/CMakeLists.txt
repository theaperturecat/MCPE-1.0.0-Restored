cmake_minimum_required(VERSION 3.25)


add_library(Core STATIC)

target_include_directories(Core
PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_sources(Core
PRIVATE
pch_core.cpp
Resource/ResourceHelper.cpp
Resource/ResourceUtil.cpp
Math/Color.cpp
Debug/DebugEventLogger.cpp
Utility/InitOnce.cpp
Utility/MCResult.cpp
Utility/MemoryTracker.cpp
Utility/PropertyBag.cpp
Utility/TypeId.cpp
Utility/utf8proc.c
Utility/UUID.cpp
PUBLIC
)

target_compile_definitions(Core
    PRIVATE
    MCPE_PLATFORM_WIN32
    MCPE_PLATFORM_WINDOWS_FAMILY
)

target_precompile_headers(Core PRIVATE     
"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch_core.h>")