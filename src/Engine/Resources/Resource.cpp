#include "Bang/Resource.h"

#include "Bang/Resources.h"
#include "Bang/ImportFilesManager.h"

USING_NAMESPACE_BANG

// IResource
IResource::IResource()
{
    ASSERT(Resources::AssertCreatedFromResources());
}

IResource::~IResource()
{
    // ASSERT(Resources::AssertDestroyedFromResources() ||
    //        ObjectManager::AssertDestroyedFromObjectManager);
}

// Resource
Path Resource::GetResourceFilepath() const
{
    return ImportFilesManager::GetFilepath( GetGUID() );
}

GUID::GUIDType Resource::GetNextInsideFileGUID() const
{
    return 0;
}

Resource *Resource::GetInsideFileResource(const GUID &insideFileGUID) const
{
    return GetInsideFileResource(insideFileGUID.GetInsideFileGUID());
}

Resource* Resource::GetInsideFileResource(GUID::GUIDType) const
{
    return nullptr;
}

void Resource::ImportXML(const XMLNode &xmlInfo)
{
    Serializable::ImportXML(xmlInfo);
}

void Resource::ExportXML(XMLNode *xmlInfo) const
{
    Serializable::ExportXML(xmlInfo);
}
