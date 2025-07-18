cmake_minimum_required(VERSION 3.31)
project(learnProject)

set(CMAKE_CXX_STANDARD 23)

add_executable(learnProject main.cpp glad.c
        Shader.cpp
        Shader.h
        src/Component.h
        src/MeshComponent.cpp
        src/MeshComponent.h
        src/MaterialComponent.cpp
        src/MaterialComponent.h
        src/Texture.cpp
        src/Texture.h
        src/FileSystem.cpp
        src/FileSystem.h
        src/MeshStore.cpp
        src/MeshStore.h
        src/Context.h
        src/Component.cpp
        src/TextureStore.cpp
        src/TextureStore.h
        src/MeshBuilder.cpp
        src/MeshBuilder.h
        src/Shader.cpp
        src/Shader.h
        src/ShaderStore.cpp
        src/ShaderStore.h
        src/ResourcePackage.h
        src/ResourcePackage.cpp
        src/ResourcePackageStore.cpp
        src/ResourcePackageStore.h
        src/Scene.cpp
        src/Scene.h
        src/Node.cpp
        src/Node.h
        src/SceneStore.cpp
        src/SceneStore.h
        src/ComponentBuilder.cpp
        src/ComponentBuilder.h
)

target_link_directories(learnProject PUBLIC ${CMAKE_SOURCE_DIR}/../3rdparts/libs/)

target_link_libraries(learnProject PUBLIC glfw3 opengl32)

target_include_directories(learnProject PUBLIC ${CMAKE_SOURCE_DIR}/../3rdparts/include/)
target_include_directories(learnProject PUBLIC ${CMAKE_SOURCE_DIR}/src)
