
#pragma once

#include <veil_export.h>

#include <functional>
#include <string_view>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "math.hpp"

namespace veil {

#define VEIL_INIT_OPENGL_DRV \
    if (!gladLoadGL()) \
        throw veil::Exception(veil::Log::message(veil::LogType::CRITICAL, "Failed to initialize GLAD"));

using KeyArray = std::array<bool, GLFW_KEY_LAST + 1>;

struct VEIL_EXPORT KeyEvents {
    KeyArray keysDown;
    KeyArray keysPressed;

}; //struct KeyEvents

class VEIL_EXPORT Clock {
    public:
        void tick();
        inline float getDeltaTime() const { return m_deltaTime; }

    private:
        double m_lastTime = 0.0;
        float m_deltaTime = 0.0f;
        
}; //class Clock

class VEIL_EXPORT Window {
    public:
        Window() = delete;
        Window(std::string_view title, const Vector2& windowSize);

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        ~Window(); 

        inline void setInputMode(int mode, int value) { glfwSetInputMode(m_window, mode, value); }

        void setUpdateCallback(std::function<void()>&& loopFunc);
        void setMouseCallback(std::function<void(double, double)>&& mouseFunc);
        void setFramebufferCallback(std::function<void()>&& framebufferFunc);
        void setKeyCallback(std::function<void(const KeyEvents&)>&& keyFunc);

        inline bool shouldClose() const { return glfwWindowShouldClose(m_window); }
        inline void pollEvents()  const { glfwPollEvents(); }
        inline void waitEvents()  const { glfwWaitEvents(); }
        inline void swapBuffers() const { glfwSwapBuffers(m_window); }

        int startUpdateLoop();

        inline void getSize(int& width, int& height) const { glfwGetFramebufferSize(m_window, &width, &height); };
        float getAspectRatio() const;
        inline GLFWwindow* getNativeHandle() const { return m_window; };
        inline const Clock& getClock() const { return m_clock; }

    private:
        GLFWwindow* m_window;

        Clock m_clock;

        std::function<void()> m_loopFunc;
        std::function<void(double, double)> m_mouseFunc;
        std::function<void()> m_framebufferFunc;
        std::function<void(const KeyEvents&)> m_keyFunc;

        KeyEvents m_keyEvents{.keysDown={false}, .keysPressed={false}};

        static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
        static void framebufferCallback(GLFWwindow* window, int width, int height);
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

}; //class Window

}; //namespace veil