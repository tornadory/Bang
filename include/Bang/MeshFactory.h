#ifndef MESHFACTORY_H
#define MESHFACTORY_H

#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

class MeshFactory
{
public:
    static RH<Mesh> GetPlane();
    static RH<Mesh> GetUIPlane();
    static RH<Mesh> GetCube();
    static RH<Mesh> GetSphere();
    static RH<Mesh> GetCone();

    static GameObject* GetPlaneGameObject();
    static GameObject* GetCubeGameObject();
    static GameObject* GetSphereGameObject();
    static GameObject* GetConeGameObject();

private:
    MeshFactory() = default;
    static RH<Mesh> GetMesh(const String &enginePath);

    static GameObject* CreatePrimitiveGameObject(Mesh* m, const String &name);
};

NAMESPACE_BANG_END

#endif // MESHFACTORY_H
