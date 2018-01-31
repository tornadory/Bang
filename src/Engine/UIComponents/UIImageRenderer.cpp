﻿#include "Bang/UIImageRenderer.h"

#include "Bang/GL.h"
#include "Bang/Mesh.h"
#include "Bang/Image.h"
#include "Bang/Material.h"
#include "Bang/Resources.h"
#include "Bang/Texture2D.h"
#include "Bang/GameObject.h"
#include "Bang/MeshFactory.h"
#include "Bang/RectTransform.h"
#include "Bang/MaterialFactory.h"
#include "Bang/UILayoutManager.h"

USING_NAMESPACE_BANG

UIImageRenderer::UIImageRenderer()
{
    SetMaterial(MaterialFactory::GetUIImage().Get());
    p_quadMesh = Resources::Clone<Mesh>( MeshFactory::GetUIPlane() );
}

UIImageRenderer::~UIImageRenderer()
{
}

void UIImageRenderer::OnRender()
{
    UIRenderer::OnRender();

    if (GetTint().a > 0.0f)
    {
        GL::Render(p_quadMesh.Get()->GetVAO(), GetRenderPrimitive(),
                   p_quadMesh.Get()->GetVertexCount());
    }
}

void UIImageRenderer::SetImageTexture(const Path &imagePath)
{
    RH<Texture2D> tex = Resources::Load<Texture2D>(imagePath);
    SetImageTexture(tex.Get());
}

void UIImageRenderer::SetImageTexture(Texture2D* imageTexture)
{
    if (imageTexture != GetImageTexture())
    {
        p_imageTexture.Set(imageTexture);
        GetMaterial()->SetTexture(p_imageTexture.Get());
    }
}

void UIImageRenderer::SetTint(const Color &tint)
{
    if (tint != GetTint())
    {
        GetMaterial()->SetDiffuseColor(tint);
    }
}

const Color &UIImageRenderer::GetTint() const
{
    return GetMaterial()->GetDiffuseColor();
}

Texture2D *UIImageRenderer::GetImageTexture() const
{
    return p_imageTexture.Get();
}

void UIImageRenderer::OnTransformChanged()
{
    UIRenderer::OnTransformChanged();
}

Rect UIImageRenderer::GetBoundingRect(Camera *camera) const
{
    Rect boundingRect = UIRenderer::GetBoundingRect(camera);
    return Rect(boundingRect.GetCenter(), boundingRect.GetCenter());
}

void UIImageRenderer::CloneInto(ICloneable *clone) const
{
    UIRenderer::CloneInto(clone);
    UIImageRenderer *img = Cast<UIImageRenderer*>(clone);
    img->SetImageTexture( GetImageTexture() );
}

void UIImageRenderer::ImportXML(const XMLNode &xmlInfo)
{
    UIRenderer::ImportXML(xmlInfo);

    if (xmlInfo.Contains("Image"))
    {
        RH<Texture2D> tex = Resources::Load<Texture2D>(xmlInfo.Get<GUID>("Image"));
        SetImageTexture(tex.Get());
    }

    if (xmlInfo.Contains("Tint"))
    { SetTint( xmlInfo.Get<Color>("Tint") ); }
}

void UIImageRenderer::ExportXML(XMLNode *xmlInfo) const
{
    UIRenderer::ExportXML(xmlInfo);

    Texture2D *imgTex = GetImageTexture();
    xmlInfo->Set("Image", imgTex ? imgTex->GetGUID() : GUID::Empty());
    xmlInfo->Set("Tint", GetTint());
}
