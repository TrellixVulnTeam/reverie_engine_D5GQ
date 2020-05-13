#ifndef GB_ANIMATION_COMPONENT
#define GB_ANIMATION_COMPONENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard Includes

// Qt
#include <QString>
#include <QJsonValue>

// Project
#include "GbComponent.h"
#include "../rendering/GbGLFunctions.h"
#include "../mixins/GbRenderable.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Gb {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Model;
class ShaderProgram;
struct Uniform;
class AnimationController;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @class BoneAnimationComponent
class BoneAnimationComponent: public Component{
public:
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Static Methods
    /// @{

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Constructors/Destructor
    /// @{
    BoneAnimationComponent();
    BoneAnimationComponent(const BoneAnimationComponent & other);
    BoneAnimationComponent(const std::shared_ptr<SceneObject>& object);
    ~BoneAnimationComponent();

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Public Methods
    /// @{

    /// @brief Set uniforms for the model component, which are specific to only this instance of a model
    void bindUniforms(const std::shared_ptr<ShaderProgram>&  shaderProgram);

    /// @brief Enable the behavior of this component
    virtual void enable() override;

    /// @brief Disable the behavior of this component
    virtual void disable() override;

    /// @brief Max number of allowed components per scene object
    virtual int maxAllowed() const override { return 1; }

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Properties
    /// @{

    /// @property Model
    std::shared_ptr<Model> model() const;

    std::unique_ptr<AnimationController>& animationController() {
        return m_animationController;
    }

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name GB object Properties
    /// @{
    /// @property className
    const char* className() const override { return "BoneAnimationComponent"; }

    /// @property namespaceName
    const char* namespaceName() const override { return "Gb::BoneAnimationComponent"; }

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
    /// @name Protected Methods
    /// @{

    /// @brief Initialize animation controller from model
    void initializeAnimationController();
       
    /// @}
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected Members
    /// @{

    /// @brief The animations used by this model component
    std::unique_ptr<AnimationController> m_animationController = nullptr;

    /// @}


};
Q_DECLARE_METATYPE(BoneAnimationComponent)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespacing

#endif 