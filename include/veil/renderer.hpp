
#pragma once 

#include <veil_export.h>

#include <unordered_map>
#include <vector>
#include <initializer_list>
#include <functional>
#include <string_view>
#include <glad/glad.h>

#include "shader.hpp"
#include "log.hpp"
#include "drawable.hpp"

namespace veil {

VEIL_EXPORT void initRenderingFlags();

class VEIL_EXPORT Renderer {
    public:
        Renderer() = default;
        ~Renderer() = default;

        void setForTargetCallback(std::function<void(const Shader*, const IDrawable*)>&& forTargetFunc);

        void reserveShaders(const std::vector<const Shader*>& shaders);
        void addTargets(std::initializer_list<std::pair<const Shader&, const IDrawable&>> targets);
        void changeTargetShader(std::initializer_list<std::pair<const Shader&, const IDrawable&>> targets);

        template<typename T> 
        void uploadUniform(const Shader& shader, std::string_view uniformName, T&& v) {
            
            GLint location = glGetUniformLocation(shader.getID(), uniformName.data());

            if (location == -1)
                throw veil::Exception(
                    Log::message(LogType::CRITICAL, "No uniform '{}' found in program {}", uniformName, shader.getID())
                );

            if constexpr (std::is_invocable_v<std::decay_t<T>>)
                m_uniformData[&shader].push_back({
                    location, 
                    [getter = std::forward<T>(v)](const Shader& shader, GLint location) {
                        shader.setUniform(location, getter());
                    }
                });
            else 
                m_uniformData[&shader].push_back({
                    location,
                    [value = std::forward<T>(v)](const Shader& shader, GLint location) {
                        shader.setUniform(location, value);
                    }
                });
        }
        template<typename T>
        void uploadUniformUniversal(std::string_view uniformName, T&& v) {

            for (const auto& data : m_renderData) {
                
                data.first->useProgram();
                uploadUniform(*data.first, uniformName, std::move(v));
            }
        }
        template<typename T>
        void uploadUniformDirect(const Shader& shader, std::string_view uniformName, const T& v) {
            
            GLint location = glGetUniformLocation(shader.getID(), uniformName.data());

            if (location == -1)
                throw veil::Exception(
                    Log::message(LogType::CRITICAL, "No uniform '{}' found in program {}", uniformName, shader.getID())
                );
            
            shader.setUniform(location, v);
        }

        void callbackUniforms() const;
        void callbackRender() const;

        inline const auto& getRenderData() const { return m_renderData; }
        inline const auto& getUniformData() const { return m_uniformData; }

    private:
        std::unordered_map<
            const Shader*, 
            std::vector<const IDrawable*> 
        > m_renderData;

        std::unordered_map<
            const Shader*, 
            std::vector<std::pair<GLint, std::function<void(const Shader&, GLint)> > >
        > m_uniformData;

        std::function<void(const Shader*, const IDrawable*)> m_forTargetFunc;
}; //class Renderer

}; //namespace veil