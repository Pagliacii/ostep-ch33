cmake_minimum_required(VERSION 3.10)

# set the make program and compilers
set(CMAKE_MAKE_PROGRAM make CACHE FILEPATH "" FORCE)
set(CMAKE_C_COMPILER gcc CACHE FILEPATH "" FORCE)
set(AR ar CACHE FILEPATH "" FORCE)

# set the project name and version
project(ostep_ch33 VERSION 1.0 LANGUAGES C)

if(APPLE)
    set(CMAKE_INSTALL_PATH "@executable_path/../lib")
elseif(UNIX)
    set(CMAKE_INSTALL_PATH "$ORIGIN/../lib")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib")
add_compile_options(-ansi -Wall -Werror -DLINUX -D_GNU_SOURCE)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# add the executable
add_executable(client client.c)

add_executable(simple simple_server.c server.c)
target_link_libraries(simple PUBLIC tools)

add_executable(select select_server.c server.c)
target_link_libraries(select PUBLIC tools)

install(TARGETS client simple DESTINATION bin)
install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/tool.h"
    DESTINATION include
)