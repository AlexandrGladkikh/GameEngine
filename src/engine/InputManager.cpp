#include "InputManager.h"
#include "Context.h"
#include "Window.h"
#include "Logger.h"

namespace engine {

InputManager::InputManager(const std::unique_ptr<Window>& window)
{
    window->setKeyInputHandler([this](int key, int action) {
        m_key.push_back(key);
        m_key_action.push_back(action);
    });
    window->setMouseInputHandler([this](int key, int action, int x_pos, int y_pos) {
        Logger::debug("{}: window->setMouseInputHandler", __FUNCTION__);
        m_mouse_key.push_back(key);
        m_mouse_key_action.push_back(action);
        m_mouse_pos.push_back({x_pos, y_pos});
    });
}

void InputManager::update(uint64_t dt)
{
    for (const auto& [key, handlers] : m_key_handlers) {
        for (size_t i = 0; i < m_key.size(); ++i) {
            if (m_key[i] != key) {
                continue;
            }
            for (auto& handler : handlers) {
                handler(m_key_action[i]);
            }
        }
    }

    Logger::info("m_mouse_handlers size: {}", m_mouse_handlers.size());
    Logger::info("m_mouse_key size: {}", m_mouse_key.size());
    Logger::info("m_mouse_key_action size: {}", m_mouse_key_action.size());
    Logger::info("m_mouse_pos size: {}", m_mouse_pos.size());
    for (const auto& [key, handlers] : m_mouse_handlers) {
        for (size_t i = 0; i < m_mouse_key.size(); ++i) {
            Logger::info("{}: for (size_t i = 0; i < m_mouse_key.size(); ++i)", __FUNCTION__);
            Logger::info("m_mouse_key[{}]: {}, key: {}", i, m_mouse_key[i], key);
            if (m_mouse_key[i] != key) {
                continue;
            }
            Logger::info("for (auto& handler : handlers)");
            for (auto& handler : handlers) {
                const auto mouse_pos = m_mouse_pos[i];
                handler(m_mouse_key_action[i], mouse_pos.first, mouse_pos.second);
            }
        }
    }

    m_key.clear();
    m_key_action.clear();
    m_mouse_key.clear();
    m_mouse_key_action.clear();
    m_mouse_pos.clear();
}

void InputManager::registerKeyHandler(int key, const std::function<void(int)>& handler)
{
    auto& handlers = m_key_handlers[key];
    handlers.push_back(handler);
}

void InputManager::unregisterKeyHandler(int key)
{
    m_key_handlers.erase(key);
}

void InputManager::registerMouseHandler(int key, const std::function<void(int, int, int)>& handler)
{
    Logger::debug("{}: registerMouseHandler key: {}", __FUNCTION__, key);
    auto& handlers = m_mouse_handlers[key];
    handlers.push_back(handler);
}

void InputManager::unregisterMouseHandler(int key)
{
    Logger::debug("{}: unregisterMouseHandler key: {}", __FUNCTION__, key);
    m_mouse_handlers.erase(key);
}

}
