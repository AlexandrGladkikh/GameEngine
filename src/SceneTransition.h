#pragma once

#include <memory>

namespace engine {

class Context;

class SceneTransition final {
public:
    explicit SceneTransition(const std::shared_ptr<Context>& context);
    ~SceneTransition() = default;
    SceneTransition(const SceneTransition&) = delete;
    SceneTransition(SceneTransition&&) = delete;
    SceneTransition& operator=(const SceneTransition&) = delete;
    SceneTransition& operator=(SceneTransition&&) = delete;

    bool transition(const std::string& scene_id_from, const std::string& scene_id_to);

private:
    std::shared_ptr<Context> m_context;
};

}
