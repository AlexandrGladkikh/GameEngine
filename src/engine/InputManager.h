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

    void registerKeyHandler(int key, const std::function<void(int)>& handler);
    void unregisterKeyHandler(int key);

    void registerMouseHandler(int key, const std::function<void(int, int, int)>& handler);
    void unregisterMouseHandler(int key);

private:
    std::unordered_map<uint32_t, std::vector<std::function<void(int)>>> m_key_handlers;
    std::unordered_map<uint32_t, std::vector<std::function<void(int, int, int)>>> m_mouse_handlers;

    std::vector<int> m_key;
    std::vector<int> m_key_action;
    std::vector<int> m_mouse_key;
    std::vector<int> m_mouse_key_action;
    std::vector<std::pair<int, int>> m_mouse_pos;
};

}
