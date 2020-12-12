#include "GbPhysicsSceneComponent.h"

#include "../GbCoreEngine.h"

#include "../scene/GbScene.h"
#include "../scene/GbScenario.h"
#include "../scene/GbSceneObject.h"

#include "../physics/GbPhysicsManager.h"
#include "../physics/GbPhysicsScene.h"

namespace Gb {
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PhysicsSceneComponent::PhysicsSceneComponent() :
    Component(ComponentType::kPhysicsScene, true),
    m_physicsScene(nullptr)
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PhysicsSceneComponent::PhysicsSceneComponent(const PhysicsSceneComponent & component) :
    Component(component.scene(), ComponentType::kPhysicsScene),
    m_physicsScene(nullptr)
{
    initialize();
    scene()->addComponent(this);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PhysicsSceneComponent::PhysicsSceneComponent(std::shared_ptr<Scene> object, const QJsonValue & json) :
    Component(object, ComponentType::kPhysicsScene),
    m_physicsScene(nullptr)
{
    loadFromJson(json);
    scene()->addComponent(this);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PhysicsSceneComponent::PhysicsSceneComponent(std::shared_ptr<Scene> object) :
    Component(object, ComponentType::kPhysicsScene),
    m_physicsScene(nullptr)
{
    initialize();
    scene()->addComponent(this);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Gb::PhysicsSceneComponent::~PhysicsSceneComponent()
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PhysicsSceneComponent::enable()
{
    Component::enable();
}
//////////////// ///////////////////////////////////////////////////////////////////////////////////////////////////////
void PhysicsSceneComponent::disable()
{
    Component::disable();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QJsonValue PhysicsSceneComponent::asJson() const
{
    QJsonObject object = Component::asJson().toObject();
    object.insert("physicsScene", m_physicsScene->asJson());
    return object;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PhysicsSceneComponent::loadFromJson(const QJsonValue& json, const SerializationContext& context)
{
    Component::loadFromJson(json, context);
    const QJsonObject& object = json.toObject();

    // Load physics scene from json
    if (!m_physicsScene) {
        initialize();
    }
    m_physicsScene->loadFromJson(object.value("physicsScene"));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PhysicsSceneComponent::initialize()
{
    // Create physics scene
    m_physicsScene = PhysicsScene::create(scene());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespacing