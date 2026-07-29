
#include <veil/window.hpp>

namespace veil {

Window::Window(std::string_view title, int width, int height) {

    m_loopFunc = nullptr;

    if (!glfwInit())
        throw veil::Exception("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to create GLFW window"));
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);

    #ifdef _WIN32
    veil::initOpenGLDriver(); // WIN32 glad needs this macro to run explicitely in the .dll code
                              // or else glad function pointers will be 0x0
                              // THE CODE HAS NOT BEEN TESTED ON LINUX YET
    #endif 

    glViewport(0, 0, width, height);
}

Window::~Window() {
    
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void Window::setUpdateCallback(std::function<void()>&& loopFunc) {

    m_loopFunc = std::move(loopFunc);
}
void Window::setMouseCallback(std::function<void(double, double)>&& mouseFunc) {

    m_mouseFunc = std::move(mouseFunc);
    glfwSetCursorPosCallback(m_window, &Window::mouseCallback);
}
void Window::setFramebufferCallback(std::function<void()>&& framebufferFunc) {

    m_framebufferFunc = std::move(framebufferFunc);
    glfwSetFramebufferSizeCallback(m_window, &Window::framebufferCallback);
}
void Window::setKeyCallback(std::function<void(const KeyDownArray&)>&& keyFunc) {
    
    m_keyFunc = std::move(keyFunc);
    glfwSetKeyCallback(m_window, &Window::keyCallback);
}

void Window::startUpdateLoop() {

    if (!m_loopFunc)
        return;
    
    glfwSwapInterval(1);

    while (!this->shouldClose()) {

        this->pollEvents();

        if (m_keyFunc)
            m_keyFunc(m_keysDown);

        m_loopFunc();

        this->swapBuffers();

    }
}

float Window::getAspectRatio() const {
    
    int w, h;
    getSize(w, h);
    return (h > 0) ? static_cast<float>(w) / h : 1.0f;
}

void Window::mouseCallback(GLFWwindow* window, double xpos, double ypos) {

    const Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (win && win->m_mouseFunc)
        win->m_mouseFunc(xpos, ypos);
}
void Window::framebufferCallback(GLFWwindow* window, int width, int height) {

    const Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

    glViewport(0, 0, width, height);

    if (win && win->m_framebufferFunc)
        win->m_framebufferFunc();
}
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key < 0 || key > GLFW_KEY_LAST)
        return;

    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS)
        win->m_keysDown[key] = true;
    else if (action == GLFW_RELEASE)
        win->m_keysDown[key] = false;
}

}; //namespace veil