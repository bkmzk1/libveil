
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

template<typename T>
static auto toProvider(T&& v) {
    
    if constexpr (std::is_invocable_v<std::decay_t<T>>) 
        return std::forward<T>(v);
    else
        return [value = std::forward<T>(v)] { return value; };
}   

class VEIL_EXPORT Renderer {
    public:
        Renderer() = default;
        ~Renderer() = default;

        void setForTargetCallback(std::function<void(const ShaderProgram*, const IDrawable*)>&& forTargetFunc);

        void reserveShaders(const std::vector<const ShaderProgram*>& shaders);
        void addTargets(std::initializer_list<std::pair<const ShaderProgram&, const IDrawable&>> targets);
        void changeTargetShader(std::initializer_list<std::pair<const ShaderProgram&, const IDrawable&>> targets);

        template<typename... Args>
        void uploadUniformBuffers(Args&&... targets) {

            (addUniformBufferSetter(std::forward<Args>(targets)), ...);
        }

        template<typename T> 
        void uploadUniform(const ShaderProgram& shader, std::string_view uniformName, T&& v) {
            
            GLint location = glGetUniformLocation(shader.getID(), uniformName.data());

            if (location == -1)
                throw veil::Exception(
                    Log::message(LogType::CRITICAL, "No uniform '{}' found in program {}", uniformName, shader.getID())
                );

            auto provider = toProvider(std::forward<T>(v));

            std::function<void(const ShaderProgram&, GLint)> setter =
                [provider](const ShaderProgram& shader, GLint location) {
                    shader.setUniform(location, provider());
                };
            addUniformSetter(shader, location, std::move(setter));
        }
        template<typename T>
        void uploadUniformUniversal(std::string_view uniformName, const T& v) {

            for (const auto& data : m_renderData) {
                
                data.first->useProgram();
                uploadUniform(*data.first, uniformName, v);
            }
        }
        template<typename T>
        void uploadUniformDirect(const ShaderProgram& shader, std::string_view uniformName, const T& v) {
            
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
        std::function<void(const ShaderProgram*, const IDrawable*)> m_forTargetFunc;

        std::unordered_map<
            const ShaderProgram*, 
            std::vector<const IDrawable*> 
        > m_renderData;

        std::unordered_map<
            const ShaderProgram*, 
            std::vector<std::pair<GLint, std::function<void(const ShaderProgram&, GLint)> > >
        > m_uniformData;

        std::unordered_map<
            const UniformBuffer*,
            std::function<void(const UniformBuffer*)>
        > m_uniformBufferData;

        template<typename T>
        void addUniformBufferSetter(std::pair<const UniformBuffer*, T>&& target) {

            auto provider = toProvider(std::forward<T>(target.second));

            std::function<void(const UniformBuffer*)> setter =
                [provider](const UniformBuffer* ubo) {
                    ubo->setValue(provider());
                };
            m_uniformBufferData.try_emplace(target.first, std::move(setter));
        }

        void addUniformSetter(const ShaderProgram& shader,GLint loc,std::function<void(const ShaderProgram&, GLint)> setter);
}; //class Renderer

}; //namespace veil