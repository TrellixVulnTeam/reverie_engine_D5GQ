#include "GbLoadProcess.h"
#include "GbProcessManager.h"

#include "../utils/GbMemoryManager.h"

#include "../GbCoreEngine.h"
#include "../events/GbEventManager.h"
#include "../resource/GbResource.h"
#include "../resource/GbResourceCache.h"

#include "../sound/GbAudioResource.h"
#include "../resource/GbImage.h"
#include "../rendering/materials/GbMaterial.h"
#include "../rendering/materials/GbCubeTexture.h"
#include "../readers/models/GbModelReader.h"
#include "../rendering/shaders/GbShaders.h"
#include "../scripting/GbPythonScript.h"

#include "../../view/GbWidgetManager.h"
#include "../../view/GL/GbGLWidget.h"

namespace Gb {

/////////////////////////////////////////////////////////////////////////////////////////////
LoadProcess::LoadProcess(CoreEngine* engine, const std::shared_ptr<ResourceHandle>& resourceHandle):
    ThreadedProcess(engine, engine->resourceCache()->processManager()),
    m_resourceHandle(resourceHandle)
{
    // Set flag on handle to denote that resource is being loaded
    resourceHandle->setIsLoading(true);
}
/////////////////////////////////////////////////////////////////////////////////////////////
LoadProcess::~LoadProcess()
{
}
/////////////////////////////////////////////////////////////////////////////////////////////
void LoadProcess::onInit()
{
    QMutexLocker locker(&m_resourceHandle->mutex());

    // Call parent class initialization
    ThreadedProcess::onInit();

    // Was quitting if resource had already been loaded, but now can be reloaded
    if (m_resourceHandle->resource(false)) {
        //fail();
#ifdef DEBUG_MODE
        logMessage("Reloading resource", LogLevel::Warning);
#endif
        //return;
    }

    // Load the resource
    std::shared_ptr<Resource> resource = nullptr;
    switch (m_resourceHandle->getResourceType()) {
    case Resource::kImage:
        resource = loadImage();
        break;
    case Resource::kTexture:
        resource = loadTexture();
        break;
    case Resource::kMaterial:
        resource = loadMaterial();
        break;
    case Resource::kMesh:
        resource = loadMesh();
        break;
    case Resource::kAnimation:
        resource = loadAnimation();
        break;
    case Resource::kModel:
        resource = loadModel();
        break;
    case Resource::kShaderProgram:
        resource = loadShaderProgram();
        break;
    case Resource::kPythonScript:
        resource = loadPythonScript();
        break;
    case Resource::kCubeTexture:
        resource = loadCubeTexture();
        break;
    case Resource::kAudio:
        resource = loadAudio();
        break;
    default:
#ifdef DEBUG_MODE
        logError("This resource type is not implemented");
        throw("This resource type is not implemented");
#endif
        break;
    }

    // Add resource to handle
    m_resourceHandle->setResource(resource, false);

    // Touch resource handle
    m_engine->resourceCache()->resourceMapMutex().lock();
    m_resourceHandle->touch();
    m_engine->resourceCache()->resourceMapMutex().unlock();

    // Determine process success or failure, terminate either way
    if (resource) {
        succeed();
    }
    else {
        fail();
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
void LoadProcess::onSuccess()
{
    // Don't emit post-construction signal if this resource is a child
    if (!m_resourceHandle->isChild()) {
        emit m_engine->resourceCache()->doneLoadingResource(m_resourceHandle);
    }
    ThreadedProcess::onSuccess();
} 
/////////////////////////////////////////////////////////////////////////////////////////////
void LoadProcess::onFail()
{
    ThreadedProcess::onFail();

#ifdef DEBUG_MODE
    logMessage("Error, failed to load resource");
#endif
}
/////////////////////////////////////////////////////////////////////////////////////////////
void LoadProcess::onAbort()
{
    ThreadedProcess::onAbort();
}
/////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Image> LoadProcess::loadImage()
{
    // TODO: UNTESTED
    QImage::Format format = QImage::Format(m_resourceHandle->resourceJson()["format"].toInt(0));
    std::shared_ptr<Image> image = std::make_shared<Image>(m_resourceHandle->getPath(), format);

    return image;
}
/////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Resource> LoadProcess::loadTexture()
{
    const GString& texPath = m_resourceHandle->getPath();

    std::shared_ptr<Texture> texture = prot_make_shared<Texture>(texPath);
    int typeInt = m_resourceHandle->resourceJson()["texUsageType"].toInt(0);
    TextureUsageType type = TextureUsageType(typeInt);
    texture->setUsageType(type);

    // TODO: Make this a flag
    texture->generateMipMaps(true); // Set texture to generate mip maps
    return texture;
}
/////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Resource> LoadProcess::loadModel()
{
    std::shared_ptr<Model> model;
    
    // Read in from json if possible
    const QJsonObject& object = m_resourceHandle->resourceJson();
    if (!object.isEmpty()) {
        // For user-created models, load from JSON
        if (!m_resourceHandle->isUserGenerated()) {
            throw("Error, for now, JSON loading is only supported for user-generated models");
        }
        model = std::make_shared<Model>(*m_resourceHandle); // Load from JSON in post-construction
    }
    else {
        // For models direct-from-file, use ModelReader
        auto reader = std::make_shared<ModelReader>(m_engine->resourceCache(), *m_resourceHandle);
        m_resourceHandle->setReader(reader);
        model = reader->loadModel();
    }

    return model;
}
/////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Resource> LoadProcess::loadMesh()
{
    // Create mesh if it is a polygon
    const GString& name = m_resourceHandle->getName();
    if (PolygonCache::isPolygonName(name)) {
        auto mesh = m_engine->resourceCache()->polygonCache()->createPolygon(name, m_resourceHandle);
        return mesh;
    }
    else {
        // Independent mesh load is not yet supported
        throw("Not implemented");
        return nullptr;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Resource> LoadProcess::loadCubeTexture()
{
    //static QMutex cubeTextureMutex;
    //QMutexLocker lock(&cubeTextureMutex);

    const GString& filePath = m_resourceHandle->getPath();
    std::shared_ptr<CubeTexture> cubeTexture = std::make_shared<CubeTexture>(filePath);
    return cubeTexture;
}
/////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Resource> LoadProcess::loadMaterial()
{
    std::shared_ptr<Material> material;

    // Read in from json if data provided
    if (!m_resourceHandle->resourceJson().isEmpty()) {
        material = prot_make_shared<Material>();
        m_resourceHandle->setResource(material, false); // Need to set before loading from JSON
        material->loadFromJson(m_resourceHandle->resourceJson(), { m_engine });
    }
    else {
        throw("Material loading directly from file not implemented");
    }

    return material;
}
/////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Resource> LoadProcess::loadAnimation()
{
    throw("Not implemented");
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Resource> LoadProcess::loadShaderProgram()
{    
    // Initialize path variables
    GString vertPath, fragPath, geomPath, compPath;
    if (m_resourceHandle->additionalPaths().size() == 2) {
        vertPath = m_resourceHandle->getPath();
        fragPath = m_resourceHandle->additionalPaths().front();
        geomPath = m_resourceHandle->additionalPaths()[1];

#ifdef DEBUG_MODE
        if (m_engine->resourceCache()->getTopLevelHandleWithPath(vertPath)->isConstructed()) {
            throw("Error, attempting to load a shader program that's already been loaded");
        }
#endif
    }
    else if (m_resourceHandle->additionalPaths().size() == 1) {
        vertPath = m_resourceHandle->getPath();
        fragPath = m_resourceHandle->additionalPaths().front();

#ifdef DEBUG_MODE
        if (m_engine->resourceCache()->getTopLevelHandleWithPath(vertPath)->isConstructed()) {
            throw("Error, attempting to load a shader program that's already been loaded");
        }
#endif
    }
    else if (m_resourceHandle->additionalPaths().size() == 0) {
        compPath = m_resourceHandle->getPath();

#ifdef DEBUG_MODE
        if (m_engine->resourceCache()->getTopLevelHandleWithPath(compPath)->isConstructed()) {
            throw("Error, attempting to load a shader program that's already been loaded");
        }
#endif
    }
    else {
        throw("Unsupported number of paths");
    }

    std::shared_ptr<ShaderProgram> shaderProgram;
    if (!m_resourceHandle->resourceJson().isEmpty()) {
        // Load from json if possible
        shaderProgram = std::make_shared<ShaderProgram>(m_resourceHandle->resourceJson());
    }
    else {
        if (compPath.isEmpty()) {
            // Is not a compute shader
            shaderProgram = std::make_shared<ShaderProgram>(vertPath, fragPath, geomPath);
        }
        else {
            // Is a compute shader
            shaderProgram = std::make_shared<ShaderProgram>(compPath);
        }
    }

    shaderProgram->release();
    return shaderProgram;
}
/////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Resource> LoadProcess::loadPythonScript()
{
    // TODO: Search for file using a FileManager
    // Throw error if script already added
    //ResourceCache& cache = *m_engine->resourceCache();
#ifdef DEBUG_MODE
    if (m_resourceHandle->isConstructed()) {
        throw("Error, attempting to reload a python script");
    }
#endif

    // Check if filepath exists
    const GString& filepath = m_resourceHandle->getPath();
    if (FileReader::fileExists(filepath)) {
        std::shared_ptr<PythonClassScript> script;
        if (!m_resourceHandle->resourceJson().isEmpty())
            // Load from JSON if possible
            script = std::make_shared<PythonClassScript>(m_engine, m_resourceHandle->resourceJson());
        else
            script = std::make_shared<PythonClassScript>(m_engine, filepath);

        return script;
    }

#ifdef DEBUG_MODE
    logError("Failed to load file at path " + filepath);
    throw("Failed to load file");
#endif
    return nullptr;

}
/////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Resource> LoadProcess::loadAudio()
{
    // TODO: Catch thread errors
    // https://stackoverflow.com/questions/25282620/catching-exception-from-worker-thread-in-the-main-thread
    const QJsonObject& resourceJson = m_resourceHandle->resourceJson();
    if (!resourceJson.contains("sourceType")) {
        throw("Error, no audio type specified");
    }

    const GString& audioPath = m_resourceHandle->getPath();
    AudioResource::SourceType sourceType = AudioResource::SourceType(resourceJson["sourceType"].toInt());
    std::shared_ptr<AudioResource> audio = prot_make_shared<AudioResource>(audioPath, sourceType);
    audio->loadAudioSource(audioPath);

    // Update source with JSON attributes
    audio->audioSourceSettings().loadFromJson(resourceJson["sourceSettings"]);
    audio->cacheSettings();

    return audio;
}

/////////////////////////////////////////////////////////////////////////////////////////////
} // End namespaces