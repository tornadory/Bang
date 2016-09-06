#ifndef FILE_H
#define FILE_H

#include <fstream>

#include <QPainter>
#include <QModelIndex>
#include <QFileSystemModel>

#include "IInspectable.h"

class File
{
protected:

    const QFileSystemModel *m_fileSystemModel = nullptr;
    QModelIndex m_modelIndex;

    bool m_isFile = true;
    String m_path = "";
    String m_name = "";
    String m_absPath = "";
    String m_extension = "";

    // Adds on top of the passed QPixmap the little icon that
    // distinguishes between assets and noAsset files.
    static QPixmap AddNoAssetFileQPixmapOnTopOf(const QPixmap &pm);

public:
    File();
    File(const String &filepath);
    File(const QFileSystemModel *model, const QModelIndex &index);
    virtual ~File();

    bool IsTexture2DAsset() const;
    bool IsImageFile() const;

    bool IsMeshAsset() const;
    bool IsMeshFile() const;

    bool IsMaterialAsset() const;
    bool IsBehaviour() const;

    bool IsTextFile() const;

    bool IsPrefabAsset() const;

    bool IsDir() const;
    bool IsFile() const;

    /**
     * @brief The input must be something like "jpg png bmp obj"
     * @param extensions
     * @return
     */
    bool IsOfExtension(const String &extensions) const;

    static File *GetSpecificFile(const File &f);
    static bool Exists(const String &filepath);
    static void Write(const String &filepath, const String &contents);
    static void Write(const String &filepath, const Array<String> &lines);
    static void Write(const String &filepath, const List<String> &lines);
    static String GetContents(const String &filepath);

    virtual QPixmap GetIcon() const;
    virtual IInspectable *GetInspectable() const;

    String GetContents() const;

    const String& GetAbsolutePath() const;
    const String& GetRelativePath() const;
    const String& GetName() const;
    const String& GetExtension() const;
    String GetNameAndExtension() const;
};

#endif // FILE_H
