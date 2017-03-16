#ifndef AUDIOFILEINSPECTABLE_H
#define AUDIOFILEINSPECTABLE_H

#include "AudioFile.h"
#include "SerializableObject.h"

class XMLNode;
class AudioFileInspectable : public SerializableObject
{
public:
    AudioFileInspectable(const AudioFile &audioFile);

    virtual void Read(const XMLNode &xmlInfo) override;
    virtual void Write(XMLNode *xmlInfo) const override;

private:
    AudioFile m_audioFile;
};

#endif // AUDIOFILEINSPECTABLE_H
