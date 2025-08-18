cmake_minimum_required(VERSION 3.31)
project(learnProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src/engine)

option(ENABLE_EDITOR "Build Qt application" ON)

if (ENABLE_EDITOR)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
endif()

add_executable(learnProject ${CMAKE_SOURCE_DIR}/main.cpp)

if (ENABLE_EDITOR)
    message(STATUS "AUTOMOC for MyApp: ${CMAKE_AUTOMOC}")
    get_target_property(AUTOMOC_PROP learnProject AUTOMOC)
    message(STATUS "MyApp AUTOMOC property: ${AUTOMOC_PROP}")

    file(GLOB_RECURSE EDITOR_SOURCES ${CMAKE_SOURCE_DIR}/src/editor/*.cpp
                                     ${CMAKE_SOURCE_DIR}/src/editor/*.h)

    file(GLOB_RECURSE EDITOR_HEADERS ${CMAKE_SOURCE_DIR}/src/editor/*.h
                                     ${CMAKE_SOURCE_DIR}/src/editor/*.h)

    target_sources(learnProject PRIVATE ${EDITOR_SOURCES} ${EDITOR_HEADERS})

    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

    target_link_libraries(learnProject PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

    target_compile_definitions(learnProject PRIVATE ENABLE_EDITOR)
endif()

target_link_libraries(learnProject PRIVATE engine)

target_include_directories(learnProject PRIVATE ${CMAKE_SOURCE_DIR}/src)
