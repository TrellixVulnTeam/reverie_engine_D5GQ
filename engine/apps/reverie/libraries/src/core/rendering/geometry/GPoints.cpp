#include "core/rendering/geometry/GPoints.h"

#include "core/rendering/geometry/GLines.h"
#include <core/rendering/view/GViewport.h>
#include "core/GCoreEngine.h"
#include "core/rendering/geometry/GPolygon.h"
#include "core/rendering/shaders/GShaderProgram.h"
#include "core/rendering/buffers/GUniformBufferObject.h"
#include "core/resource/GResourceCache.h"
#include "core/resource/GResourceHandle.h"
#include "core/rendering/geometry/GMesh.h"
#include "core/rendering/geometry/GSkeleton.h"
#include "core/rendering/geometry/GPolygon.h"
#include "heave/kinematics/GTransform.h"
#include "core/rendering/renderer/GRenderCommand.h"
#include "core/rendering/renderer/GOpenGlRenderer.h"
#include "core/rendering/renderer/GRenderContext.h"

namespace rev {


Points::Points(UniformContainer& uc) :
    m_pointSize(0.01f),
    m_pointColor(0.8f, 0.3f, 0.2f, 1.0f)
{
    //m_mesh = std::make_shared<Mesh>();
    m_renderSettings.setShapeMode(PrimitiveMode::kPoints);
    m_renderSettings.addDefaultBlend();

    // Set uniforms
    m_uniforms.m_pointSize.setValue(m_pointSize, uc);
    m_uniforms.m_pointColor.setValue(m_pointColor, uc);
}

Points::Points(ResourceCache& cache, const Skeleton & skeleton, Flags<ResourceBehaviorFlag> flags) :
    Points(cache.engine()->openGlRenderer()->renderContext().uniformContainer())
{
    m_renderSettings.setShapeMode(PrimitiveMode::kPoints);
    m_renderSettings.addDefaultBlend();

    // Load vertex data for skeleton bones
    // TODO: Clean this up
    initializeEmptyMesh(cache, flags);
    RenderContext& context = m_meshHandle->engine()->openGlRenderer()->renderContext();
    Mesh* mesh = Points::mesh();

    uint32_t numBones = (uint32_t)skeleton.boneNodes().size();
    m_vertexData.get<MeshVertexAttributeType::kPosition>().resize(numBones);
    m_vertexData.get<MeshVertexAttributeType::kIndices>().resize(numBones);
    m_vertexData.get<MeshVertexAttributeType::kMiscInt>().resize(numBones);
    std::iota(m_vertexData.get<MeshVertexAttributeType::kIndices>().begin(), m_vertexData.get<MeshVertexAttributeType::kIndices>().end(), 0);
    int count = 0;
    for (auto& vec : m_vertexData.get<MeshVertexAttributeType::kMiscInt>()) {
        vec[0] = count;
        count++;
    }
    mesh->postConstruction(ResourcePostConstructionData{ false, &m_vertexData });
}

Points::Points(ResourceCache& cache, size_t numPoints, Flags<ResourceBehaviorFlag> flags) :
    m_pointSize(0.01f),
    m_pointColor(0.8f, 0.3f, 0.2f, 1.0f)
    //m_transform(std::make_unique<Transform>())
{
    m_renderSettings.setShapeMode(PrimitiveMode::kPoints);
    m_renderSettings.addDefaultBlend();

    // Load vertex data for skeleton bones
    // TODO: Clean this up
    // Note, only really need the number of points (bones) in the skeleton)
    initializeEmptyMesh(cache, flags);
    Mesh* mesh = Points::mesh();

    UniformContainer& uc = cache.engine()->openGlRenderer()->renderContext().uniformContainer();
    m_uniforms.m_pointSize.setValue(m_pointSize, uc);
    m_uniforms.m_pointColor.setValue(m_pointColor, uc);

    m_vertexData.get<MeshVertexAttributeType::kPosition>().resize(numPoints);
    m_vertexData.get<MeshVertexAttributeType::kIndices>().resize(numPoints);
    m_vertexData.get<MeshVertexAttributeType::kMiscInt>().resize(numPoints);
    std::iota(m_vertexData.get<MeshVertexAttributeType::kIndices>().begin(), m_vertexData.get<MeshVertexAttributeType::kIndices>().end(), 0);
    int count = 0;
    for (auto& vec : m_vertexData.get<MeshVertexAttributeType::kMiscInt>()) {
        vec[0] = count;
        count++;
    }
    RenderContext& context = m_meshHandle->engine()->openGlRenderer()->renderContext();
    mesh->postConstruction(ResourcePostConstructionData{ false, &m_vertexData });
}

Points::Points(const Lines & lines):
    m_pointColor(lines.m_lineColor),
    m_pointSize(0.01f)
    //m_transform(std::make_unique<Transform>())
{
    m_meshHandle = lines.m_meshHandle;
    m_renderSettings.setShapeMode(PrimitiveMode::kPoints);
    m_renderSettings.addDefaultBlend();

    UniformContainer& uc = m_meshHandle->engine()->openGlRenderer()->renderContext().uniformContainer();
    m_uniforms.m_pointSize.setValue(m_pointSize, uc);
    m_uniforms.m_pointColor.setValue(m_pointColor, uc);
}

Points::~Points()
{
}

void Points::setPointSize(float size, UniformContainer& uc)
{
    m_pointSize = size;
    m_uniforms.m_pointSize.setValue(m_pointSize, uc);
}

void Points::setPointColor(const Vector4& color, UniformContainer& uc)
{
    m_pointColor = color;
    m_uniforms.m_pointColor.setValue(m_pointColor, uc);
}

void Points::setUniforms(DrawCommand & drawCommand) const
{
    ShaderProgram* shader = drawCommand.shaderProgram();
    ShaderProgram* prepassShader = drawCommand.prepassShaderProgram();

    /// @todo Set this only when screen size changes, or even move this uniform out of points object
    UniformContainer& uc = shader->handle()->engine()->openGlRenderer()->renderContext().uniformContainer();
    m_uniforms.m_screenPixelWidth.setValue((float)Viewport::ScreenDimensions().x(), uc);

    drawCommand.addUniform(
        m_uniforms.m_pointColor,
        shader->uniformMappings().m_color,
        prepassShader ? prepassShader->uniformMappings().m_color : -1
    );
    drawCommand.addUniform(
        m_uniforms.m_pointSize,
        shader->uniformMappings().m_pointSize,
        prepassShader ? prepassShader->uniformMappings().m_pointSize : -1
    );

    drawCommand.addUniform(
        m_uniforms.m_screenPixelWidth,
        shader->uniformMappings().m_screenPixelWidth,
        prepassShader ? prepassShader->uniformMappings().m_screenPixelWidth : -1
    );
}

size_t Points::numPoints() const
{
    Mesh* mesh = Points::mesh();
    return m_vertexData.get<MeshVertexAttributeType::kPosition>().size();
}

void Points::loadVertexData(ResourceCache& cache, const MeshVertexAttributes& vertexData, Flags<ResourceBehaviorFlag> flags)
{
    // Initialize a new mesh
    initializeEmptyMesh(cache, flags);
    m_meshHandle->setIsLoading(true);
    
    // Load data
    RenderContext& context = m_meshHandle->engine()->openGlRenderer()->renderContext();
    Mesh* mesh = Points::mesh();
    for (size_t i = 0; i < vertexData.get<MeshVertexAttributeType::kIndices>().size(); i++) {
        int point = vertexData.get<MeshVertexAttributeType::kIndices>()[i];
        addPoint(m_vertexData, vertexData.get<MeshVertexAttributeType::kPosition>()[point]);
    }
    mesh->postConstruction(ResourcePostConstructionData{ false, &m_vertexData });
    m_meshHandle->setIsLoading(false);
}

void Points::reload()
{    
    assert(false && "Deprecate this");
}

void to_json(json& orJson, const Points& korObject)
{
    ToJson<Renderable>(orJson, korObject);
}

void from_json(const json& korJson, Points& orObject)
{
    FromJson<Renderable>(korJson, orObject);
}

void Points::bindUniforms(ShaderProgram& shaderProgram)
{
    Renderable::bindUniforms(shaderProgram);

	if (!Ubo::GetCameraBuffer()) return;
}

void Points::releaseUniforms(ShaderProgram& shaderProgram)
{
    Q_UNUSED(shaderProgram)
}

void Points::drawGeometry(ShaderProgram& shaderProgram, 
    RenderSettings * settings)
{
    Q_UNUSED(shaderProgram)

    // TODO: Move this to a RenderSetting
    glEnable(GL_PROGRAM_POINT_SIZE); // Enable point sizing
    //glEnable(GL_POINT_SMOOTH); // Make points actual circles
    //glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    Mesh* mesh = Points::mesh();
    if (!mesh) {
        return;
    }
    mesh->vertexData().drawGeometry(settings->shapeMode(), 1);
}

void Points::addPoint(MeshVertexAttributes& vertexData, const Vector3 & point)
{
    // Set next point to this point for vertex attributes already stored
    int size = (int)vertexData.get<MeshVertexAttributeType::kPosition>().size();

    // Add vertex position
    Vec::EmplaceBack(vertexData.get<MeshVertexAttributeType::kPosition>(), point);

    // Add indices 
    vertexData.get<MeshVertexAttributeType::kIndices>().push_back(size);
}


} // End namespaces
