cmake_minimum_required(VERSION 3.13)

project(codecrafters-shell C)

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23)

add_executable(shell ${SOURCE_FILES})

if(APPLE)
    # Homebrew paths — these must match your actual Homebrew install
    target_include_directories(shell PRIVATE /opt/homebrew/Cellar/readline/8.2.13/include)
    # Note: target_link_directories only sets search paths, you still need to link the library
    target_link_libraries(shell PRIVATE readline)
else()
    # On Linux or other OS, adjust paths accordingly or rely on system libs
    target_link_libraries(shell PRIVATE readline)
endif()