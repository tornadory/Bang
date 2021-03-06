#include "Bang/ALAudioSource.h"

USING_NAMESPACE_BANG

ALAudioSource::ALAudioSource()
{
    alGenSources(1, &m_alSourceId);
    SetParams(m_audioParams); // Initialize AL source
}

ALAudioSource::~ALAudioSource()
{
    Stop();
    EventEmitter<IDestroyListener>::
            PropagateToListeners(&IDestroyListener::OnDestroyed, this);
    alDeleteSources(1, &m_alSourceId);
}

void ALAudioSource::Play()
{
    alSourcePlay(m_alSourceId);
}

void ALAudioSource::Pause()
{
    alSourcePause(m_alSourceId);
}

void ALAudioSource::Stop()
{
    alSourceStop(m_alSourceId);
}

void ALAudioSource::SetVolume(float volume)
{
    if (volume != GetVolume())
    {
        m_audioParams.volume = volume;
        alSourcef(GetALSourceId(), AL_GAIN, GetVolume());
    }
}
void ALAudioSource::SetPitch(float _pitch)
{
    float pitch = Math::Max(_pitch, 0.01f);
    if (pitch != GetPitch())
    {
        m_audioParams.pitch = pitch;
        alSourcef(GetALSourceId(), AL_PITCH, GetPitch());
    }
}
void ALAudioSource::SetRange(float range)
{
    if (range != GetRange())
    {
        m_audioParams.range = range;
        alSourcef(m_alSourceId,  AL_MAX_DISTANCE,       GetRange());
        alSourcef(m_alSourceId,  AL_REFERENCE_DISTANCE, GetRange() * 0.5f);
    }
}
void ALAudioSource::SetLooping(bool looping)
{
    if (looping != GetLooping())
    {
        m_audioParams.looping = looping;
        alSourcef(GetALSourceId(), AL_LOOPING, GetLooping());
    }
}

void ALAudioSource::SetPosition(const Vector3 &position)
{
    if (position != GetPosition())
    {
        m_audioParams.position = position;
        alSourcefv(GetALSourceId(), AL_POSITION, GetPosition().Data());

        //Vector3 at = -transform->GetForward(), up = transform->GetUp();
        //ALfloat listenerOri[] = { at.x, at.y, at.z, up.x, up.y, up.z };
        //alSourcefv(m_alSourceId, AL_ORIENTATION, listenerOri);
    }
}

void ALAudioSource::SetParams(const AudioParams &audioParams)
{
    SetVolume(audioParams.volume);
    SetPitch(audioParams.pitch);
    SetRange(audioParams.range);
    SetLooping(audioParams.looping);
    SetPosition(audioParams.position);
}

void ALAudioSource::SetALBufferId(ALuint bufferId)
{
    alSourcei(m_alSourceId, AL_BUFFER, bufferId);
}

bool ALAudioSource::IsPlaying() const { return GetState() == State::Playing; }
bool ALAudioSource::IsPaused() const { return GetState() == State::Paused; }
bool ALAudioSource::IsStopped() const { return GetState() == State::Stopped; }
float ALAudioSource::GetVolume() const { return m_audioParams.volume; }
float ALAudioSource::GetPitch() const { return m_audioParams.pitch; }
float ALAudioSource::GetRange() const { return m_audioParams.range; }
ALuint ALAudioSource::GetALSourceId() const { return m_alSourceId; }
const Vector3 &ALAudioSource::GetPosition() const { return m_audioParams.position; }
const AudioParams &ALAudioSource::GetParams() { return m_audioParams; }
bool ALAudioSource::GetLooping() const { return m_audioParams.looping; }
ALAudioSource::State ALAudioSource::GetState() const
{
    ALint state;
    alGetSourcei(m_alSourceId, AL_SOURCE_STATE, &state);
    return static_cast<State>(state);
}
