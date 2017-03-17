#ifndef FONTASSETFILE_H
#define FONTASSETFILE_H

#include <QPixmap>
#include <QModelIndex>
#include <QFileSystemModel>

#include "File.h"
#include "String.h"

class SerializableObject;
class FontAssetFile : public File
{
public:
    FontAssetFile(const QFileSystemModel *model, const QModelIndex &index);
    virtual const QPixmap& GetIcon() const override;

    #ifdef BANG_EDITOR
    virtual SerializableObject *GetInspectable() const override;
    #endif

    virtual bool IsAsset() const override;

    const String &GetFontFilepath() const;

private:
    String m_trueFontFilepath = "";
};

#endif // FONTASSETFILE_H
