#ifndef GB_AUDIO_LISTENER_COMPONENT
#define GB_AUDIO_LISTENER_COMPONENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard Includes
#include <shared_mutex>

// Qt
#include <QString>
#include <QJsonValue>

// Project
#include "GComponent.h"
#include "core/sound/GAudioResource.h"
#include "fortress/containers/math/GVector.h"

namespace SoLoud {
class AudioSource;
class Wav;
class WavStream;
class Bus;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace rev {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SoundManager;
class ResourceHandle;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class AudioListenerComponent
class AudioListenerComponent: public Component {
public:
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Static
    /// @{
    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Constructors/Destructor
    /// @{
    AudioListenerComponent();
    AudioListenerComponent(const std::shared_ptr<SceneObject>& object);
    ~AudioListenerComponent();

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Public Methods
    /// @{

    /// @brief Set velocity in settings/SoLoud
    void setVelocity(const Vector3& velocity);
    
    /// @brief Set speed of sound in settings/SoLoud
    /// @note default is 343 (assuming m/s)
    void setSpeedOfSound(float sos);

    /// @brief Queue a move in the sound manager (updates all 3D sound settings)
    void queueUpdate3d();

    /// @brief Enable the behavior of this script component
    virtual void enable() override;

    /// @brief Disable the behavior of this script component
    virtual void disable() override;

    /// @brief Max number of allowed components per scene object
    virtual int maxAllowed() const override { return 1; }

    virtual void preDestruction(CoreEngine* core) override;

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Properties
    /// @{

    /// @brief the velocity of the object, in world units
    const Vector3& velocity() const { return m_velocity; }

    /// @brief The speed of sound, in world units
    float speedOfSound() const { return m_speedOfSound; }

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Friend Functions
    /// @{

    /// @brief Convert from the given class type to JSON
    /// @note Actually defined in namespace outside of class, so this should be recognized by nlohmann JSON
    /// @param orJson The output JSON
    /// @param korObject The object to convert to JSON
    friend void to_json(nlohmann::json& orJson, const AudioListenerComponent& korObject);

    /// @brief Convert from JSON to the given class type
    /// @param korJson The input JSON
    /// @param orObject The object to be obtained from JSON
    friend void from_json(const nlohmann::json& korJson, AudioListenerComponent& orObject);


    /// @}

protected:
    friend class SoundManager;
    friend class UpdateListener3dCommand;

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected Methods
    /// @{

    /// @brief Get sound manager
    SoundManager* soundManager() const;

    /// @brief Set listener position in SoLoud
    void update3dAttributes();

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected Members
    /// @{

    // TODO: Implement set3dListenerParameters
    // Settings: location, speed, speed of sound

    /// @brief the velocity of the object, in world units
    Vector3 m_velocity;

    /// @brief The speed of sound, in world units
    float m_speedOfSound;

    /// @}


};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespacing

#endif 
