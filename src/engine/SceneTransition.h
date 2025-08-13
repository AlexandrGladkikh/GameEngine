#pragma once

#include <memory>
#include <filesystem>
#include <unordered_map>

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

    bool transition(const std::unordered_map<uint32_t, std::filesystem::path>& scene_infos, uint32_t scene_id_from, uint32_t scene_id_to);

private:
    std::shared_ptr<Context> m_context;
};

}
