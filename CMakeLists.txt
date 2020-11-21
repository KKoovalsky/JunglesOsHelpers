cmake_minimum_required(VERSION 3.16)

################################################################################
# Macros
################################################################################


macro(CreateMainTarget)
    add_library(JunglesOsHelpers INTERFACE)
    target_include_directories(JunglesOsHelpers INTERFACE ${CMAKE_CURRENT_LIST_DIR})
    target_compile_features(JunglesOsHelpers INTERFACE cxx_std_17)
    target_compile_options(JunglesOsHelpers INTERFACE -Wall -Wextra)
endmacro()


################################################################################
# Main script
################################################################################


project(JunglesOsHelpers)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

CreateMainTarget()

set(JUNGLES_OS_HELPERS_ENABLE_TESTING OFF CACHE BOOL "Enables self-testing of the library")

if(JUNGLES_OS_HELPERS_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()
