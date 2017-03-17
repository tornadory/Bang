#ifndef AUDIOCLIPASSETFILE_H
#define AUDIOCLIPASSETFILE_H

#include <QModelIndex>
#include <QFileSystemModel>

#include "File.h"
#include "String.h"

class SerializableObject;
class AudioClipAssetFile : public File
{
public:
    AudioClipAssetFile(const QFileSystemModel *model,
                       const QModelIndex &index);
    virtual const QPixmap& GetIcon() const override;

    #ifdef BANG_EDITOR
    virtual SerializableObject *GetInspectable() const override;
    #endif

    void SetAudioFilepath(const String &audioFilepath);
    const String& GetAudioFilepath() const;

    virtual bool IsAsset() const override;

private:
    String m_audioFilepath = "";
};

#endif // AUDIOCLIPASSETFILE_H
