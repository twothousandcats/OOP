cmake_minimum_required(VERSION 3.14)
project(url VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings as errors
#if(MSVC)
#    add_compile_options(/W4 /WX)
#else()
#    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
#endif()

# Сборка исходников
add_library(url_logic STATIC
        src/CHttpUrl.cpp
        src/CUrlParsingError.cpp
)


# Библиотека логики
#add_library(url_logic STATIC ${SRC_FILES})
target_include_directories(url_logic PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Основное приложение
add_executable(url_app src/main.cpp) # не дублировать в exe лишний раз
target_link_libraries(url_app PRIVATE url_logic)
set_target_properties(url_app PROPERTIES OUTPUT_NAME "url")

# Тесты
enable_testing()
find_package(Catch2 REQUIRED)

file(GLOB TEST_FILES "tests/*.cpp")
add_executable(url_tests ${TEST_FILES})

target_link_libraries(url_tests PRIVATE
        url_logic
        Catch2::Catch2WithMain
)

target_include_directories(url_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Регистрация CTest
include(CTest)
add_test(NAME urlUnitTests COMMAND url_tests)
set_property(TEST urlUnitTests PROPERTY LABELS "Unit")

# Цель для запуска тестов
add_custom_target(run_tests_url
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -R urlUnitTests
        DEPENDS url_tests
        COMMENT "Running unit tests..."
        VERBATIM
)

# Зависимость: приложение не соберется, пока не пройдут тесты
add_dependencies(url_app run_tests_url)