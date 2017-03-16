#ifndef MESHASSETFILEINSPECTABLE_H
#define MESHASSETFILEINSPECTABLE_H

#include "SerializableObject.h"
#include "MeshAssetFile.h"

class XMLNode;
class MeshAssetFileInspectable : public SerializableObject
{
public:
    MeshAssetFileInspectable(const MeshAssetFile &meshFile);

    virtual void Read(const XMLNode &xmlInfo) override;
    virtual void Write(XMLNode *xmlInfo) const override;

private:
    MeshAssetFile m_meshAssetFile;
};

#endif // MESHASSETFILEINSPECTABLE_H
