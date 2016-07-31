#include "MeshAssetFile.h"

#include "Mesh.h"
#include "AssetsManager.h"

MeshAssetFile::MeshAssetFile(const QFileSystemModel *model, const QModelIndex *index)
    : File(model, index)
{
    XMLNode *xmlInfo;
    xmlInfo = XMLParser::FromFile(m_path);
    m_meshFilepath = xmlInfo->GetFilepath("MeshFilepath");
    delete xmlInfo;
}

const std::string& MeshAssetFile::GetMeshFilepath() const
{
    return m_meshFilepath;
}
