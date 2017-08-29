#include "Bang/SerializableObject.h"

#include <istream>
#include <ostream>
#include <sstream>
#include <fstream>

#include "Bang/Path.h"
#include "Bang/File.h"
#include "Bang/String.h"
#include "Bang/XMLNode.h"
#include "Bang/GUIDManager.h"

SerializableObject::SerializableObject()
{
    SetGUID( GUIDManager::GetNewGUID() );
}

SerializableObject::~SerializableObject()
{
}

SerializableObject::SerializableObject(const SerializableObject &rhs)
{
    // Don't copy GUID, intentionally left in blank
}

XMLNode SerializableObject::GetXMLInfo() const
{
    XMLNode xmlInfo;
    ExportXML(&xmlInfo);
    return xmlInfo;
}

String SerializableObject::GetSerializedString() const
{
    XMLNode xmlInfo;
    ExportXML(&xmlInfo);
    return xmlInfo.ToString();
}

void SerializableObject::ImportXML(const String &xmlInfoString)
{
    XMLNode xmlInfo = XMLNode::FromString(xmlInfoString);
    ImportXML(xmlInfo);
    PostImportXML(xmlInfo);
}

void SerializableObject::ImportXMLReflection(const XMLNode &xmlInfo)
{
}

void SerializableObject::ExportXMLReflection(XMLNode *xmlInfo) const
{
}

void SerializableObject::ImportXML(const XMLNode &xmlInfo)
{
    ImportXMLReflection(xmlInfo);
    if (xmlInfo.Contains("GUID"))
    { SetGUID(xmlInfo.Get<GUID>("GUID")); }
}

void SerializableObject::ExportXML(XMLNode *xmlInfo) const
{
    ExportXMLReflection(xmlInfo);
    xmlInfo->SetTagName( GetClassName() );
    xmlInfo->Set<GUID>( "GUID", GetGUID() );
}

bool SerializableObject::ImportXMLFromFile(const Path &path)
{
    if (path.Exists())
    {
        String fileContents = File::GetContents(path);
        ImportXML(fileContents);
        return true;
    }
    return false;
}

bool SerializableObject::ExportXMLToFile(const Path &path) const
{
    File::Write(path, GetSerializedString());
    return true;
}

void SerializableObject::PostImportXML(const XMLNode &xmlInfo) {}

void SerializableObject::CloneInto(ICloneable *) const
{
}

String SerializableObject::GetInstanceId() const
{
    return String::ToString( static_cast<const void*>(this) );
}

HideFlags &SerializableObject::GetHideFlags() { return m_hideFlags; }
const HideFlags &SerializableObject::GetHideFlags() const { return m_hideFlags; }

void SerializableObject::SetGUID(const GUID &guid)
{
    m_GUID = guid;
    GUIDManager::RemoveGUID( GetGUID() );
}

const GUID &SerializableObject::GetGUID() const
{
    return m_GUID;
}
