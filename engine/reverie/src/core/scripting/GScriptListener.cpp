#include "GScriptListener.h"

#include "../events/GEvent.h"
#include "../scene/GSceneObject.h"
#include "../scene/GScene.h"
#include "../GCoreEngine.h"
#include "../resource/GResourceCache.h"
#include "../input/GInputHandler.h"
#include "../components/GTransformComponent.h"

#include "../../view/GWidgetManager.h"
#include "../../view/GL/GGLWidget.h"

namespace rev {

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
    return m_sceneObject->scene()->engine();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
ResourceCache * ScriptListener::resourceCache()
{
    return engine()->resourceCache();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
TransformComponent * ScriptListener::transform()
{
    return &m_sceneObject->transform();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
Scene * ScriptListener::scene()
{
    return m_sceneObject->scene();
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