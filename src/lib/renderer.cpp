
#include "../../include/veil/renderer.hpp"

namespace veil {

void initRenderingFlags() {

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); 
}

void Renderer::reserveShaders(const std::vector<const ShaderProgram*>& shaders) {

    for (const auto& shader : shaders) 
        m_renderData.try_emplace(shader, 0);
}

void Renderer::addTargets(std::initializer_list<std::pair<const ShaderProgram&, const IDrawable&>> targets) {

    for (const auto& target : targets) {

        const ShaderProgram* shader = &target.first;
        const IDrawable* drawable = &target.second;

        m_renderData[shader].push_back(drawable);
        drawable->setCurrentShader(shader);
    }
}
void Renderer::changeTargetShader(std::initializer_list<std::pair<const ShaderProgram&, const IDrawable&>> targets) {

    for (const auto& target : targets) {

        const IDrawable* drawable = &target.second;
        const ShaderProgram* newShader = &target.first;
        const ShaderProgram* oldShader = drawable->getCurrentShader();

        if (newShader && newShader != oldShader) {
            
            auto& models = m_renderData[oldShader];
            auto it = std::find(models.begin(), models.end(), drawable);
            
            if (it != models.end()) {
                std::iter_swap(it, models.end() - 1);
                models.pop_back();
            }
        }
        m_renderData[newShader].push_back(drawable);
        drawable->setCurrentShader(newShader);
    }
}

void Renderer::setForTargetCallback(std::function<void(const ShaderProgram*, const IDrawable*)>&& forModelFunc) {

    m_forTargetFunc = std::move(forModelFunc);
}

void Renderer::callbackUniforms() const {

    if (!m_uniformBufferData.empty()) {

        for (const auto& [ubo, setter] : m_uniformBufferData) {

            setter(ubo);
        }
    }

    if (!m_uniformData.empty()) {

        for (const auto& [shader, uniforms] : m_uniformData) {

            shader->useProgram();

            for (const auto& [location, setter] : uniforms)
                setter(*shader, location);
        }
    }
}

void Renderer::callbackRender() const {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& data : m_renderData) {

        const auto& shader = data.first;
        const auto& targets = data.second;

        shader->useProgram();

        for (const auto& target : targets) {

            if (m_forTargetFunc)
                m_forTargetFunc(shader, target);
            
            target->render();
        }
    }
}

void Renderer::addUniformSetter(
                    const ShaderProgram& shader, 
                    GLint location, 
                    std::function<void(const ShaderProgram&, GLint)> setter) 
{
    m_uniformData[&shader].push_back({location, std::move(setter)});
}

};