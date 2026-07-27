
#include "veil/glrenderer.hpp"

namespace veil {

void GLRenderer::setTargets(std::initializer_list<std::pair<const Shader&, const ModelInstance&>> targets) {

    for (const auto& targ : targets) 
        m_renderData[&targ.first].push_back(&targ.second);
}

void GLRenderer::setTargets(std::initializer_list<std::pair<const Shader&, const InstancedModels&>> targets) {

    for (const auto& targ : targets)
        m_instancedRenderData[&targ.first].push_back(&targ.second);
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

    if (!m_renderData.empty())
        for (const auto& data : m_renderData) {

            const auto& shader = data.first;
            const auto& models = data.second;

            for (const auto& model : models) {

                if (m_forModelFunc)
                    m_forModelFunc(shader, model);
            
                model->render(*shader);
            }
        }
    if (!m_instancedRenderData.empty())
        for (const auto& data : m_instancedRenderData) {

            const auto& shader = data.first;
            const auto& instancedModels = data.second;

            for (const auto& instancedModel : instancedModels) {

                instancedModel->render(*shader);
            }
        }
}

};