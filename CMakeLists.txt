cmake_minimum_required(VERSION 3.16)

################################################################################
# Macros
################################################################################


macro(CreateMainTarget)
    add_library(JunglesOsHelpers INTERFACE)
    target_include_directories(JunglesOsHelpers INTERFACE ${CMAKE_CURRENT_LIST_DIR})
    target_compile_features(JunglesOsHelpers INTERFACE cxx_std_17)
    add_library(jungles::os_helpers ALIAS JunglesOsHelpers)

    add_library(JunglesOs STATIC jungles_os_helpers/freertos/src/event_group.cpp)
    target_include_directories(JunglesOs PUBLIC ${CMAKE_CURRENT_LIST_DIR})
endmacro()


################################################################################
# Main script
################################################################################

project(JunglesOsHelpers)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

CreateMainTarget()

set(JUNGLES_OS_HELPERS_ENABLE_TESTING OFF CACHE BOOL "Enables self-testing of the library")

if(JUNGLES_OS_HELPERS_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()
