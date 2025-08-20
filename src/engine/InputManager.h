#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>

namespace engine {

class Window;

class InputManager final {
public:
    explicit InputManager(const std::unique_ptr<Window>& window);
    ~InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager& operator=(InputManager&&) = delete;

    void update(uint64_t dt);

    void registerHandler(int key, const std::function<void(int)>& handler);
    void unregisterHandler(int key);

private:
    std::unordered_map<uint32_t, std::vector<std::function<void(int)>>> m_handlers;

    std::vector<int> m_key;
    std::vector<int> m_action;
};

}
