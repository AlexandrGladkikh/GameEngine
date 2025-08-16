cmake_minimum_required(VERSION 3.31)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(engine glad.c
        Component.h
        MeshComponent.cpp
        MeshComponent.h
        MaterialComponent.cpp
        MaterialComponent.h
        Texture.cpp
        Texture.h
        FileSystem.cpp
        FileSystem.h
        MeshStore.cpp
        MeshStore.h
        Context.h
        Component.cpp
        TextureStore.cpp
        TextureStore.h
        MeshBuilder.cpp
        MeshBuilder.h
        Shader.cpp
        Shader.h
        ShaderStore.cpp
        ShaderStore.h
        ResourcePackage.h
        ResourcePackage.cpp
        ResourcePackageStore.cpp
        ResourcePackageStore.h
        Scene.cpp
        Scene.h
        Node.cpp
        Node.h
        SceneStore.cpp
        SceneStore.h
        ComponentBuilder.cpp
        ComponentBuilder.h
        CameraComponent.cpp
        CameraComponent.h
        TransformComponent.cpp
        TransformComponent.h
        SceneLoader.cpp
        SceneLoader.h
        Window.cpp
        Window.h
        Engine.cpp
        Engine.h
        SceneTransition.cpp
        SceneTransition.h
        Renderer.cpp
        Renderer.h
        SceneRequester.cpp
        SceneRequester.h
        SceneRequesterHelper.h
        Logger.cpp
        Logger.h
        InputManager.cpp
        InputManager.h
        UserComponentsBuilder.h
        Utils.h
        Utils.cpp
        FlipbookAnimationComponent.h
        FlipbookAnimationComponent.cpp
)

target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/../3rdparts/include/)
target_link_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/../3rdparts/libs/)
target_link_libraries(engine PUBLIC glfw3 opengl32)

target_include_directories(engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})