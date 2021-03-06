#include "Bang/UIRenderer.h"

#include "Bang/Rect.h"
#include "Bang/GameObject.h"
#include "Bang/RectTransform.h"

USING_NAMESPACE_BANG

UIRenderer::UIRenderer()
{
    SetViewProjMode(GL::ViewProjMode::IgnoreBoth);

    SetCullByRectTransform(true);
}

UIRenderer::~UIRenderer()
{
}

void UIRenderer::OnRender()
{
    Renderer::OnRender();
}

void UIRenderer::OnRender(RenderPass renderPass)
{
    bool render = true;
    if (GetCullByRectTransform())
    {
        RectTransform *rt = GetGameObject()->GetRectTransform();
        if (rt)
        {
            Rect rectNDC = rt->GetViewportRectNDC();
            if (rectNDC != Rect::Zero &&
                Rect::Intersection(Rect::NDCRect, rectNDC) == Rect::Zero)
            {
                render = false;
            }
        }
    }

    if (render)
    {
        Renderer::OnRender(renderPass);
    }
}

void UIRenderer::SetCullByRectTransform(bool cullByRectTransform)
{
    m_cullByRectTransform = cullByRectTransform;
}

bool UIRenderer::GetCullByRectTransform() const
{
    return m_cullByRectTransform;
}

void UIRenderer::OnEnabled()
{
    Renderer::OnEnabled();
    PropagateRendererChanged();
}

void UIRenderer::OnDisabled()
{
    Renderer::OnDisabled();
    PropagateRendererChanged();
}

void UIRenderer::OnTransformChanged()
{
    PropagateRendererChanged();
}

void UIRenderer::OnChildAdded(GameObject*, GameObject*)
{
    PropagateRendererChanged();
}

void UIRenderer::OnChildRemoved(GameObject*, GameObject*)
{
    PropagateRendererChanged();
}

void UIRenderer::OnParentChanged(GameObject*, GameObject*)
{
    PropagateRendererChanged();
}

Rect UIRenderer::GetBoundingRect(Camera *camera) const
{
    GameObject *go = GetGameObject();
    RectTransform *rt = go ? go->GetRectTransform() : nullptr;
    return rt ? rt->GetViewportRectNDC() : Rect::Zero;
}
