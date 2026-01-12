#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <functional>
#include <vector>

namespace engine {

class Window final {
public:
    explicit Window(const std::string& title, GLint width, GLint height, GLint gl_version_major, GLint gl_version_minor);
    ~Window();

    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    void setWindowPosition(int x, int y);

    void setKeyInputHandler(const std::function<void(int, int)>& handler);
    void setMouseInputHandler(const std::function<void(int, int, int, int)>& handler);

    void update(uint64_t dt);

    void swapBuffer();

    auto size() const -> std::pair<int, int> ;

    std::string title() const;

private:
    GLFWwindow* m_window;

    std::function<void(int, int)> m_keyInputHandler;
    std::function<void(int, int, int, int)> m_mouseInputHandler;

    std::vector<int> m_key;
    std::vector<int> m_key_action;
    std::vector<int> m_mouse_key;
    std::vector<int> m_mouse_key_action;
    std::vector<std::pair<int, int>> m_mouse_pos;
};

}
