#ifndef GB_AUDIO_SOURCE_COMPONENT
#define GB_AUDIO_SOURCE_COMPONENT
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

template<typename T, size_t N> class Vector;
typedef Vector<float, 3> Vector3;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum class VoiceFlag {
    kProtected = 1 << 0, // Whether or not the voices produced by the component are protected (safe from deletion)
    kPaused = 1 << 1, // Whether or not the voices produced by the component are paused on play
    kBackground = 1 << 2, // Whether or not to play as background (for music)
    k3D = 1 << 3 // Whether or not to use 3D play settings
};

/// @struct AudioComponentSettings
/// @brief Settings for this specific audio component
struct AudioComponentSettings {

    /// @brief Convert from the given class type to JSON
    /// @note Actually defined in namespace outside of class, so this should be recognized by nlohmann JSON
    /// @param orJson The output JSON
    /// @param korObject The object to convert to JSON
    friend void to_json(nlohmann::json& orJson, const AudioComponentSettings& korObject);

    /// @brief Convert from JSON to the given class type
    /// @param korJson The input JSON
    /// @param orObject The object to be obtained from JSON
    friend void from_json(const nlohmann::json& korJson, AudioComponentSettings& orObject);


    bool testFlag(VoiceFlag flag) const {
        return m_voiceFlags & (size_t)flag;
    }

    void setFlag(VoiceFlag flag, bool value) {
        if (value) {
            // Toggle flag on
            m_voiceFlags |= (size_t)flag;
        }
        else {
            // Toggle flag off
            m_voiceFlags &= ~(size_t)flag;
        }
    }

    /// @brief Flag settings
    size_t m_voiceFlags = 0;

    /// @brief Scales on top of audio source volume
    float m_volume = 1.0f;

    /// @brief Pan, -1.0f to 1.0f
    float m_pan = 0.0f;

    /// @brief Relative play speed
    float m_relativePlaySpeed = 1.0f;

    /// @brief 
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @class AudioSourceComponent
class AudioSourceComponent: public Component {
public:
    //-----------------------------------------------------------------------------------------------------------------
    /// @name Static
    /// @{

    enum class SourceType {
        kWav,
        kWavStream,
        kSpeech
    };

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Constructors/Destructor
    /// @{
    AudioSourceComponent();
    AudioSourceComponent(const std::shared_ptr<SceneObject>& object);
    AudioSourceComponent(const AudioSourceComponent& other); // to store in sound manager vector
    ~AudioSourceComponent();

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Public Methods
    /// @{

    /// @brief Queue this sound to be played via sound manager
    void queuePlay();

    /// @brief Queue all voices associated with this sound to be moved
    void queueMove();

    /// @brief Enable the behavior of this script component
    virtual void enable() override;

    /// @brief Disable the behavior of this script component
    virtual void disable() override;

    /// @brief Max number of allowed components per scene object
    virtual int maxAllowed() const override { return -1; }

    virtual void preDestruction(CoreEngine* core) override;

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Properties
    /// @{

    AudioComponentSettings& voiceProperties() { return m_voiceProperties; }

    const std::shared_ptr<ResourceHandle>& audioHandle() { return m_audioHandle; }
    void setAudioHandle(const std::shared_ptr<ResourceHandle>& handle) {
        m_audioHandle = handle;
    }

    AudioResource* audioResource() {
        if (!m_audioHandle) {
            return nullptr;
        }
        else {
            return m_audioHandle->resourceAs<AudioResource>();
        }
    }

    template<typename T>
    T* audioSource() {
        if (!m_audioHandle) {
            return nullptr;
        }
        return static_cast<T*>(m_audioHandle->resourceAs<AudioResource>()->audioSource<T>());
    }

    template<typename T>
    const T* audioSource() const {
        if (!m_audioHandle) {
            return nullptr;
        }
        return static_cast<const T*>(m_audioHandle->resourceAs<AudioResource>()->audioSource<T>());
    }

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Friend Functions
    /// @{

    /// @brief Convert from the given class type to JSON
    /// @note Actually defined in namespace outside of class, so this should be recognized by nlohmann JSON
    /// @param orJson The output JSON
    /// @param korObject The object to convert to JSON
    friend void to_json(nlohmann::json& orJson, const AudioSourceComponent& korObject);

    /// @brief Convert from JSON to the given class type
    /// @param korJson The input JSON
    /// @param orObject The object to be obtained from JSON
    friend void from_json(const nlohmann::json& korJson, AudioSourceComponent& orObject);


    /// @}

protected:
    friend class AudioComponentWidget;
    friend class AudioSourceCommand;
    friend class PlayAudioCommand;
    friend class MoveAudioCommand;
    friend class SoundManager;

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected Methods
    /// @{

    /// @brief Add a voice handle
    void addVoice(int handle);

    /// @brief Play a sound from this component
    /// @details Should not be called directly, call queuePlay instead
    void play();

    /// @brief Move all voices associated with this audio component
    void move(const Vector3& pos);

    /// @brief Clear the audio source
    void clear(CoreEngine* core);

    /// @brief Get sound manager
    SoundManager* soundManager() const;

    /// @brief Update voices in this component
    void updateVoices();

    /// @}

    //-----------------------------------------------------------------------------------------------------------------
    /// @name Protected Members
    /// @{

    /// @brief Audio source
    /// @details Audio sources contain all of the information pertaining to each sound.
    /// Shorter sounds that are used frequently should use Wav, longer should use WavStream,
    /// since WavStream consumes less memory (at the cost of processing power)
    /// @note Audio resources may be shared between components
    std::shared_ptr<ResourceHandle> m_audioHandle;

    /// @brief Voices
    /// @details a voice is an instance of a sound.
    /// @note The default max number of concurrent voices is 16, virtual voices is 1024
    std::vector<int> m_activeVoiceHandles;

    /// @brief Voice groups
    /// @note The max number of voice groups is 4095

    /// @brief The filters applied to this particular audio source

    /// @brief The index of the bus to use for this audio source
    int m_busIndex;

    /// @brief The mutex used to manage voices
    std::shared_mutex m_voiceMutex;

    /// @brief Properties specific to this audio component
    AudioComponentSettings m_voiceProperties;

    /// @}


};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // end namespacing

#endif 
