cmake_minimum_required(VERSION 3.16)
project(SudokuGUI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(sudoku_gui
    src/main.cpp
    src/Game.cpp
    src/Board.cpp
    src/Solver.cpp
    src/Generator.cpp
)

target_include_directories(sudoku_gui PRIVATE src)
target_link_libraries(sudoku_gui PRIVATE SFML::Graphics SFML::Window SFML::System)

if(MSVC)
    target_compile_options(sudoku_gui PRIVATE /W4 /permissive-)
else()
    target_compile_options(sudoku_gui PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_command(TARGET sudoku_gui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:sudoku_gui>/assets
)
