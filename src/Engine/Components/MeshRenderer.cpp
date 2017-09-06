#include "Bang/MeshRenderer.h"

#include "Bang/Mesh.h"
#include "Bang/VAO.h"
#include "Bang/MeshIO.h"
#include "Bang/Material.h"
#include "Bang/Transform.h"
#include "Bang/Resources.h"
#include "Bang/GameObject.h"

USING_NAMESPACE_BANG

MeshRenderer::MeshRenderer()
{
    SetRenderPrimitive( GL::Primitives::Triangles );
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::SetMesh(Mesh *m) { p_mesh = m; }
Mesh *MeshRenderer::GetMesh() const { return p_mesh; }
AABox MeshRenderer::GetAABBox() const
{
    return p_mesh ? p_mesh->GetAABBox() : AABox::Empty;
}

void MeshRenderer::OnRender()
{
    Renderer::OnRender(); ENSURE(p_mesh);
    GL::Render(p_mesh->GetVAO(), GetRenderPrimitive(), p_mesh->GetVertexCount());
}

void MeshRenderer::CloneInto(ICloneable *clone) const
{
    Renderer::CloneInto(clone);
    MeshRenderer *mr = SCAST<MeshRenderer*>(clone);
    mr->SetMesh( GetMesh() );
}

void MeshRenderer::ImportXML(const XMLNode &xmlInfo)
{
    Renderer::ImportXML(xmlInfo);
    if (xmlInfo.Contains("Mesh"))
    { SetMesh( Resources::Load<Mesh>( xmlInfo.Get<GUID>("Mesh") ) ); }
}

void MeshRenderer::ExportXML(XMLNode *xmlInfo) const
{
    Renderer::ExportXML(xmlInfo);
    xmlInfo->Set("Mesh", GetMesh() ? GetMesh()->GetGUID() : GUID::Empty());
}
