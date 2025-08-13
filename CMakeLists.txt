cmake_minimum_required(VERSION 3.31)
project(learnProject)

set(CMAKE_CXX_STANDARD 23)

add_library(engine src/engine/glad.c

        src/engine/Component.h
        src/engine/MeshComponent.cpp
        src/engine/MeshComponent.h
        src/engine/MaterialComponent.cpp
        src/engine/MaterialComponent.h
        src/engine/Texture.cpp
        src/engine/Texture.h
        src/engine/FileSystem.cpp
        src/engine/FileSystem.h
        src/engine/MeshStore.cpp
        src/engine/MeshStore.h
        src/engine/Context.h
        src/engine/Component.cpp
        src/engine/TextureStore.cpp
        src/engine/TextureStore.h
        src/engine/MeshBuilder.cpp
        src/engine/MeshBuilder.h
        src/engine/Shader.cpp
        src/engine/Shader.h
        src/engine/ShaderStore.cpp
        src/engine/ShaderStore.h
        src/engine/ResourcePackage.h
        src/engine/ResourcePackage.cpp
        src/engine/ResourcePackageStore.cpp
        src/engine/ResourcePackageStore.h
        src/engine/Scene.cpp
        src/engine/Scene.h
        src/engine/Node.cpp
        src/engine/Node.h
        src/engine/SceneStore.cpp
        src/engine/SceneStore.h
        src/engine/ComponentBuilder.cpp
        src/engine/ComponentBuilder.h
        src/engine/CameraComponent.cpp
        src/engine/CameraComponent.h
        src/engine/TransformComponent.cpp
        src/engine/TransformComponent.h
        src/engine/SceneLoader.cpp
        src/engine/SceneLoader.h
        src/engine/Window.cpp
        src/engine/Window.h
        src/engine/Engine.cpp
        src/engine/Engine.h
        src/engine/SceneTransition.cpp
        src/engine/SceneTransition.h
        src/engine/Renderer.cpp
        src/engine/Renderer.h
        src/engine/SceneRequester.cpp
        src/engine/SceneRequester.h
        src/engine/SceneRequesterHelper.h
        src/engine/Logger.cpp
        src/engine/Logger.h
        src/engine/InputManager.cpp
        src/engine/InputManager.h
        src/engine/UserComponentsBuilder.h
        src/engine/Utils.h
        src/engine/Utils.cpp
        src/engine/FlipbookAnimationComponent.h
        src/engine/FlipbookAnimationComponent.cpp
)

target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/../3rdparts/include/)
target_link_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/../3rdparts/libs/)
target_link_libraries(engine PUBLIC glfw3 opengl32)

#find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(learnProject main.cpp)

target_link_libraries(learnProject PUBLIC engine)

target_include_directories(learnProject PUBLIC ${CMAKE_SOURCE_DIR}/src)
