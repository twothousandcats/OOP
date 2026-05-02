cmake_minimum_required(VERSION 3.14)
project(array VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings as errors
#if(MSVC)
#    add_compile_options(/W4 /WX)
#else()
#    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
#endif()

# Header-only library: templates must be visible to every translation unit
add_library(array_logic INTERFACE)

target_include_directories(array_logic INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(array_logic INTERFACE cxx_std_20)

# Main application
add_executable(array_app src/main.cpp)
target_link_libraries(array_app PRIVATE array_logic)
set_target_properties(array_app PROPERTIES OUTPUT_NAME "array")

# Tests
enable_testing()
find_package(Catch2 REQUIRED)

file(GLOB TEST_FILES CONFIGURE_DEPENDS "tests/*.cpp")
add_executable(array_tests ${TEST_FILES})

target_link_libraries(array_tests PRIVATE
        array_logic
        Catch2::Catch2WithMain
)

# CTest registration
include(CTest)
include(Catch)
catch_discover_tests(array_tests
        TEST_PREFIX "array."
        PROPERTIES LABELS "Unit"
)

# Convenience target to run tests
add_custom_target(run_tests_array
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L Unit
        DEPENDS array_tests
        COMMENT "Running unit tests..."
        VERBATIM
)