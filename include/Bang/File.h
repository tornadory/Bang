#ifndef FILE_H
#define FILE_H

#include "Bang/Path.h"

NAMESPACE_BANG_BEGIN

class File
{
public:
    File();
    File(const Path &filepath);
    File(const String &filepath);
    virtual ~File();

    String GetContents() const;
    const Path& GetPath() const;

    static bool Remove(const Path &path);
    static bool CreateDirectory(const Path &dirPath);
    static bool Rename(const Path &oldPath, const Path &newPath);

    static bool Duplicate(const Path &fromPath, const Path &toPath);
    static bool DuplicateFile(const Path &fromFilepath,
                              const Path &toFilepath,
                              bool overwrite = true);
    static bool DuplicateDir(const Path &fromDirpath,
                             const Path &toDirpath,
                             bool overwrite = true);

    static String GetContents(const Path &filepath);
    static void Write(const Path &filepath, const String &contents);
    static void Write(const Path &filepath, const Array<String> &lines);
    static void Write(const Path &filepath, const List<String> &lines);

protected:
    Path m_path;
};

NAMESPACE_BANG_END

#endif // FILE_H
