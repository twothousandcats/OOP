cmake_minimum_required(VERSION 3.14)
project(date VERSION 1.0 LANGUAGES CXX)

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
file(GLOB SRC_FILES "src/*.cpp")
list(FILTER SRC_FILES EXCLUDE REGEX ".*tests.*")

# Библиотека логики
add_library(date_logic STATIC ${SRC_FILES})
target_include_directories(date_logic PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Основное приложение
add_executable(date_app src/main.cpp) # не дублировать в exe лишний раз
target_link_libraries(date_app PRIVATE date_logic)
set_target_properties(date_app PROPERTIES OUTPUT_NAME "date")

# Тесты
enable_testing()
find_package(Catch2 REQUIRED)

file(GLOB TEST_FILES "tests/*.cpp")
add_executable(date_tests ${TEST_FILES})

target_link_libraries(date_tests PRIVATE
        date_logic
        Catch2::Catch2WithMain
)

target_include_directories(date_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Регистрация CTest
include(CTest)
add_test(NAME dateUnitTests COMMAND date_tests)
set_property(TEST dateUnitTests PROPERTY LABELS "Unit")

# Цель для запуска тестов
add_custom_target(run_tests_date
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R dateUnitTests
        DEPENDS date_tests
        COMMENT "Running unit tests..."
        VERBATIM
)

# Зависимость: приложение не соберется, пока не пройдут тесты
add_dependencies(date_app run_tests_date)