#include "Bang/AudioPlayerRunnable.h"

#include "Bang/AudioClip.h"
#include "Bang/AudioManager.h"
#include "Bang/ALAudioSource.h"

USING_NAMESPACE_BANG

AudioPlayerRunnable::AudioPlayerRunnable(AudioClip *clip,
                                         ALAudioSource *alAudioSource,
                                         float delayInSeconds)
{
    m_audioClip = clip;
    m_alAudioSource = alAudioSource;
    m_delayInSeconds = delayInSeconds;
    SetAutoDelete(true);
}

AudioPlayerRunnable::~AudioPlayerRunnable()
{
    AudioManager::GetInstance()->OnAudioFinishedPlaying(this);
    if (m_alAudioSource->m_autoDelete) { delete m_alAudioSource; }
}

void AudioPlayerRunnable::Resume() { m_alAudioSource->Play(); }
void AudioPlayerRunnable::Pause() { m_alAudioSource->Pause(); }
void AudioPlayerRunnable::Stop()
{
    m_forceExit = true;
    m_alAudioSource->Stop();
}
AudioClip *AudioPlayerRunnable::GetAudioClip() const { return m_audioClip; }
ALAudioSource *AudioPlayerRunnable::GetALAudioSource() const
{
    return m_alAudioSource;
}

void AudioPlayerRunnable::Run()
{
    ENSURE(m_audioClip->IsLoaded());

    if (m_delayInSeconds > 0.0f) // Wait delay
    {
        Thread::SleepCurrentThread(m_delayInSeconds);
    }

    AudioManager::ClearALErrors();
    m_alAudioSource->Play(); // Play and wait until source is stopped
    do
    {
        AudioManager::ClearALErrors();
        Thread::SleepCurrentThread(0.3f);

        ALenum error = alGetError();
        bool hasError = (error != AL_NO_ERROR);
        if (hasError) { m_forceExit = true; }
    }
    while ( !m_forceExit && !m_alAudioSource->IsStopped() );
}
