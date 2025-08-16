cmake_minimum_required(VERSION 3.31)
project(learnProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_EDITOR "Build Qt application" ON)

add_executable(learnProject ${CMAKE_SOURCE_DIR}/main.cpp)

if (ENABLE_EDITOR)
    file(GLOB_RECURSE EDITOR_SOURCES ${CMAKE_SOURCE_DIR}/src/editor/*.cpp
                                     ${CMAKE_SOURCE_DIR}/src/editor/*.h)

    target_sources(learnProject PRIVATE ${EDITOR_SOURCES})

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

    target_link_libraries(learnProject PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

    target_compile_definitions(learnProject PRIVATE ENABLE_EDITOR)
endif()

add_subdirectory(src/engine)
target_link_libraries(learnProject PRIVATE engine)

target_include_directories(learnProject PRIVATE ${CMAKE_SOURCE_DIR}/src)
