
#include "veil/glrenderer.hpp"

namespace veil {

void GLRenderer::reserveShaders(const std::vector<const Shader*>& shaders) {

    for (const auto& shader : shaders) 
        m_renderData.try_emplace(shader, 0);
}

void GLRenderer::addTargets(std::initializer_list<std::pair<const Shader&, const util::IDrawable&>> targets) {

    for (const auto& target : targets) {

        const Shader* shader = &target.first;
        const util::IDrawable* drawable = &target.second;

        m_renderData[shader].push_back(drawable);
        drawable->setCurrentShader(shader);
    }
}
void GLRenderer::changeTargetShader(std::initializer_list<std::pair<const Shader&, const util::IDrawable&>> targets) {

    for (const auto& target : targets) {

        const util::IDrawable* drawable = &target.second;
        const Shader* newShader = &target.first;
        const Shader* oldShader = drawable->getCurrentShader();

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

void GLRenderer::setForModelCallback(std::function<void(const Shader*, const ModelInstance*)>&& forModelFunc) {

    m_forModelFunc = std::move(forModelFunc);
}

void GLRenderer::callbackUniforms() const {

    if (m_uniformData.empty())
        return;

    for (const auto& [shader, uniforms] : m_uniformData) {

        shader->useProgram();

        for (const auto& [location, setter] : uniforms)
            setter(*shader, location);
    }
}

void GLRenderer::callbackRender() const {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& data : m_renderData) {

        const auto& shader = data.first;
        const auto& models = data.second;

        shader->useProgram();

        for (const auto& model : models) {

            if (m_forModelFunc && model->getType() == util::DrawableType::MODEL_SINGULAR)
                m_forModelFunc(shader, static_cast<const ModelInstance*>(model));
            
            model->render();
        }
    }
}

};