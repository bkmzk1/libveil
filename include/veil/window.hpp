
#pragma once

#include <veil_export.h>

#include <string>
#include <functional>
#include <string_view>
#include <chrono>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "logmgr.hpp"

namespace veil {

struct GLCamera;

inline void initOpenGLDriver() {
    
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw veil::Exception("Failed to initialize GLAD"); 
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

        void startUpdateLoop();

        inline void getSize(int& width, int& height) const { glfwGetFramebufferSize(m_window, &width, &height); };
        float getAspectRatio() const;
        inline GLFWwindow* getNativeHandle() const { return m_window; };

    private:
        GLFWwindow* m_window;

        std::function<void()> m_loopFunc;
        std::function<void(double, double)> m_mouseFunc;
        std::function<void()> m_framebufferFunc;

        static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
        static void framebufferCallback(GLFWwindow* window, int width, int height);

}; //class Window

}; //namespace veil