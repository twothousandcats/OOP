cmake_minimum_required(VERSION 3.14)
project(StringStack VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings as errors
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Сборка исходников
add_library(StringStack_logic STATIC
        src/StringStack.cpp
)


# Библиотека логики
#add_library(StringStack_logic STATIC ${SRC_FILES})
target_include_directories(StringStack_logic PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Основное приложение
add_executable(StringStack_app src/main.cpp) # не дублировать в exe лишний раз
target_link_libraries(StringStack_app PRIVATE StringStack_logic)
set_target_properties(StringStack_app PROPERTIES OUTPUT_NAME "StringStack")

# Тесты
enable_testing()
find_package(Catch2 REQUIRED)

file(GLOB TEST_FILES "tests/*.cpp")
add_executable(StringStack_tests ${TEST_FILES})

target_link_libraries(StringStack_tests PRIVATE
        StringStack_logic
        Catch2::Catch2WithMain
)

target_include_directories(StringStack_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Регистрация CTest
include(CTest)
add_test(NAME StringStackUnitTests COMMAND StringStack_tests)
set_property(TEST StringStackUnitTests PROPERTY LABELS "Unit")

# Цель для запуска тестов
add_custom_target(run_tests_StringStack
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R StringStackUnitTests
        DEPENDS StringStack_tests
        COMMENT "Running unit tests..."
        VERBATIM
)

# Зависимость: приложение не соберется, пока не пройдут тесты
add_dependencies(StringStack_app run_tests_StringStack)