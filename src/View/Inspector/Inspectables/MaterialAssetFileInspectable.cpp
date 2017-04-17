#include "Bang/MaterialAssetFileInspectable.h"

#include "Bang/Scene.h"
#include "Bang/Material.h"
#include "Bang/Renderer.h"
#include "Bang/IconManager.h"
#include "Bang/SceneManager.h"
#include "Bang/AssetsManager.h"

MaterialAssetFileInspectable::MaterialAssetFileInspectable()
{
}

MaterialAssetFileInspectable::MaterialAssetFileInspectable
    (const MaterialAssetFile &materialAssetFile) :
        m_materialAssetFile(materialAssetFile)
{
    XMLNode *xmlMatInfo = XMLParser::FromFile(
                m_materialAssetFile.GetRelativePath());
    if (xmlMatInfo)
    {
        m_xmlInfo = *xmlMatInfo;
        delete xmlMatInfo;
    }
}

const QPixmap &MaterialAssetFileInspectable::GetIcon() const
{
    return Material::GetIconStatic();
}

void MaterialAssetFileInspectable::Read(const XMLNode &xmlInfo)
{
    // Update live instances currently being used
    AssetsManager::UpdateAsset(m_materialAssetFile.GetAbsolutePath(), xmlInfo);
    m_xmlInfo = xmlInfo;
}

void MaterialAssetFileInspectable::Write(XMLNode *xmlInfo) const
{
    xmlInfo->SetTagName("MaterialAssetFileInspectable");

    // Do Read & Write so that old files with different formats have the
    // newer one
    Material *mat = new Material();
    mat->Read(m_xmlInfo);
    mat->Write(xmlInfo);

    m_xmlInfo = *xmlInfo;

    delete mat;

    IconManager::InvalidatePixmap( m_materialAssetFile.GetAbsolutePath() );
}
