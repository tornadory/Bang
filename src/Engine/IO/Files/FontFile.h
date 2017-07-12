#ifndef FONTASSETFILE_H
#define FONTASSETFILE_H

#include <QPixmap>
#include <QModelIndex>
#include <QFileSystemModel>
#include "Bang/WinUndef.h"

#include "Bang/Path.h"
#include "Bang/BFile.h"
#include "Bang/String.h"
#include "Bang/XMLNode.h"

class FontFile : public BFile
{
public:
    FontFile();
    FontFile(const Path &path);

    virtual const QPixmap& GetIcon() const override;

    virtual bool IsAsset() const override;

    const Path &GetFontFilepath() const;

private:
    Path m_trueFontFilepath;
};

#endif // FONTASSETFILE_H
