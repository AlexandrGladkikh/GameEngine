#include "Window.h"

#include <cstring>

namespace engine {

Window::Window(const std::string& title, GLint width, GLint height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);

    auto window_size = size();
    glViewport(0, 0, window_size.first, window_size.second);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto window_ptr = static_cast<Window*>(glfwGetWindowUserPointer(window));
        window_ptr->m_key.push_back(key);
        window_ptr->m_action.push_back(action);
    });
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::setKeyInputHandler(const std::function<void(int, int)>& handler)
{
    m_keyInputHandler = handler;
}

void Window::update(uint64_t dt)
{
    glfwPollEvents();

    if (m_keyInputHandler) {
        for (size_t i = 0; i < m_key.size(); ++i) {
            m_keyInputHandler(m_key[i], m_action[i]);
        }
        m_key.clear();
        m_action.clear();
    }
}

void Window::swapBuffer()
{
    glfwSwapBuffers(m_window);
}

auto Window::size() const -> std::pair<int, int>
{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return std::make_pair(width, height);
}

auto Window::title() const -> std::string
{
    return glfwGetWindowTitle(m_window);
}

}
