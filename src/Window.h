#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

namespace engine {

class Window final {
public:
    explicit Window(const std::string& title, GLint width, GLint height);
    ~Window();

    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    auto size() const -> std::pair<int, int> ;

    std::string title() const;

private:
    GLFWwindow* m_window;
};

}
