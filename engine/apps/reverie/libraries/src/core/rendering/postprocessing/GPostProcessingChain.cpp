#include "core/rendering/postprocessing/GPostProcessingChain.h"
#include "core/rendering/postprocessing/GPostProcessingEffect.h"
#include "core/rendering/shaders/GShaderProgram.h"
#include "core/rendering/shaders/GShaderPreset.h"
#include "core/components/GCameraComponent.h"
#include "core/rendering/view/GFrameBuffer.h"
#include "core/rendering/renderer/GRenderContext.h"

#include "core/scene/GSceneObject.h"
#include "core/GCoreEngine.h"
#include "core/resource/GResourceCache.h"

namespace rev {


// PostProcessingChain

PostProcessingChain::PostProcessingChain(SceneCamera * camera, RenderContext* context):
    m_camera(camera),
    m_readBufferIndex(0)
{
    if (!context->isCurrent()) {
        Logger::Throw("Error, context is not current");
    }
    initialize(context);
}

PostProcessingChain::~PostProcessingChain()
{
#ifdef DEBUG_MODE
    bool error = gl::OpenGLFunctions::printGLError("Failed before deleting framebuffer");
    if (error) {
        Logger::LogError("Failed before deleting framebuffer");
    }
#endif

    m_effects.clear();
    m_pingPongBuffers.clear();
}

void PostProcessingChain::clearBuffers()
{
    for (const auto& buffer : m_pingPongBuffers) {
        buffer->bind();
        buffer->clear();
        buffer->release();
    }
}

void PostProcessingChain::pingPongBuffers()
{
    m_readBufferIndex = 1 - m_readBufferIndex;
}

FrameBuffer & PostProcessingChain::readBuffer()
{
    return *m_pingPongBuffers[m_readBufferIndex];
}

FrameBuffer & PostProcessingChain::writeBuffer()
{
    return *m_pingPongBuffers[1 - m_readBufferIndex];
}

void PostProcessingChain::addEffect(const std::shared_ptr<PostProcessingEffect>& effect)
{
    m_effects.push_back(effect);
}

void PostProcessingChain::postProcess()
{
    // Populate first ping-pong framebuffer with image from scene ================================
    // Clear framebuffer
    FrameBuffer& rb = readBuffer();
    rb.bind();
    rb.clear();
    rb.release();

    // Blit scene texture into read buffer from camera buffer
    // Was re-rendering, but instead of re-render to quad, blit from scene framebuffer, which will already have been drawn
    m_camera->frameBuffers().writeBuffer().blit(FrameBuffer::BlitMask::kColorBit, rb);

    // Apply post-processing effects =====================================================================
    for (const std::shared_ptr<PostProcessingEffect>& effect : m_effects) {
        effect->applyEffect(*this);
    }

    // TODO: Avoid blit back into camera buffer
    // Perform final blit to set effects on camera's framebuffer
    readBuffer().blit(FrameBuffer::BlitMask::kColorBit, m_camera->frameBuffers().writeBuffer());

    // Make sure that all framebuffers are released
    FrameBuffer::Release();
}

void PostProcessingChain::clear()
{
    m_effects.clear();
}

bool PostProcessingChain::isEmpty() const
{
    return m_effects.empty();
}

void PostProcessingChain::resize(uint32_t w, uint32_t h)
{
    for (std::shared_ptr<FrameBuffer>& buffer : m_pingPongBuffers) {
        buffer->reinitialize(w, h);
    }
    m_checkpointBuffer->reinitialize(w, h);
}

void PostProcessingChain::saveCheckPoint()
{
    // Blit read buffer into checkpoint buffer
    readBuffer().blit(FrameBuffer::BlitMask::kColorBit, *m_checkpointBuffer);
}


void to_json(json& orJson, const PostProcessingChain& korObject)
{
}

void from_json(const json& korJson, PostProcessingChain& orObject)
{
    Q_UNUSED(korJson)
    Q_UNUSED(orObject)
}

void PostProcessingChain::initialize(RenderContext* context)
{
    // Initialize checkpoint buffer
    m_checkpointBuffer = std::make_shared<FrameBuffer>(context->context(),
        AliasingType::kDefault,
        FrameBuffer::BufferAttachmentType::kTexture);

    // Initialize ping-pong framebuffers
    auto ping = std::make_shared<FrameBuffer>(context->context(),
        AliasingType::kDefault,
        FrameBuffer::BufferAttachmentType::kTexture);
    auto pong = std::make_shared<FrameBuffer>(context->context(),
        AliasingType::kDefault,
        FrameBuffer::BufferAttachmentType::kTexture);

    // Populate ping pong buffer vector
    m_pingPongBuffers.clear();
    m_pingPongBuffers.push_back(ping);
    m_pingPongBuffers.push_back(pong);
}


    
// End namespaces
}
