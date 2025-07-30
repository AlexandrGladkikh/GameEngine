#pragma once

#include <memory>

namespace engine {

class Context;
class Scene;

class Renderer final {
public:
    Renderer() = default;
    ~Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    static void render(const std::shared_ptr<Context>& context, const std::shared_ptr<Scene>& scene);
};

}
