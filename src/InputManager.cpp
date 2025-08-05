#include "InputManager.h"
#include "Context.h"
#include "Window.h"

#include <ranges>

namespace engine {

InputManager::InputManager(const std::unique_ptr<Window>& window)
{
    window->setKeyInputHandler([this](int key, int action) {
        m_key.push_back(key);
        m_action.push_back(action);
    });
}

void InputManager::update(uint64_t dt)
{
    for (const auto& [key, val] : m_handlers) {
        for (size_t i = 0; i < m_key.size(); ++i) {
            if (m_key[i] != key) {
                continue;
            }
            val->handler(m_key[i], m_action[i]);
        }
    }

    m_key.clear();
    m_action.clear();
}

void InputManager::registerHandler(int key, const std::function<void(int, int)>& handler)
{
    m_handlers[key] = std::make_unique<InputHandler>(key, handler);
}

void InputManager::unregisterHandler(int key)
{
    m_handlers.erase(key);
}

}
