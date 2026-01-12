#include "Window.h"
#include "Logger.h"

#include <cstring>

namespace engine {

Window::Window(const std::string& title, GLint width, GLint height, GLint gl_version_major, GLint gl_version_minor)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, gl_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, gl_version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto window_size = size();
    glViewport(0, 0, window_size.first, window_size.second);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto window_ptr = static_cast<Window*>(glfwGetWindowUserPointer(window));
        window_ptr->m_key.push_back(key);
        window_ptr->m_key_action.push_back(action);
    });
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int key, int action, int mods) {
        Logger::info("glfwSetMouseButtonCallback");
        auto window_ptr = static_cast<Window*>(glfwGetWindowUserPointer(window));
        window_ptr->m_mouse_key.push_back(key);
        window_ptr->m_mouse_key_action.push_back(action);
        double x_pos;
        double y_pos;
        glfwGetCursorPos(window, &x_pos, &y_pos);
        window_ptr->m_mouse_pos.push_back({x_pos, window_ptr->size().second - y_pos});
    });
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::setWindowPosition(int x, int y)
{
    glfwSetWindowPos(m_window, x, y);
}

void Window::setKeyInputHandler(const std::function<void(int, int)>& handler)
{
    m_keyInputHandler = handler;
}

void Window::setMouseInputHandler(const std::function<void(int, int, int, int)>& handler)
{
    m_mouseInputHandler = handler;
}

void Window::update(uint64_t dt)
{
    glfwPollEvents();

    if (m_keyInputHandler) {
        for (size_t i = 0; i < m_key.size(); ++i) {
            m_keyInputHandler(m_key[i], m_key_action[i]);
        }
        m_key.clear();
        m_key_action.clear();
    }

    if (m_mouseInputHandler) {
        for (size_t i = 0; i < m_mouse_key.size(); ++i) {
            Logger::info("for (size_t i = 0; i < m_mouse_key.size(); ++i)");
            const auto mouse_pos = m_mouse_pos[i];
            m_mouseInputHandler(m_mouse_key[i], m_mouse_key_action[i], mouse_pos.first, mouse_pos.second);
        }
        m_mouse_key.clear();
        m_mouse_key_action.clear();
        m_mouse_pos.clear();
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
