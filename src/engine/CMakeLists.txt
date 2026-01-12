cmake_minimum_required(VERSION 3.31)

project(Engine LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(engine
        glad.c
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
        Helpers.h
        MouseEventFilterComponent.h
        MouseEventFilterComponent.cpp
        NodePositioningHelper.cpp
        NodePositioningHelper.h
        SceneConfig.cpp
        SceneConfig.h
        EngineSettings.cpp
        EngineSettings.h
)

if (APPLE)
    find_package(glfw3 3.3 REQUIRED)   # обычно работает с brew-версией glfw
    find_package(OpenGL REQUIRED)

    target_link_libraries(engine PRIVATE glfw OpenGL::GL)
    target_include_directories(engine PRIVATE ${OPENGL_INCLUDE_DIRS})

    target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdparts)
elseif(WIN32)
    get_filename_component(THIRD_PARTIES_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../3rdparts/ ABSOLUTE)
    target_include_directories(engine PUBLIC ${THIRD_PARTIES_DIR}/include/)

    target_link_directories(engine PUBLIC ${THIRD_PARTIES_DIR}/libs/)
    target_link_libraries(engine PUBLIC glfw3 opengl32)
endif ()

target_include_directories(engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
