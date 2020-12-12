#include "GbScriptComponent.h"

#include "../GbCoreEngine.h"
#include "../resource/GbResourceCache.h"

#include "../loop/GbSimLoop.h"
#include "../processes/GbProcessManager.h"
#include "../scripting/GbPythonScript.h"
#include "../processes/GbScriptedProcess.h"

#include "../scene/GbScene.h"
#include "../scene/GbScenario.h"
#include "../scene/GbSceneObject.h"

namespace Gb {
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Gb::ScriptComponent::ScriptComponent(const std::shared_ptr<SceneObject>& object):
    Component(object, ComponentType::kPythonScript)
{
    setSceneObject(sceneObject());
    sceneObject()->addComponent(this);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Gb::ScriptComponent::~ScriptComponent()
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScriptComponent::reset()
{
    // Pause  process
    bool wasPaused = m_scriptProcess->isPaused();
    if (!wasPaused) {
        m_scriptProcess->pause();
    }

    // Reload script
    m_script->reload();
    m_scriptProcess->refresh();

    // Unpause process
    if (!wasPaused) {
        m_scriptProcess->unPause();
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScriptComponent::initializeBehavior(const QString& filepath)
{
    // Return if no filepath
    QFile file(filepath);
    if (!file.exists()) {
#ifdef DEBUG_MODE
        logWarning("Warning, filepath to script not found:" + filepath);
#endif
        return;
    }

    // Set script for this component
    // Note: Adds script to the scenario (and python) if not present
    m_path = filepath;
    m_script = m_engine->resourceCache()->guaranteeHandleWithPath(m_path, 
        Resource::kPythonScript)->resourceAs<PythonClassScript>();

    // Start the process for this script component
    m_scriptProcess = std::make_shared<ScriptedProcess>(
        m_engine,
        this,
        m_engine->simulationLoop()->processManager().get());
    m_engine->simulationLoop()->processManager()->attachProcess(
        m_scriptProcess);

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScriptComponent::enable()
{
    //if (m_scriptProcess) {
    //    m_scriptProcess->unPause();
    //}
    Component::enable();
}
//////////////// ///////////////////////////////////////////////////////////////////////////////////////////////////////
void ScriptComponent::disable()
{
    //if (m_scriptProcess) {
    //    m_scriptProcess->pause();
    //}
    Component::disable();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QJsonValue ScriptComponent::asJson() const
{
    QJsonObject object = Component::asJson().toObject();
    object.insert("path", m_path);
    return object;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScriptComponent::loadFromJson(const QJsonValue& json, const SerializationContext& context)
{
    Component::loadFromJson(json, context);
    const QJsonObject& object = json.toObject();
    if (object.contains("path")) {
        m_path = object.value("path").toString();
    }
    if (m_path != "") {
        initializeBehavior(m_path);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespacing