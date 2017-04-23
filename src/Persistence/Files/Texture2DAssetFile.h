#ifndef FILETEXTURE2DASSET_H
#define FILETEXTURE2DASSET_H

#include <QPixmap>
#include <QModelIndex>
#include <QFileSystemModel>
#include "Bang/WinUndef.h"

#include "Bang/File.h"

class SerializableObject;
class Texture2DAssetFile : public File
{
public:
    Texture2DAssetFile();
    Texture2DAssetFile(const QFileSystemModel *model, const QModelIndex &index);

    String GetImageAbsFilepath() const;
    virtual const QPixmap& GetIcon() const override;

    #ifdef BANG_EDITOR
    virtual SerializableObject *GetNewInspectable() const override;
    #endif

    virtual bool IsAsset() const override;
};

#endif // FILETEXTURE2DASSET_H
