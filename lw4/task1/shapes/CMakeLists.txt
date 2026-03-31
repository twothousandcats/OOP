cmake_minimum_required(VERSION 3.14)
project(shapes VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /WX-)
else ()
    add_compile_options(-Wall -Wextra -pedantic -Werror=return-type -Wno-unused-parameter)
endif ()

# Зависимости
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)
find_package(Catch2 REQUIRED)

# Библиотека логики
add_library(shapes-logic STATIC
        src/CLineSegment.cpp
        src/CTriangle.cpp
        src/CRectangle.cpp
        src/CCircle.cpp
        src/CCanvas.cpp
)

target_include_directories(shapes-logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(shapes-logic PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)

# Основное приложение
add_executable(shapes src/main.cpp)
target_link_libraries(shapes PRIVATE
        shapes-logic
        SFML::Graphics
        SFML::Window
        SFML::System
)

# Тесты
add_executable(shapes-tests tests/tests.cpp)
target_include_directories(shapes-tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(shapes-tests
        PRIVATE
        shapes-logic
        Catch2::Catch2WithMain
)
target_compile_options(shapes-tests PRIVATE -O1) # fakeit+catch2

include(CTest)
include(Catch)
catch_discover_tests(shapes-tests)