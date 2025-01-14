#include "core/sound/GAudioCommands.h"

#include "core/GCoreEngine.h"
#include "core/events/GEventManager.h"
#include "core/components/GAudioSourceComponent.h"
#include "core/components/GAudioListenerComponent.h"

#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_thread.h>
#include "soloud_speech.h"

namespace rev {



// Audio Listener command

AudioListenerCommand::AudioListenerCommand()
{
}

AudioListenerCommand::AudioListenerCommand(AudioListenerComponent * component):
    m_audioListener(component)
{
}

AudioListenerCommand::~AudioListenerCommand()
{
}



// Move Listener command

UpdateListener3dCommand::UpdateListener3dCommand(AudioListenerComponent * component) :
    AudioListenerCommand(component)
{
}

void UpdateListener3dCommand::perform()
{
    m_audioListener->update3dAttributes();
}




// Audio Source command

AudioSourceCommand::AudioSourceCommand()
{
}

AudioSourceCommand::AudioSourceCommand(AudioSourceComponent * component):
    m_audioComponent(component)
{
}

AudioSourceCommand::~AudioSourceCommand()
{
}




// Play command

PlayAudioCommand::PlayAudioCommand(AudioSourceComponent * component) :
    AudioSourceCommand(component)
{
}

void PlayAudioCommand::perform()
{
    m_audioComponent->play();
}




// Move Audio Command

MoveAudioCommand::MoveAudioCommand(AudioSourceComponent * component, const Vector3 & pos):
    AudioSourceCommand(component),
    m_position(pos)
{
}

void MoveAudioCommand::perform()
{
    m_audioComponent->move(m_position);
}




// Update Voice Command

UpdateVoiceCommand::UpdateVoiceCommand(AudioSourceComponent * component, int voiceHandle) :
    AudioSourceCommand(component),
    m_voiceHandle(voiceHandle)
{
}



} // End namespaces