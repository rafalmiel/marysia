project(magisterka)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/marysia/src)

file(GLOB_RECURSE SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE UI_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.ui)
qt5_wrap_ui(ui_HEADERS ${UI_LIST})

add_executable(${PROJECT_NAME} main.cpp ${SRC_LIST} ${ui_HEADERS})
target_link_libraries(${PROJECT_NAME} mar)
qt5_use_modules(${PROJECT_NAME} Widgets)

add_subdirectory(marysia)
