#include "GbScriptListener.h"

#include "../events/GbEvent.h"
#include "../scene/GbSceneObject.h"
#include "../GbCoreEngine.h"
#include "../resource/GbResourceCache.h"
#include "../input/GbInputHandler.h"
#include "../components/GbTransformComponent.h"

#include "../../view/GbWidgetManager.h"
#include "../../view/GL/GbGLWidget.h"

namespace Gb {

//////////////////////////////////////////////////////////////////////////////////////////////////////////
ScriptListener::ScriptListener(SceneObject& sceneObject):
    m_sceneObject(&sceneObject)
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
ScriptListener::~ScriptListener()
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
InputHandler * ScriptListener::inputHandler()
{
    return &(engine()->widgetManager()->mainGLWidget()->inputHandler());
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
CoreEngine * ScriptListener::engine()
{
    return m_sceneObject->engine();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
ResourceCache * ScriptListener::resourceCache()
{
    return engine()->resourceCache();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
TransformComponent * ScriptListener::transform()
{
    return m_sceneObject->transform().get();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
Scene * ScriptListener::scene()
{
    return m_sceneObject->scene().get();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ScriptListener::eventTest(const CustomEvent& ev)
{
    Q_UNUSED(ev);
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScriptListener::perform(const CustomEvent& ev)
{
    Q_UNUSED(ev);
    logInfo("Performing event EVENT EVENT");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ScriptListener::s_logMessages = true;



//////////////////////////////////////////////////////////////////////////////////////////////////////////
}