#pragma once

#include <memory>

namespace engine {

class MeshStore;
class ShaderStore;
class TextureStore;
class ResourcePackageStore;
class SceneStore;
class Window;
class InputManager;
class UserComponentsBuilder;
class EngineAccessor;

struct Context {
    std::unique_ptr<MeshStore> meshStore;
    std::unique_ptr<ShaderStore> shaderStore;
    std::unique_ptr<TextureStore> textureStore;
    std::unique_ptr<ResourcePackageStore> resourcePackageStore;
    std::unique_ptr<SceneStore> sceneStore;
    std::unique_ptr<Window> window;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<UserComponentsBuilder> userComponentsBuilder;
    std::unique_ptr<EngineAccessor> engineAccessor;
};

}
