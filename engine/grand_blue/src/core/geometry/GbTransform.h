#ifndef GB_TRANSFORM_H
#define GB_TRANSFORM_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard Includes

// External

// Project
#include "GbTransformComponents.h"
#include "../containers/GbContainerExtensions.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace physx {
class PxTransform;
class PxVec3;
}

namespace Gb {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Forward declarations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SceneObject;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/** @class Transform
    @brief A transform consisting of a translation, a rotation, and a scaling
*/
class Transform: public Object, public Serializable{
public:
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Static
    /// @{

    /// @brief Enum for which transforms to inherit from a parent state
    enum InheritanceType {
        kAll, // inherit entire parent world matrix
        kTranslation, // inherit only the translation of the world state (e.g. won't orbit parent)
        kPreserveOrientation // preserves the local orientation of this transform 
    };

    /// @brief Interpolate between two transforms
    static Transform interpolate(const Transform& t1,
        const Transform& t2,
        float percentFactor, 
        bool updateWorld);

    /// @brief Interpolate between a bunch of transforms with weights
    static Transform interpolate(const std::vector<Transform>& transforms,
        const std::vector<float>& weights,
        bool updateWorld);

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Constructors/Destructor
    /// @{

    Transform();
    Transform(const Vector3& position, const Quaternion& rotation, const Vector3&scale, bool updateLocal = false);
    Transform(const QJsonValue& json);
    Transform(const physx::PxTransform& pxTransform);
    //Transform(const physx::PxTransform& pxTransform, const physx::PxVec3& velocity);
    Transform(const Matrix4x4g& localMatrix);
    Transform(const Transform& transform);
    virtual ~Transform();

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Operators
    /// @{

    Transform& operator=(const Transform& other);

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Properties
    /// @{    

    /// @brief Translation component
    TranslationComponent& translation() { return m_translation; }
    const Vector3& getPosition() const { return m_translation.getPosition(); }
    //const Vector3& getVelocity() const { return m_translation.getVelocity(); }

    /// @brief Rotation component
    RotationComponent& rotation() { return m_rotation; }
    const Quaternion& getRotationQuaternion() const { return m_rotation.getQuaternion(); }

    /// @brief Scale component
    ScaleComponent& scale() { return m_scale; }
    const Vector3& getScaleVec() const { return m_scale.getScale(); }

    /// @brief Local model matrix
    const Matrix4x4g& localMatrix() const { return m_localMatrix; }

    /// @brief Global model matrix
    const Matrix4x4g& worldMatrix() const { return m_worldMatrix; }

    /// @brief Returns world matrix without scaling
    Matrix4x4g rotationTranslationMatrix() const;

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Public methods
    /// @{    

    /// @brief Rotate about the given axis, in radians
    void rotateAboutAxis(const Vector3g& axis, float angle);

    /// @brief Return as a physX transform
    physx::PxTransform asPhysX() const;

    /// @brief Update the transform manually 
    /// @details Called to avoid redundant matrix multiplications on component modification
    void updateLocalMatrix();
    void updateWorldMatrix();
    void updateWorldMatrix(const Matrix4x4g& matrix);

    /// @brief Compute world matrix
    virtual void computeWorldMatrix();

    /// @brief Obtain the parent to this transform
    Transform* parentTransform() const;
    void setParent(Transform* p);
    void clearParent();

    /// @brief Obtain the translation of the parent in world space
    TranslationComponent worldTranslation() const;

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Serializable Overrides
    /// @{

    /// @brief Outputs this data as a valid json string
    QJsonValue asJson() const override;

    /// @brief Populates this data using a valid json string
    virtual void loadFromJson(const QJsonValue& json) override;

    /// @}

protected:
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Friends 
    /// @{

    friend class AbstractTransformComponent;
    friend class AffineComponent;
    friend class TranslationComponent;
    friend class SceneObject;
    friend class SkeletonPose;

    /// @}


    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected methods
    /// @{  

    /// @brief Add and remove children from this transform
    void addChild(Transform* c);
    void removeChild(Transform* c);

    /// @brief Initialize this transform
    virtual void initialize();

    /// @brief Compute local matrix
    void computeLocalMatrix();

    /// @brief Decompose matrix into scale, rotation, translation
    void decompose(const Matrix4x4g& mat);

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected members
    /// @{    

    /// @brief Type of geometry inheritance for this transform
    Transform::InheritanceType m_inheritanceType;

    /// @brief Parent transform
    Transform* m_parent = nullptr;

    /// @brief Child transforms
    std::unordered_map<Uuid, Transform*> m_children;

    /// @brief Translation component
    TranslationComponent m_translation;

    /// @brief Rotation component
    RotationComponent m_rotation;

    /// @brief Scale component
    ScaleComponent m_scale;

    /// @brief Local model matrix
    Matrix4x4g m_localMatrix;

    /// @brief Global model matrix
    Matrix4x4g m_worldMatrix;

    /// @}

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespacing

#endif 