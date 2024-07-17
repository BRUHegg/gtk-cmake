cmake_minimum_required(VERSION 3.13)
project(gtk-test CXX)

# My very original CMake boilerplate XD

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -fPIC -g")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (PkgConfig REQUIRED)
pkg_check_modules (GTK2 REQUIRED gtk+-3.0)

include_directories (${GTK2_INCLUDE_DIRS})
link_directories (${GTK2_LIBRARY_DIRS})
add_executable (gtk-test main.cpp)
add_definitions (${GTK2_CFLAGS_OTHER})
target_link_libraries (gtk-test ${GTK2_LIBRARIES})


set_target_properties(gtk-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")
