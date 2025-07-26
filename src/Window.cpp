#include "Window.h"

#include <glad/glad.h>

namespace engine {

Window::Window(const std::string& title, GLint width, GLint height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(m_window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);

    auto window_size = size();
    glViewport(0, 0, window_size.first, window_size.second);
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

auto Window::size() const
{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return std::make_pair(width, height);
}

auto Window::title() const
{
    return glfwGetWindowTitle(m_window);
}

}
