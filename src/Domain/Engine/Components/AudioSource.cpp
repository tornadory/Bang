#include "AudioSource.h"

#include "Gizmos.h"
#include "XMLNode.h"
#include "AudioClip.h"
#include "Transform.h"
#include "AudioClip.h"
#include "Texture2D.h"
#include "GameObject.h"
#include "ICloneable.h"
#include "AudioManager.h"
#include "AssetsManager.h"

AudioSource::AudioSource()
{
    alGenSources(1, &m_alSourceId);
}

AudioSource::~AudioSource()
{
    Stop();

    if (m_audioClip)
    {
        m_audioClip->OnAudioSourceDettached(this);
    }

    alDeleteSources(1, &m_alSourceId);
}

String AudioSource::GetName() const
{
    return "AudioSource";
}

void AudioSource::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    AudioSource *as = Object::SCast<AudioSource>(clone);
    as->SetAudioClip( GetAudioClip() );
    as->SetVolume( GetVolume()  );
    as->SetPitch( GetPitch() );
    as->SetRange( GetRange() );
    as->SetLooping( IsLooping() );
}

#ifdef BANG_EDITOR
void AudioSource::OnButtonClicked(const String &attrName)
{
    if (IsPlaying())
    {
        Stop();
    }
    else
    {
        Play();
    }
}
#endif

void AudioSource::Read(const XMLNode &xmlInfo)
{
    Component::Read(xmlInfo);

    String audioClipFilepath = m_audioClip ? m_audioClip->GetFilepath() : "";
    String newAudioClipFilepath = xmlInfo.GetFilepath("AudioClip");
    if (audioClipFilepath != newAudioClipFilepath)
    {
        SetAudioClip( AssetsManager::Load<AudioClip>(newAudioClipFilepath) );
    }
    SetVolume(xmlInfo.GetFloat("Volume"));
    SetPitch(xmlInfo.GetFloat("Pitch"));
    SetRange(xmlInfo.GetFloat("Range"));
    SetLooping(xmlInfo.GetBool("Looping"));
}

void AudioSource::Write(XMLNode *xmlInfo) const
{
    Component::Write(xmlInfo);
    xmlInfo->SetTagName( GetName() );

    String audioClipFilepath = m_audioClip ? m_audioClip->GetFilepath() : "";
    xmlInfo->SetFilepath("AudioClip", audioClipFilepath,
                         AudioClip::GetFileExtensionStatic());
    xmlInfo->SetFloat("Volume",   m_volume);
    xmlInfo->SetFloat("Pitch",    m_pitch);
    xmlInfo->SetFloat("Range",    m_range);
    xmlInfo->SetBool("Looping",   m_looping);

    AudioSource *noConstThis = const_cast<AudioSource*>(this);
    if (IsPlaying())
    {
        xmlInfo->SetButton("Stop", noConstThis, {});
        xmlInfo->SetButton("Play", noConstThis, {XMLProperty::Hidden});
    }
    else
    {

        xmlInfo->SetButton("Stop", noConstThis, {XMLProperty::Hidden});
        xmlInfo->SetButton("Play", noConstThis, {});
    }
}

void AudioSource::Play(float delaySeconds)
{
    ASSERT(m_audioClip);

    AudioManager::PlayAudioClip(m_audioClip, GetALSourceId(),
                                delaySeconds);
}

void AudioSource::Pause()
{
    ASSERT(m_audioClip);
    alSourcePause(m_alSourceId);
}

void AudioSource::Stop()
{
    ASSERT(m_audioClip);
    alSourceStop(m_alSourceId);
}

AudioClip *AudioSource::GetAudioClip() const
{
    return m_audioClip;
}

void AudioSource::SetAudioClip(AudioClip *audioClip)
{
    if (m_audioClip)
    {
        m_audioClip->OnAudioSourceDettached(this);
    }

    SetAudioClipNoDettachAttach(audioClip);

    if (m_audioClip)
    {
        m_audioClip->OnAudioSourceAttached(this);
    }
}

void AudioSource::SetVolume(float volume)
{
    m_volume = volume;
    alSourcef(GetALSourceId(), AL_GAIN, m_volume);
}

void AudioSource::SetPitch(float pitch)
{
    m_pitch = pitch;
    alSourcef(GetALSourceId(), AL_PITCH, m_pitch);
}

void AudioSource::SetRange(float range)
{
    m_range = range;
    alSourcef(GetALSourceId(), AL_MAX_DISTANCE, m_range);
}

void AudioSource::SetLooping(bool looping)
{
    m_looping = looping;
    alSourcef(GetALSourceId(), AL_LOOPING, m_looping);
}

bool AudioSource::IsPlaying() const
{
    return GetState() == State::Playing;
}

bool AudioSource::IsPaused() const
{
    return GetState() == State::Paused;
}

bool AudioSource::IsStopped() const
{
    return GetState() == State::Stopped;
}

AudioSource::State AudioSource::GetState() const
{
    ALint state;
    alGetSourcei(m_alSourceId, AL_SOURCE_STATE, &state);
    return static_cast<State>(state);
}

float AudioSource::GetPlayProgress() const
{
    ASSERT(m_audioClip, "", 0.0f);

    float secondsOffset;
    alGetSourcef(m_alSourceId, AL_SEC_OFFSET, &secondsOffset);
    return secondsOffset / m_audioClip->GetLength();
}

float AudioSource::GetVolume() const
{
    return m_volume;
}

float AudioSource::GetPitch() const
{
    return m_pitch;
}

float AudioSource::GetRange() const
{
    return m_range;
}

bool AudioSource::IsLooping() const
{
    return m_looping;
}

void AudioSource::OnUpdate()
{
    Component::OnUpdate();

    UpdateALProperties();
}

void AudioSource::OnDrawGizmos(bool depthed, bool overlay)
{
    Component::OnDrawGizmos(depthed, overlay);

    #ifdef BANG_EDITOR
    if (!depthed && !overlay)
    {
        Texture2D *tex = AssetsManager::Load<Texture2D>(
                    "Textures/AudioSourceIcon.btex2d", true);
        Gizmos::SetPosition(transform->GetPosition());
        Gizmos::SetScale(Vector3::One * 0.1f);
        Gizmos::SetColor(Color::White);
        Gizmos::RenderIcon(tex);
    }

    if (depthed && gameObject->IsSelected())
    {
        Gizmos::SetColor(Color::Orange);
        Gizmos::SetPosition(transform->GetPosition());
        Gizmos::RenderSimpleSphere(transform->GetPosition(), m_range);
    }
    #endif
}

ALuint AudioSource::GetALSourceId() const
{
    return m_alSourceId;
}

void AudioSource::UpdateALProperties() const
{
    alSourcef(m_alSourceId,  AL_GAIN,         m_volume);
    alSourcef(m_alSourceId,  AL_PITCH,        m_pitch);
    alSourcei(m_alSourceId,  AL_LOOPING,      m_looping);
    Vector3 position = gameObject? transform->GetPosition() : Vector3::Zero;
    alSourcefv(m_alSourceId, AL_POSITION, position.Values());
    alSourcefv(m_alSourceId, AL_VELOCITY, Vector3::Zero.Values());

    alSourcef(m_alSourceId,  AL_MAX_DISTANCE, m_range);
    alSourcef(m_alSourceId,  AL_REFERENCE_DISTANCE, m_range * 0.5f);

    //Vector3 at = -transform->GetForward(), up = transform->GetUp();
    //ALfloat listenerOri[] = { at.x, at.y, at.z, up.x, up.y, up.z };
    //alSourcefv(m_alSourceId, AL_ORIENTATION, listenerOri);
}

void AudioSource::SetAudioClipNoDettachAttach(AudioClip *audioClip)
{
    m_audioClip = audioClip;
    alSourcei(m_alSourceId, AL_BUFFER,
              audioClip ? m_audioClip->GetALBufferId() : 0);
}
