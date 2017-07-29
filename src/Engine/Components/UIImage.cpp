﻿#include "Bang/UIImage.h"

#include "Bang/Material.h"
#include "Bang/Texture2D.h"
#include "Bang/AssetsManager.h"

UIImage::UIImage()
{
    SetMaterial(AssetsManager::Load<Material>(
                    EPATH("Materials/UI/G_UIImage.bmat") ) );
    UseMaterialCopy();

    m_imageTexture = new Texture2D();
    m_imageTexture->CreateEmpty(1,1);
}

UIImage::~UIImage()
{
}

void UIImage::CloneInto(ICloneable *clone) const
{
    UIRenderer::CloneInto(clone);
    UIImage *img = SCAST<UIImage*>(clone);
    img->SetImage( GetImageTexture() );
}

void UIImage::SetImage(Texture2D *imageTexture)
{
    m_imageTexture = imageTexture;
    GetMaterial()->SetTexture(m_imageTexture);
}

Texture2D *UIImage::GetImageTexture() const
{
    return m_imageTexture;
}

void UIImage::Read(const XMLNode &xmlInfo)
{
    UIRenderer::Read(xmlInfo);
    Path texFilepath = xmlInfo.Get<Path>("Image");
    SetImage( AssetsManager::Load<Texture2D>(texFilepath) );
}

void UIImage::Write(XMLNode *xmlInfo) const
{
    UIRenderer::Write(xmlInfo);

    Texture2D *imgTex = GetImageTexture();
    Path texFilepath = imgTex ? imgTex->GetFilepath() : Path();
    xmlInfo->Set("Image", texFilepath);
}

