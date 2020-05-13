#ifndef GB_COMPONENT_H
#define GB_COMPONENT_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard Includes

// External
#include <QMetaType>

// Project
#include "../GbObject.h"
#include "../mixins/GbLoadable.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Gb {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SceneObject;
class Scene;
class CoreEngine;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** @class Component
    @brief  A component to be attached to a Scene or SceneObject
*/
class Component: public Object, public Serializable {
public:
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Static
    /// @{

    enum ComponentType {
        kNone = -1,
        kTransform,
        kRenderer,
        kCamera,
        kLight,
        kPythonScript,
        kStateMachine,
        kModel,
        kListener,
        kRigidBody,
        kPhysicsScene,
        kCanvas,
        kCharacterController,
        kBoneAnimation,
        MAX_COMPONENT_TYPE_VALUE
    };

    enum ParentType {
        kScene,
        kSceneObject
    };

    /// @brief The required component types to create this component
    struct Requirements {
        bool isEmpty() const { return m_requiredTypes.size() == 0; }
        std::set<ComponentType> m_requiredTypes;
    };

    static Requirements GetRequirements(ComponentType type);

    /// @brief Creat a component from its type
    static Component* create(const std::shared_ptr<SceneObject>& object, ComponentType type);
    static Component* create(const std::shared_ptr<SceneObject>& object, const QJsonObject& json);

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Constructors/Destructor
    /// @{
    Component(ComponentType type, bool isScene = false);
    Component(CoreEngine* engine, ComponentType type, bool isScene = false);
    Component(const Component& other);
    Component(const std::shared_ptr<SceneObject>& object, ComponentType type);
    Component(const std::shared_ptr<Scene>& object, ComponentType type);
    virtual ~Component();
    
    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Public Methods
    /// @{

    /// @brief Toggle this component
    void toggle(int enable);

    /// @brief Whether this component is enabled or not
    virtual bool isEnabled() const {
        return m_isEnabled;
    }

    /// @brief Enable this component
    virtual void enable(){
        m_isEnabled = true;
    }

    /// @brief Disable this component
    virtual void disable(){
        m_isEnabled = false;
    }

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Properties
    /// @{

    bool isSceneComponent() const { return m_isSceneComponent; }

    /// @brief Get type of component
    ComponentType getComponentType() const { return m_type; }

    /// @brief Get parent type of the component
    ParentType parentType() const {
        if (sceneObject())
            return kSceneObject;
        else
            return kScene;
    }

    /// @brief Obtain pointer to scene object
    std::shared_ptr<SceneObject> sceneObject() const;

    /// @brief Obtain pointer to scene
    std::shared_ptr<Scene> scene() const;

    /// @brief Set the scene object
    void setSceneObject(const std::shared_ptr<SceneObject>& object);

    /// @brief Set the scene object
    void setScene(std::shared_ptr<Scene> object);

    /// @brief Max number of allowed components per scene object
    /// @details If left at -1, there is no limit
    virtual int maxAllowed() const { return -1; }

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name GB object Properties
    /// @{
    /// @property className
    const char* className() const override { return "Component"; }

    /// @property namespaceName
    const char* namespaceName() const override { return "Gb::Component"; }
    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Serializable overrides
    /// @{

    /// @brief Outputs this data as a valid json string
    virtual QJsonValue asJson() const override;

    /// @brief Populates this data using a valid json string
    virtual void loadFromJson(const QJsonValue& json) override;

    /// @}

protected:
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected Members
    /// @{


    /// @brief Map of component requirements, indexed by type
    static std::unordered_map<ComponentType, Requirements> TypeRequirements;

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected Members
    /// @{

    /// @brief If component is to be attached to a scene rather than a scene object
    bool m_isSceneComponent;

    /// @brief Whether component is enabled or disabled
    bool m_isEnabled;

    /// @brief Type of component
    ComponentType m_type;

    /// @brief Weak pointer to scene or object that owns this component
    /// @note Not used by all subclasses
    std::weak_ptr<Object> m_object;

    /// @brief Pointer to the core engine
    CoreEngine* m_engine;

    /// @}


};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespacing

#endif 