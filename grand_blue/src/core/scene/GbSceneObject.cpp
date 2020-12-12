///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "GbSceneObject.h"

// Standard Includes

// External

// Project
#include "GbScene.h"
#include "GbScenario.h"
#include "../GbCoreEngine.h"
#include "../resource/GbResourceCache.h"

#include "../rendering/view/GbPointLightCamera.h"

#include "../components/GbTransformComponent.h"
#include "../components/GbComponent.h"
#include "../components/GbScriptComponent.h"
#include "../components/GbShaderComponent.h"
#include "../components/GbCameraComponent.h"
#include "../components/GbLightComponent.h"
#include "../components/GbModelComponent.h"
#include "../components/GbListenerComponent.h"
#include "../components/GbPhysicsComponents.h"
#include "../components/GbCanvasComponent.h"
#include "../components/GbAnimationComponent.h"
#include "../components/GbCubeMapComponent.h"
#include "../components/GbAudioSourceComponent.h"
#include "../processes/GbScriptedProcess.h"

#include "../rendering/shaders/GbShaders.h"
#include "../rendering/lighting/GbShadowMap.h"
#include "../rendering/renderer/GbRenderCommand.h"
#include "../rendering/renderer/GbMainRenderer.h"

#include "../events/GbEventManager.h"
#include "../readers/GbJsonReader.h"


#include "../utils/GbMemoryManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Gb {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class Implementations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Scene Object
std::shared_ptr<SceneObject> SceneObject::create(std::shared_ptr<Scene> scene)
{
    // Create scene object and add to node map
    std::shared_ptr<SceneObject> sceneObjectPtr = prot_make_shared<SceneObject>(scene);
    sceneObjectPtr->addToNodeMap();

    // Add to scene
    sceneObjectPtr->setScene(scene);
    scene->addObject(sceneObjectPtr);

    // Add transform to scene object
    sceneObjectPtr->m_transformComponent = std::make_shared<TransformComponent>(sceneObjectPtr);

    // Set unique name
    static size_t count = 0;
    static GString name("SceneObject_");
    sceneObjectPtr->setName(name + GString::FromNumber(count));
    count++;

    return sceneObjectPtr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<SceneObject> SceneObject::create(CoreEngine * core, const QJsonValue & json)
{
    // Get scene from name
    const QJsonObject& jsonObject = json.toObject();
    std::shared_ptr<Scene> scene;
    if (jsonObject.contains("scene")) {
        QString sceneName = jsonObject["scene"].toString();
        scene = core->scenario()->getSceneByName(sceneName);
    }
    else if (core->scenario()->getScenes().size() == 1) {
        scene = core->scenario()->getScenes().front();
    }
    else {
#ifdef DEBUG_MODE
        throw("Error, invalid SceneObject json");
#endif
        qDebug() << ("create::Error, invalid SceneObjectjson");
    }

    // Initialize scene object from scene and load from JSON
    std::shared_ptr<SceneObject> sceneObject = SceneObject::create(scene);
    sceneObject->loadFromJson(json);
    return sceneObject;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<SceneObject> SceneObject::get(const Uuid & uuid)
{
    if (DagNode::DagNodeMap().find(uuid) != DagNode::DagNodeMap().end()) {
        return std::static_pointer_cast<SceneObject>(DagNode::DagNodeMap().at(uuid));
    }

    return nullptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<SceneObject> SceneObject::get(const GString & uuidStr)
{
    Uuid uuid = Uuid(uuidStr);
    if (DagNode::DagNodeMap().find(uuid) != DagNode::DagNodeMap().end()) {
        return std::static_pointer_cast<SceneObject>(DagNode::DagNodeMap().at(uuid));
    }

    return nullptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<SceneObject> SceneObject::getByName(const GString & name)
{
    static QString soStr = QStringLiteral("SceneObject");

    auto iter = std::find_if(DagNode::DagNodeMap().begin(), DagNode::DagNodeMap().end(),
        [&](const std::pair<Uuid, std::shared_ptr<DagNode>>& dagPair) {
        if (dagPair.second->className() == soStr) {
            auto sceneObject = std::static_pointer_cast<SceneObject>(dagPair.second);
            return sceneObject->getName() == name;
        }
        return false;
    });

    if (iter != DagNode::DagNodeMap().end()) {
        return std::static_pointer_cast<SceneObject>(iter->second);
    }

    return nullptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SceneObject::~SceneObject()
{
    // Set scene to nullptr, so that components know the object is being deleted
    m_scene = nullptr;

    // Delete all components
    for (const std::vector<Component*>& componentVec : m_components) {
        for (auto& component : componentVec) {
            delete component;
        }
    }

    //logInfo("Deleting " + m_name);

    emit m_engine->eventManager()->deletedSceneObject(m_uuid);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<Scene> & SceneObject::scene() const {
    return m_scene;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ShaderComponent* SceneObject::shaderComponent() const
{
    if (hasComponent(Component::ComponentType::kShader)) {
        ShaderComponent* renderComp = static_cast<ShaderComponent*>(m_components[(int)Component::ComponentType::kShader][0]);
        return renderComp;
    }
    return nullptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CameraComponent* SceneObject::camera() const
{
    if (hasComponent(Component::ComponentType::kCamera)) {
        CameraComponent* camera = static_cast<CameraComponent*>(m_components[(int)Component::ComponentType::kCamera][0]);
        return camera;
    }
    return nullptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LightComponent* SceneObject::light() const
{
    if (hasComponent(Component::ComponentType::kLight)) {
        LightComponent* light = static_cast<LightComponent*>(m_components[(int)Component::ComponentType::kLight][0]);
        return light;
    }
    return nullptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CubeMapComponent * SceneObject::cubeMap() const
{
    if (hasComponent(Component::ComponentType::kCubeMap)) {
        CubeMapComponent* cubemap = static_cast<CubeMapComponent*>(m_components[(int)Component::ComponentType::kCubeMap][0]);
        return cubemap;
    }
    return nullptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::setCamera(CameraComponent* camera)
{
    camera->setSceneObject(std::dynamic_pointer_cast<SceneObject>(sharedPtr()));
    setComponent(camera);

    // Need to recompute view matrix with scene object position
    m_transformComponent->computeWorldMatrix();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CharControlComponent * SceneObject::characterController() const
{
    if (hasComponent(Component::ComponentType::kCharacterController)) {
        CharControlComponent* cc = static_cast<CharControlComponent*>(m_components[(int)Component::ComponentType::kCharacterController][0]);
        return cc;
    }
    return nullptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ModelComponent * SceneObject::modelComponent() const
{
    if (hasComponent(Component::ComponentType::kModel)) {
        ModelComponent* m = static_cast<ModelComponent*>(m_components[(int)Component::ComponentType::kModel][0]);
        return m;
    }
    return nullptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CanvasComponent * SceneObject::canvasComponent() const
{
    if (hasComponent(Component::ComponentType::kCanvas)) {
        CanvasComponent* c = static_cast<CanvasComponent*>(m_components[(int)Component::ComponentType::kCanvas][0]);
        return c;
    }
    return nullptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BoneAnimationComponent * SceneObject::boneAnimationComponent() const
{
    if (hasComponent(Component::ComponentType::kBoneAnimation)) {
        BoneAnimationComponent* ba = static_cast<BoneAnimationComponent*>(m_components[(int)Component::ComponentType::kBoneAnimation][0]);
        return ba;
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::createDrawCommands(SceneCamera& camera, 
    MainRenderer& renderer, 
    const SortingLayer& currentLayer,
    bool overrideLayer)
{
    // Skip if no shader component or does not contain render layer
    ShaderComponent* sc = shaderComponent();
    bool hasLayer = hasRenderLayer(currentLayer.getName());
    if (sc && (hasLayer || overrideLayer)) {
        if (sc->isEnabled()) {
            // If shader component is not enabled, don't need to draw anything!
            std::shared_ptr<ShaderPreset> shaderPreset = sc->shaderPreset();
            if (shaderPreset) {
                // Grab some settings from shader preset
                //Renderer& r = shaderPreset->renderer();
                const std::shared_ptr<ShaderProgram>& shaderProgram = shaderPreset->shaderProgram();
                RenderSettings& renderSettings = shaderPreset->renderSettings();

                if (shaderProgram) {
                    // Create render commands
                    ModelComponent* modelComp = modelComponent();
                    CanvasComponent* canvasComp = canvasComponent();
                    std::vector<std::shared_ptr<DrawCommand>> renderCommands;
                    if (modelComp) {
                        modelComp->createDrawCommands(renderCommands,
                            camera,
                            *shaderProgram,
                            shaderPreset->prepassShaderProgram().get());
                    }
                    if (canvasComp) {
                        canvasComp->createDrawCommands(renderCommands,
                            camera,
                            *shaderProgram,
                            shaderPreset->prepassShaderProgram().get());
                    }

                    // Set uniforms in render commands
#ifdef DEBUG_MODE
                    if (!scene()) {
                        throw("Error, object must be in scene to be rendered");
                    }
#endif
                    for (const std::shared_ptr<DrawCommand>& command : renderCommands) {
                        // Set uniforms for the scene
                        scene()->bindUniforms(*command);

                        // Set uniforms for scene object
                        bindUniforms(*command);

                        // Set uniforms for shader preset
                        command->addUniforms(shaderPreset->uniforms());

                        // Set render settings
                        // Note, shader preset overrides any sub-settings
                        command->renderSettings().overrideSettings(renderSettings);

                        // Set render layer of command
                        command->setRenderLayer(&const_cast<SortingLayer&>(currentLayer));

                        // Add command to renderer
                        renderer.addRenderCommand(command);
                    }
                }
            }
        }
    }

    // Generate commands for children
    for (const std::shared_ptr<SceneObject>& child : children()) {
        child->createDrawCommands(camera, renderer, currentLayer, overrideLayer);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<SortingLayer>> SceneObject::getRenderLayers() const
{
    // Get render layers
    std::vector<std::shared_ptr<SortingLayer>> layers;
    for (std::weak_ptr<SortingLayer> weakPtr : m_renderLayers) {
        if (std::shared_ptr<SortingLayer> ptr = weakPtr.lock()) {
            layers.push_back(ptr);
        }
    }

    return layers;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool SceneObject::hasRenderLayer(const GString & label)
{
    //auto layers = renderLayers();
    auto iter = std::find_if(m_renderLayers.begin(), m_renderLayers.end(),
        [&](const auto& layer) {
        if (std::shared_ptr<SortingLayer> ptr = layer.lock()) {
            return ptr->getName() == label;
        }
        else {
            return false;
        }
    });
    if (iter == m_renderLayers.end()) {
        return false;
    }
    else {
        return true;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::createDrawCommands(ShadowMap & sm, MainRenderer & renderer)
{
    // TODO: Clean this up with sub-routines
    // Skip if no shader component or does not contain render layer
    ShaderComponent* sc = shaderComponent();
    if (sc) {
        if (sc->isEnabled()) {
            // If shader component is not enabled, don't need to draw anything!
            std::shared_ptr<ShaderPreset> shaderPreset = sc->shaderPreset();
            if (shaderPreset) {
                // Grab some settings from shader preset
                const std::shared_ptr<ShaderProgram>& shaderProgram = shaderPreset->shaderProgram();
                RenderSettings& renderSettings = shaderPreset->renderSettings();

                if (shaderProgram) {
                    // Create render commands
                    ModelComponent* modelComp = modelComponent();
                    std::vector<std::shared_ptr<DrawCommand>> renderCommands;
                    if (modelComp) {
                        ShaderProgram* prepassShader;
                        if (sm.lightComponent()->getLightType() != Light::kPoint) {
                            // Directional and spot lights are relatively straightforward, just one camera
                            prepassShader = shaderPreset->prepassShaderProgram().get();

                        }
                        else {
                            // Get cubemap prepass shader for point lights
                            prepassShader = m_engine->resourceCache()->getHandleWithName("prepass_shadowmap", 
                                Resource::kShaderProgram)->resourceAs<ShaderProgram>(false).get();
                        }
                        modelComp->createDrawCommands(renderCommands,
                            *sm.camera(),
                            *shaderProgram,
                            prepassShader);

                        // Set uniforms in render commands and add to renderer
                        for (const auto& command : renderCommands) {
                            // Set uniforms for the scene
                            scene()->bindUniforms(*command);

                            // Set uniforms for scene object
                            bindUniforms(*command);

                            // Set uniforms for shader preset
                            command->addUniforms(shaderPreset->uniforms());

                            // Set render settings
                            command->renderSettings().overrideSettings(renderSettings);

                            // Add command to renderer
                            renderer.addShadowMapCommand(command);
                        }
                    }
                }
            }
        }
    }

    // Generate commands for children
    for (const std::shared_ptr<SceneObject>& child : children()) {
        child->createDrawCommands(sm, renderer);
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<SortingLayer>> SceneObject::renderLayers()
{
    // Get render layers
    std::vector<std::shared_ptr<SortingLayer>> layers;
    for (std::weak_ptr<SortingLayer> weakPtr : m_renderLayers) {
        if (std::shared_ptr<SortingLayer> ptr = weakPtr.lock()) {
            layers.push_back(ptr);
        }
    }

    // Clear any layers that have gone out of scope
    if (layers.size() < m_renderLayers.size()) {
        m_renderLayers.clear();
        for (const std::shared_ptr<SortingLayer>& ptr : layers) {
            m_renderLayers.push_back(ptr);
        }
    }

    return layers;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool SceneObject::addRenderLayer(const std::shared_ptr<SortingLayer>& layer)
{
    std::vector<std::shared_ptr<SortingLayer>> layers = renderLayers();
    auto iter = std::find_if(layers.begin(), layers.end(),
        [&](const auto& l) {
        return l->getUuid() == layer->getUuid();
    });
    if (iter != layers.end()) {
#ifdef DEBUG_MODE
        throw("Error, layer already found");
#endif
        return false;
    }
    m_renderLayers.push_back(layer);
    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool SceneObject::removeRenderLayer(const GString & label)
{
    auto iter = std::find_if(m_renderLayers.begin(), m_renderLayers.end(),
        [&](const auto& layer) {
        std::shared_ptr<SortingLayer> layerPtr = layer.lock();
        return layerPtr->getName() == label;
    });
    if (iter == m_renderLayers.end()) {
#ifdef DEBUG_MODE
        throw("Error, layer not found");
#endif
        return false;
    }

    m_renderLayers.erase(iter);
    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::clearModels()
{
    components().at((int)Component::ComponentType::kModel).clear();

    for (std::shared_ptr<SceneObject>& child : children()) {
        child->clearModels();
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::updatePhysics()
{
    if (hasComponent(Component::ComponentType::kRigidBody)) {
        for (auto& comp : m_components[(int)Component::ComponentType::kRigidBody]) {
            RigidBodyComponent* rigidBodyComp = static_cast<RigidBodyComponent*>(comp);
            rigidBodyComp->updateTransformFromPhysics();
        }
    }

    for (const std::shared_ptr<SceneObject>& child : m_children) {
        child->updatePhysics();
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::abortScriptedProcesses()
{
    if (!hasComponent(Component::ComponentType::kPythonScript)) return;
    
    for (Component*& component : m_components.at((int)Component::ComponentType::kPythonScript)) {
        // Cast script component
        ScriptComponent* scriptComponent = static_cast<ScriptComponent*>(component);

        // Abort the process
        if(!scriptComponent->process()->isAborted())
        scriptComponent->process()->abort();
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Component * SceneObject::getComponent(const Uuid & uuid, Component::ComponentType type)
{
    std::vector<Component*>& components = m_components[(int)type];
    auto iter = std::find_if(components.begin(), components.end(),
        [&](const auto& component) {
        return component->getUuid() == uuid;
    });

    if (iter != components.end()) {
        return *iter;
    }
    else {
        return nullptr;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SceneObject::addComponent(Component*  component)
{
    if (canAdd(component)) {
        if (hasComponent(component->getComponentType())) {
            //if (m_components.at(component->getComponentType()).count(component->getUuid().asString())) {
            //    throw("Error, component with this UUID already on scene object");
            //}
            m_components.at((int)component->getComponentType()).push_back(component);
        }
        else {
            setComponent(component);
        }
        return true;
    }
    else{
#ifdef DEBUG_MODE
        throw("Error, failed to add component");
#endif
        delete component;
        return false;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SceneObject::removeComponent(Component * component)
{
    if (hasComponent(component->getComponentType())) {
        std::vector<Component*>& componentVec = m_components.at((int)component->getComponentType());
        std::vector<Component*>::iterator iC = std::find_if(componentVec.begin(),
            componentVec.end(),
            [&](const auto& comp) {return comp->getUuid() == component->getUuid(); });
        if (iC != componentVec.end()) {
            delete *iC;
            componentVec.erase(iC);
            return true;
        }
        else {
#ifdef DEBUG_MODE
            throw("Error, component does not exist for removal from Scene Object");
#endif
            return false;
        }
        return true;
    }
    else {
#ifdef DEBUG_MODE
        throw("Error, component does not exist for removal from Scene Object");
#endif
        return false;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SceneObject::canAdd(Component * component)
{
    if (component->isSceneComponent()) return false;

    Component::ComponentType type = component->getComponentType();

    // Return false if component is a transform type
    if (type == Component::ComponentType::kTransform) {
#ifdef DEBUG_MODE
        logWarning("canAdd:: Warning, cannot add additional transforms to a scene object");
#endif
        return false;
    }

    // Return false if the scene object doesn't have the requisite components to add this one
    if (!satisfiesConstraints(type)) return false;

    // Check component type
    switch (type) {
    case Component::ComponentType::kCamera:
    case Component::ComponentType::kLight:
    case Component::ComponentType::kPythonScript:
    case Component::ComponentType::kShader:
    case Component::ComponentType::kModel:
    case Component::ComponentType::kListener:
    case Component::ComponentType::kRigidBody:
    case Component::ComponentType::kCanvas:
    case Component::ComponentType::kCharacterController:
    case Component::ComponentType::kBoneAnimation:
    case Component::ComponentType::kCubeMap:
    case Component::ComponentType::kAudioSource:
    case Component::ComponentType::kAudioListener:
        break;
    case Component::ComponentType::kTransform:
#ifdef DEBUG_MODE
        logError("canAdd:: Warning, cannot add additional transforms to a scene object");
#endif
    default:
#ifdef DEBUG_MODE
        throw("canAdd:: Warning, failed to add component to scene object");
#endif
        return false;
    }

    bool hasComponentType = hasComponent(type);
    if (!hasComponentType) {
        // Return true if no component of the given type was found
        return true;
    }

    int numComponents = m_components[(int)type].size();
    int maxAllowed = component->maxAllowed();
    if((numComponents >= maxAllowed) && (maxAllowed > 0)){
        // Return false if component exceeds max allowed per SceneObject
        return false;
    }
    else {
        return true;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SceneObject::satisfiesConstraints(const Component::ComponentType & type) const
{
    bool satisfies = true;
    Component::Constraints reqs = Component::GetRequirements(type);
    for (const std::pair<Component::ComponentType, bool>& typePair: reqs.m_constraints) {
        if (typePair.second)
            // Constraint is to have a component
            satisfies &= hasComponent(typePair.first);
        else
            // Constraint is to not have a component
            satisfies &= !hasComponent(typePair.first);
    }
    return satisfies;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<SceneObject> SceneObject::getChild(const Uuid & uuid) const
{
    std::shared_ptr<SceneObject> chosenChild = nullptr;
    for (const std::shared_ptr<SceneObject>& child : children()) {
        if (child->getUuid() == uuid) {
            chosenChild = child;
            break;
        }
        else {
            chosenChild = child->getChild(uuid);
            if (chosenChild)
                break;
        }
    }

    return chosenChild;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<SceneObject> SceneObject::getChildByName(const GString & name) const
{
    std::vector<std::shared_ptr<SceneObject>> kids = children();
    std::shared_ptr<SceneObject> chosenChild = nullptr;
    for (std::shared_ptr<SceneObject>& child : kids) {
        if (child->getName() == name) {
            chosenChild = child;
            break;
        }
        else {
            chosenChild = child->getChildByName(name);
            if (chosenChild)
                break;
        }
    }

    return chosenChild;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::switchScene(std::shared_ptr<Scene> newScene)
{
    // Remove object from its current scene
    if (scene()) {
        scene()->removeObject(std::static_pointer_cast<SceneObject>(sharedPtr()));
    }

    // Add to new scene
    newScene->addObject(std::static_pointer_cast<SceneObject>(sharedPtr()));
    setScene(newScene);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<SceneObject> SceneObject::parent() const
{
    if (hasParents()) {
        for (const auto& parentPair : m_parents) {
            if (std::shared_ptr<DagNode> parent = parentPair.second.lock()) {
                return std::static_pointer_cast<SceneObject>(parent);
            }
            else {
                throw("Error, parent pointer no longer valid");
            }
        }
        return nullptr;
    }else{
        return nullptr;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::setParent(const std::shared_ptr<SceneObject>& newParent)
{
    if (parent()) {
        removeParent(parent()->getUuid());
    }

    if (hasParents()) {
        throw("Error, scene object still has parents");
    }

    if (newParent) {
        // The parent is being added
        addParent(newParent);
    }
    else {
        // The parent is being removed
        // Add to top-level item list if not present
        auto thisShared = std::static_pointer_cast<SceneObject>(sharedPtr());
        if (!m_scene->hasTopLevelObject(thisShared)) {
            m_scene->addObject(thisShared);
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SceneObject::hasComponent(Component::ComponentType type) const
{
    if (m_components[(int)type].size() == 0) {
        return false;
    }
    else {
        return true;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SceneObject::hasCamera() const
{
    return hasComponent(Component::ComponentType::kCamera);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SceneObject::hasShaderComponent() const
{
    return hasComponent(Component::ComponentType::kShader);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::removeFromScene()
{
    // Abort all scripted processes before removal
    abortScriptedProcesses();

    // Remove from parent, if there is one
    if (parent()) {
        parent()->removeChild(this);
    }

    // Remove from static DAG node map
    scene()->removeObject(std::static_pointer_cast<SceneObject>(sharedPtr()), true);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AudioSourceComponent * SceneObject::audioSource(size_t index) const
{
    size_t audioIndex = (size_t)Component::ComponentType::kAudioSource;
    const std::vector<Component*>& audioComps = m_components[audioIndex];
    if (!audioComps.size()) {
        return nullptr;
    }

    if (audioComps.size() - 1 < index) {
        throw("Error, not enough audio components to retrieve at specified index");
        return nullptr;
    }

    return static_cast<AudioSourceComponent*>(audioComps[index]);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QJsonValue SceneObject::asJson() const
{
    QJsonObject object = QJsonObject();
    QJsonArray components;
    for (const auto& componentVec : m_components) {
        for (const auto& component: componentVec) {
            // Skip if component was created via a script
            if (component->isPythonGenerated()) continue;

            // Append to components JSON
            components.append(component->asJson());
        }
    }
    object.insert("name", m_name.c_str());
    object.insert("components", components);
    object.insert("transform", m_transformComponent->asJson());
    if(m_scene)
        object.insert("scene", scene()->getName().c_str());

    // Render layers
    QJsonArray layers;
    for (const auto& layer : getRenderLayers()) {
        layers.append(layer->getName().c_str());
    }
    object.insert("renderLayers", layers);

    // Add child objects to json
    QJsonArray kids;
    for (const auto& child : children()) {
        kids.append(child->asJson());
    }
    object.insert("children", kids);

    return object;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::loadFromJson(const QJsonValue& json, const SerializationContext& context)
{
    Q_UNUSED(context);

    // Set scene object name
    const QJsonObject& jsonObject = json.toObject();
    if (jsonObject.contains("name")) {
        m_name = jsonObject.value("name").toString();
    }

    // Add scene object components
    if (jsonObject.contains("transform")) {
        m_transformComponent->loadFromJson(jsonObject.value("transform"));
    }

    // Add scene object components
    if (jsonObject.contains("components")) {
        // Set up component arrays
        QJsonArray remaining = jsonObject.value("components").toArray();
        QJsonArray remainingCache = QJsonArray();

        // Get pointer to this scene object 
        std::shared_ptr<SceneObject> thisSceneObject;
        if (scene()) {
            thisSceneObject = scene()->getSceneObject(getUuid());
        }
        else {
            thisSceneObject = std::static_pointer_cast<SceneObject>(DagNode::DagNodeMap().at(m_uuid));
        }

        // iterate over components to construct
        while (remaining.size()) {
            for (const auto& componentJson : remaining) {
                // Get component type
                QJsonObject componentJsonObject = componentJson.toObject();
                Component::ComponentType componentType = Component::ComponentType(componentJsonObject.value("type").toInt());

                QString jsonStr = JsonReader::ToQString(componentJsonObject);
                // Check if scene object satisfies type requirements
                if (!satisfiesConstraints(componentType)) {
                    remainingCache.append(componentJsonObject);
                    continue;
                }

                Component::create(thisSceneObject, componentJsonObject);
            } 

            remaining.swap(remainingCache);
            remainingCache = QJsonArray();
        } // End while
    }

    // Load render layers
    m_renderLayers.clear();
    if (jsonObject.contains("renderLayers")) {
        m_renderLayers.clear();
        QJsonArray layers = jsonObject["renderLayers"].toArray();
        CoreEngine* engine = CoreEngine::engines().begin()->second;
        for (const auto& layerJson : layers) {
            QString layerName = layerJson.toString();
            Vec::EmplaceBack(m_renderLayers,
                engine->scenario()->settings().renderLayer(layerName));
        }
    }

    // Load child scene objects
    if (jsonObject.contains("children")) {
        for (const auto& childJson : jsonObject.value("children").toArray()) {
            // Get pointer to this object's scene and use it to instantiate a child object
            std::shared_ptr<SceneObject> child = SceneObject::create(m_scene);
            child->setParent(SceneObject::get(m_uuid));
            child->loadFromJson(childJson.toObject());
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::onAddChild(const std::shared_ptr<SceneObject>& child)
{
    // Don't need to do anything with transforms, since onAddParent covers it
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::onAddParent(const std::shared_ptr<SceneObject>& parent)
{
    // Set transform's parent
    m_transformComponent->setParent(parent->transform().get());

    // Remove from top-level item list if present
    auto thisShared = std::static_pointer_cast<SceneObject>(sharedPtr());
    if (m_scene->hasTopLevelObject(thisShared)) {
        m_scene->removeObject(thisShared);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::onRemoveChild(const std::shared_ptr<SceneObject>& child)
{
    // Don't need to do anything with transforms, since onRemoveParent covers it
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::onRemoveParent(const std::shared_ptr<SceneObject>& parent)
{
    m_transformComponent->clearParent();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::setComponent(Component * component)
{
    // Delete all components of the given type
    Component::ComponentType type = component->getComponentType();
    if (hasComponent(type)) {
        for (const auto& comp : m_components.at((int)type)) {
            delete comp;
        }
    }
    // Replace components
    m_components[(int)type] = { component };
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::bindUniforms(DrawCommand& rendercommand)
{
    // Set world matrix uniform 
    rendercommand.setUniform(Shader::s_worldMatrixUniformName, m_transformComponent->worldMatrix());
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void SceneObject::setDefaultRenderLayers()
{
    // TODO: Configure these from a file
    Scenario* scenario = scene()->scenario();
    m_renderLayers.emplace_back(scenario->settings().renderLayer("skybox"));
    m_renderLayers.emplace_back(scenario->settings().renderLayer("world"));
    m_renderLayers.emplace_back(scenario->settings().renderLayer("effects"));
    m_renderLayers.emplace_back(scenario->settings().renderLayer("ui"));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SceneObject::SceneObject():
    m_isEnabled(true)
{
    m_components.resize((int)Component::ComponentType::MAX_COMPONENT_TYPE_VALUE);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SceneObject::SceneObject(const std::shared_ptr<Scene>& scene):
    DagNode(),
    m_isEnabled(true)
{
    if (scene) { 
        m_scene = scene; 
        m_engine = scene->engine();
        if(m_scene->scenario()){
            // Check since debug scene has no scenario
            setDefaultRenderLayers();
        }
    }
    else {
#ifdef DEBUG_MODE
        throw("Error, scene object must be instantiated with a scene");
#endif
    }
    m_components.resize((int)Component::ComponentType::MAX_COMPONENT_TYPE_VALUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespacing
