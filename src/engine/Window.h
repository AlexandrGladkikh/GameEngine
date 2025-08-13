#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <functional>
#include <vector>

namespace engine {

class Window final {
public:
    explicit Window(const std::string& title, GLint width, GLint height);
    ~Window();

    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    void setKeyInputHandler(const std::function<void(int, int)>& handler);

    void update(uint64_t dt);

    void swapBuffer();

    auto size() const -> std::pair<int, int> ;

    std::string title() const;

private:
    GLFWwindow* m_window;

    std::function<void(int, int)> m_keyInputHandler;

    std::vector<int> m_key;
    std::vector<int> m_action;
};

}
