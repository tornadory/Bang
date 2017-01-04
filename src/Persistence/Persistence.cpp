#include "Persistence.h"

#ifdef BANG_EDITOR
#include "Explorer.h"
#endif

#include <QDir>
#include <QFile>
#include <QCryptographicHash>

#include "Debug.h"
#include "StringUtils.h"

String Persistence::s_currentSceneFilepath = "";

// Set by main.cpp
String Persistence::c_ProjectRootAbsolute = "";
String Persistence::c_ProjectAssetsRootAbsolute = "";
String Persistence::c_EngineRootAbsolute = "";
String Persistence::c_EngineAssetsRootAbsolute = "";

bool Persistence::IsDir(const String &path)
{
    ASSERT(!path.Empty(), "", return false);
    return QFileInfo(path.ToQString()).isDir();
}

bool Persistence::IsFile(const String &path)
{
    ASSERT(!path.Empty(), "", return false);
    return QFileInfo(path.ToQString()).isFile();
}

bool Persistence::IsAbsolute(const String &path)
{
    ASSERT(!path.Empty(), "", return false);
    return QFileInfo(path.ToQString()).isAbsolute();
}

String Persistence::GetProjectRootPathAbsolute()
{
    return Persistence::c_ProjectRootAbsolute;
}

String Persistence::GetAssetsPathAbsolute()
{
    return Persistence::c_ProjectAssetsRootAbsolute;
}

String Persistence::GetDir(const String &filepath)
{
    ASSERT(!filepath.Empty(), "", return "");
    String directory = "";
    const size_t lastSlash = filepath.rfind('/');
    if (lastSlash != String::npos)
    {
        directory = filepath.substr(0, lastSlash);
    }
    return directory;
}

String Persistence::GetFileName(const String &filepath)
{
    ASSERT(!filepath.Empty(), "", return "");
    String filename = "";
    const size_t lastSlash = filepath.rfind('/');
    if (lastSlash != String::npos)
    {
        filename = filepath.substr(lastSlash + 1);
        filename = StringUtils::Split(filename, '.')[0];
    }
    return filename;
}

String Persistence::GetFileNameWithExtension(const String &filepath)
{
    ASSERT(!filepath.Empty(), "", return "");
    String filename = "";
    const size_t lastSlash = filepath.rfind('/');
    if (lastSlash != String::npos)
    {
        filename = filepath.substr(lastSlash + 1);
    }
    return filename;
}

String Persistence::GetPathWithoutExtension(const String &filepath)
{
    ASSERT(!filepath.Empty(), "", return "");
    String path = GetDir(filepath);
    path += GetFileName(filepath);
    return path;
}


String Persistence::ToAbsolute(const String &relPath,
                               const String &prependDirectory)
{
    ASSERT(!relPath.Empty(), "", return "");

    String result = "";
    if (Persistence::IsAbsolute(relPath))
    {
        result = relPath;
    }
    else
    {
        String pDir = prependDirectory;
        if (!pDir.Empty())
        {
            if (pDir[pDir.Length()-1] == '/')
            {
                pDir = pDir.SubString(1, pDir.Length()-1);
            }
        }

        String rPath = relPath;
        if (rPath[0] == '.' && rPath[1] == '/') // Something like "./Images/wololo"
        {
            return  pDir + "/" +
                    relPath.substr(2, relPath.Length() - 1);
        }

        result = pDir + "/" + relPath;
    }

    // If we try to get canonical for non-existing filepath,
    // then it returns empty string, we must handle this
    if (Persistence::Exists(result))
    {
        result = QFileInfo(result.ToQString()).canonicalFilePath();
    }

    return result;
}

String Persistence::ToAbsolute(const String &relPath, bool isEngineFile)
{
    ASSERT(!relPath.Empty(), "", return "");
    return Persistence::ToAbsolute(relPath,
              isEngineFile ? Persistence::c_EngineAssetsRootAbsolute :
                             Persistence::c_ProjectAssetsRootAbsolute);
}

String Persistence::ToRelative(const String &absPath,
                               const String &prependDirectory)
{
    ASSERT(!absPath.Empty(), "", return "");

    if (!IsAbsolute(absPath))
    {
        if (absPath[0] != '.')
        {
            return "./" + absPath;
        }
        return absPath;
    }

    std::size_t pos = absPath.find(prependDirectory);
    if (pos == String::npos) return absPath;

    String relPath = "." + absPath.substr( pos + prependDirectory.Length(),
                                          (absPath.Length() - prependDirectory.Length()));
    return relPath;
}

String Persistence::ToRelative(const String &relPath, bool isEngineFile)
{
    ASSERT(!relPath.Empty(), "", return "");
    return Persistence::ToRelative(relPath,
              isEngineFile ? Persistence::c_EngineAssetsRootAbsolute :
                             Persistence::c_ProjectAssetsRootAbsolute);
}

String Persistence::ToRelative(const String &absPath)
{
    ASSERT(!absPath.Empty(), "", return "");
    return Persistence::ToRelative(absPath, Persistence::IsEngineFile(absPath));
}

bool Persistence::IsEngineFile(const String &filepath)
{
    ASSERT(!filepath.Empty(), "", return "");
    return filepath.BeginsWith(Persistence::c_EngineAssetsRootAbsolute);
}

String Persistence::GetDirUp(const String &filepath)
{
    ASSERT(!filepath.Empty(), "", return "");
    Array<String> splits = filepath.Split('/');
    splits.PopBack();
    return (Persistence::IsAbsolute(filepath) ? "/" : "") +
            String::Join(splits, "/");
}

String Persistence::GetRightmostDir(const String &dir)
{
    Array<String> parts = StringUtils::Split(dir, '/');
    ASSERT(!parts.Empty(), "", return "");
    return parts.Back();
}

bool Persistence::DuplicateFile(const String &fromFilepath,
                                const String &toFilepath)
{
    ASSERT(Persistence::ExistsFile(fromFilepath), "", return false);
    return QFile::copy(fromFilepath.ToQString(), toFilepath.ToQString());
}

bool Persistence::DuplicateDir(const String &fromDirpath,
                               const String &toDirpath)
{
    ASSERT(Persistence::ExistsDirectory(fromDirpath), "", return false);

    Persistence::CreateDirectory(toDirpath);
    List<String> filepaths = Persistence::GetFiles(fromDirpath, false);
    for(String filepath : filepaths)
    {
        String fileName = Persistence::GetFileNameWithExtension(filepath);
        Persistence::DuplicateFile(fromDirpath + "/" + fileName,
                                   toDirpath   + "/" + fileName);
    }

    List<String> subdirs = Persistence::GetSubDirectories(fromDirpath, false);
    for (String subdir : subdirs)
    {
        Persistence::DuplicateDir(
                    subdir,
                    toDirpath + "/" + Persistence::GetRightmostDir(subdir));
    }
}

#ifdef BANG_EDITOR
String Persistence::GetNextDuplicateName(const String &path)
{
    ASSERT(!path.Empty(), "", return "");

    String filePath = path;
    String fileDir  = Persistence::GetDir(filePath);
    String fileName = Persistence::GetFileNameWithExtension(filePath);

    Array<String> splitted = StringUtils::Split(fileName, '.');
    fileName = splitted[0];
    String fileExtension = splitted.Size() <= 1 ? "" : splitted[1];
    splitted = StringUtils::Split(fileName, '_');
    int number = 1;
    if (splitted.Size() > 1)
    {
        String numberString = splitted[splitted.Size() - 1];
        bool ok = false;
        int readNumber = StringUtils::ToInt(numberString, &ok);
        if (ok)
        {
            number = readNumber + 1;
            splitted.PopBack();

            int lastUnderscorePos = fileName.rfind('_');
            if (lastUnderscorePos != -1) // Strip _[number] from fileName
            {
                fileName = fileName.substr(0, lastUnderscorePos);
            }
        }
    }

    String result = "";
    if (fileDir != "")
    {
        result += fileDir + "/";
    }
    result += fileName + "_" + std::to_string(number);
    if (fileExtension != "")
    {
        result += "." + fileExtension;
    }
    return result;
}

String Persistence::GetDuplicateName(const String &path)
{
    ASSERT(!path.Empty(), "", return "");
    String result = path;
    while (Persistence::Exists(result))
    {
        result = Persistence::GetNextDuplicateName(result);
    }
    return result;
}
#endif

List<String> Persistence::GetSubDirectories(const String &dirPath,
                                            bool recursive)
{
    ASSERT(!dirPath.Empty(), "", return {});
    List<String> subdirsList;
    if (!Persistence::ExistsDirectory(dirPath)) { return subdirsList; }

    QStringList subdirs =  QDir(dirPath.ToQString()).entryList();
    for (QString qSubdir : subdirs)
    {
        if (qSubdir == "." || qSubdir == "..") continue;

        String subdirName = String(qSubdir);
        String subdirPath = dirPath + "/" + subdirName;
        if (Persistence::IsDir(subdirPath))
        {
            subdirsList.Add(subdirPath);
            if (recursive)
            {
                List<String> subdirsListRecursive =
                        GetSubDirectories(subdirPath, recursive);
                subdirsList.Splice(subdirsList.End(), subdirsListRecursive);
            }
        }
    }
    return subdirsList;
}

List<String> Persistence::GetFiles(const String &dirPath,
                                   bool recursive,
                                   const List<String> &extensions)
{
    ASSERT(!dirPath.Empty(), "", return {});
    List<String> filesList;
    QStringList extensionList;
    for (String ext : extensions)
    {
        if (!ext.BeginsWith("*.")) { ext = "*." + ext; }
        extensionList.append(ext.ToQString());
    }

    List<String> subdirs = GetSubDirectories(dirPath, recursive);
    subdirs.PushFront(dirPath);
    for (String subdir : subdirs)
    {
        QStringList filepathList = QDir(subdir.ToQString())
                .entryList(extensionList);

        for (QString qFilepath : filepathList)
        {
            String filepath(qFilepath);
            filepath = subdir + "/" + filepath;
            if (filepath == "." || filepath == ".." ||
                !Persistence::IsFile(filepath)) { continue; }

            filesList.Add(filepath);
        }
    }
    return filesList;
}

bool Persistence::Remove(const String &path)
{
    ASSERT(Persistence::ExistsFile(path), "", return false);
    if (Persistence::IsFile(path))
    {
        QFile f(path.ToQString());
        return f.remove();
    }
    else
    {
        List<String> subDirs  = GetSubDirectories(path, false);
        for (String subDir : subDirs) { Persistence::Remove(subDir); }
        List<String> subFiles = GetFiles(path, false);
        for (String subFile : subFiles) { Persistence::Remove(subFile); }
        QDir().rmdir(path.ToQString());
    }
    return false;
}

bool Persistence::ExistsFile(const String &filepath)
{
    ASSERT(!filepath.Empty(), "", return false);
    return QFile(filepath.ToQString()).exists();
}

bool Persistence::ExistsDirectory(const String &dirPath)
{
    ASSERT(!dirPath.Empty(), "", return false);
    return QDir(dirPath.ToQString()).exists();
}

bool Persistence::Exists(const String &filepath)
{
    ASSERT(!filepath.Empty(), "", return false);
    if (Persistence::IsDir(filepath))
    {
        return Persistence::ExistsDirectory(filepath);
    }
    return Persistence::ExistsFile(filepath);
}

bool Persistence::CreateDirectory(const String &dirPath)
{
    ASSERT(!dirPath.Empty(), "", return false);
    return QDir().mkdir(dirPath.ToQString());
}

void Persistence::SetActiveSceneFilepath(const String &scenePath)
{
    s_currentSceneFilepath = scenePath;
}

String Persistence::SetActiveSceneFilepath()
{
    return s_currentSceneFilepath;
}

bool Persistence::Rename(const String &oldPath, const String &newPath)
{
    ASSERT(Persistence::Exists(oldPath), "", return false);
    if (Persistence::IsDir(oldPath))
    {
        return QDir().rename(oldPath.ToQString(), newPath.ToQString());
    }
    else
    {
        return QFile().rename(oldPath.ToQString(), newPath.ToQString());
    }
}

bool Persistence::Move(const String &oldPath, const String &newPath)
{
    return Persistence::Rename(oldPath, newPath);
}

String Persistence::GetHash(const String &filepath)
{
    ASSERT(Persistence::ExistsFile(filepath), "", return "");

    QFile file(filepath.ToQString());
    if(file.open(QIODevice::ReadOnly))
    {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        QByteArray result = hash.hash(file.readAll(), QCryptographicHash::Sha1);
        file.close();
        return String( QString(result.toHex()) );
    }
    return "";
}

String Persistence::AppendExtension(const String &filepath, const String extNoDot)
{
    ASSERT(!filepath.Empty(), "", return "");
    if (filepath.find("." + extNoDot) != String::npos) return filepath;
    return filepath + "." + extNoDot;
}



