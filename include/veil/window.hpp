
#pragma once

#include <veil_export.h>

#include <string>
#include <functional>
#include <string_view>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "logmgr.hpp"

namespace veil {

struct GLCamera;

using KeyDownArray = std::array<bool, GLFW_KEY_LAST + 1>;

inline void initOpenGLDriver() {
    
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw veil::Exception(Log::message(LogType::CRITICAL, "Failed to initialize GLAD")); 
}

class VEIL_EXPORT Window {
    public:
        Window(std::string_view title, int width = 1280, int height = 720);
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;
        ~Window(); 

        inline bool shouldClose() const { return glfwWindowShouldClose(m_window); }
        inline void pollEvents()  const { glfwPollEvents(); }
        inline void waitEvents()  const { glfwWaitEvents(); }
        inline void swapBuffers() const { glfwSwapBuffers(m_window); }

        void setUpdateCallback(std::function<void()>&& loopFunc);
        void setMouseCallback(std::function<void(double, double)>&& mouseFunc);
        void setFramebufferCallback(std::function<void()>&& framebufferFunc);
        void setKeyCallback(std::function<void(const KeyDownArray&)>&& keyFunc);

        void startUpdateLoop();

        inline void getSize(int& width, int& height) const { glfwGetFramebufferSize(m_window, &width, &height); };
        float getAspectRatio() const;
        inline GLFWwindow* getNativeHandle() const { return m_window; };

    private:
        GLFWwindow* m_window;

        std::function<void()> m_loopFunc;
        std::function<void(double, double)> m_mouseFunc;
        std::function<void()> m_framebufferFunc;
        std::function<void(const KeyDownArray&)> m_keyFunc;

        KeyDownArray m_keysDown{};

        static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
        static void framebufferCallback(GLFWwindow* window, int width, int height);
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

}; //class Window

}; //namespace veil