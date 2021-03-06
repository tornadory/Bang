#include "Bang/RectTransform.h"

#include "Bang/GL.h"
#include "Bang/Rect.h"
#include "Bang/Input.h"
#include "Bang/Gizmos.h"
#include "Bang/XMLNode.h"
#include "Bang/Matrix4.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/UILayoutManager.h"
#include "Bang/ITransformListener.h"

USING_NAMESPACE_BANG

RectTransform::RectTransform()
{
}

RectTransform::~RectTransform()
{
}

Vector2 RectTransform::
FromViewportPointToLocalPointNDC(const Vector2 &vpPoint) const
{
    Rect parentVPRect = GetParentViewportRect();
    Vector2 parentSizePx = Vector2::Max(Vector2::One, parentVPRect.GetSize());
    Vector2f pixelNDCSize = (1.0f / Vector2f(parentSizePx)) * 2.0f;
    return Vector2f(vpPoint - parentVPRect.GetMin()) * pixelNDCSize - 1.0f;
}
Vector2 RectTransform::
FromViewportPointToLocalPointNDC(const Vector2i &vpPoint) const
{
    return FromViewportPointToLocalPointNDC( Vector2(vpPoint) );
}

Vector2 RectTransform::FromViewportAmountToLocalAmountNDC(const Vector2 &vpAmount) const
{
    Rect parentVPRect = GetParentViewportRect();
    Vector2 parentSizePx = Vector2::Max(Vector2::One, parentVPRect.GetSize());
    return GL::FromAmountToAmountNDC( Vector2(vpAmount) , parentSizePx);
}
Vector2 RectTransform::
FromViewportAmountToLocalAmountNDC(const Vector2i &vpAmount) const
{
    return FromViewportAmountToLocalAmountNDC( Vector2(vpAmount) );
}


Vector2 RectTransform::
FromLocalAmountNDCToViewportAmount(const Vector2 &localAmountNDC) const
{
    Rect parentVPRect = GetParentViewportRect();
    Vector2 parentSizePx = Vector2::Max(Vector2::One, parentVPRect.GetSize());
    return GL::FromAmountNDCToAmount(localAmountNDC, parentSizePx);
}

Vector2 RectTransform::
FromLocalPointNDCToViewportPoint(const Vector2 &localPointNDC) const
{
    return GL::FromViewportPointNDCToViewportPoint(
                FromLocalPointNDCToViewportPointNDC(localPointNDC) );
}

Vector2 RectTransform::
FromViewportPointNDCToLocalPointNDC(const Vector2 &vpPointNDC) const
{
    return FromWorldToLocalPoint( Vector3(vpPointNDC, 0) ).xy();
}

Rect RectTransform::
FromLocalRectNDCToViewportRectNDC(const Rect &localRectNDC) const
{
    return Rect( FromLocalPointNDCToViewportPointNDC(localRectNDC.GetMin()),
                 FromLocalPointNDCToViewportPointNDC(localRectNDC.GetMax()));
}

Rect RectTransform::
FromViewportRectNDCToLocalRectNDC(const Rect &vpRectNDC) const
{
    return Rect( FromViewportPointNDCToLocalPointNDC(vpRectNDC.GetMin()),
                 FromViewportPointNDCToLocalPointNDC(vpRectNDC.GetMax()) );
}

Vector2 RectTransform::
FromLocalPointNDCToViewportPointNDC(const Vector2 &localPointNDC) const
{
    return FromLocalToWorldPoint( Vector3(localPointNDC, 0) ).xy();
}

void RectTransform::SetMarginLeft(int marginLeft)
{
    if (GetMarginLeft() != marginLeft)
    {
        m_marginLeftBot.x = marginLeft;
        Invalidate();
    }
}

void RectTransform::SetMarginTop(int marginTop)
{
    if (GetMarginTop() != marginTop)
    {
        m_marginRightTop.y = marginTop;
        Invalidate();
    }
}

void RectTransform::SetMarginRight(int marginRight)
{
    if (GetMarginRight() != marginRight)
    {
        m_marginRightTop.x = marginRight;
        Invalidate();
    }
}

void RectTransform::SetMarginBot(int marginBot)
{
    if (GetMarginBot() != marginBot)
    {
        m_marginLeftBot.y = marginBot;
        Invalidate();
    }
}

void RectTransform::AddMarginLeft(int marginLeft)
{
    SetMarginLeft(GetMarginLeft() + marginLeft);
}

void RectTransform::AddMarginTop(int marginTop)
{
    SetMarginTop(GetMarginTop() + marginTop);
}

void RectTransform::AddMarginRight(int marginRight)
{
    SetMarginRight(GetMarginRight() + marginRight);
}

void RectTransform::AddMarginBot(int marginBot)
{
    SetMarginBot(GetMarginBot() + marginBot);
}

void RectTransform::SetMargins(int marginAll)
{
    SetMargins(marginAll, marginAll, marginAll, marginAll);
}

void RectTransform::SetMarginMin(Axis axis, int marginMax)
{
    SetMargins(axis, GetMarginMin(axis), marginMax);
}
void RectTransform::SetMarginMax(Axis axis, int marginMin)
{
    SetMargins(axis, marginMin, GetMarginMax(axis));
}

void RectTransform::SetMarginLeftBot(const Vector2i &marginLeftBot)
{
    SetMarginLeft(marginLeftBot.x);
    SetMarginBot(marginLeftBot.y);
}

void RectTransform::SetMarginRightTop(const Vector2i &marginRightTop)
{
    SetMarginRight(marginRightTop.x);
    SetMarginTop(marginRightTop.y);
}

void RectTransform::SetMargins(const Vector2i &marginLeftBot,
                               const Vector2i &marginRightTop)
{
    SetMarginLeftBot(marginLeftBot);
    SetMarginRightTop(marginRightTop);
}

void RectTransform::SetMargins(Axis axis, const Vector2i &margins)
{
    SetMargins(axis, margins[0], margins[1]);
}

void RectTransform::SetMargins(Axis axis, int marginMin, int marginMax)
{
    if (axis == Axis::Vertical)
    { SetMarginBot(marginMin); SetMarginTop(marginMax); }

    if (axis == Axis::Horizontal)
    { SetMarginLeft(marginMin); SetMarginRight(marginMax); }
}

void RectTransform::SetMargins(int left, int top, int right, int bot)
{
    SetMarginBot(bot);
    SetMarginTop(top);
    SetMarginLeft(left);
    SetMarginRight(right);
}

void RectTransform::SetPivotPosition(const Vector2 &pivotPosition)
{
    if (m_pivotPosition != pivotPosition)
    {
        m_pivotPosition = pivotPosition;
        Invalidate();
    }
}

void RectTransform::SetAnchorMin(const Vector2 &anchorMin)
{
    if (m_anchorMin != anchorMin)
    {
        m_anchorMin = anchorMin;
        Invalidate();
    }
}

void RectTransform::SetAnchorMax(const Vector2 &anchorMax)
{
    if (m_anchorMax != anchorMax)
    {
        m_anchorMax = anchorMax;
        Invalidate();
    }
}

void RectTransform::SetAnchorMinX(float anchorMinX)
{
    SetAnchorMin( Vector2(anchorMinX, GetAnchorMin().y) );
}

void RectTransform::SetAnchorMinY(float anchorMinY)
{
    SetAnchorMin( Vector2(GetAnchorMin().x, anchorMinY) );
}

void RectTransform::SetAnchorMaxX(float anchorMaxX)
{
    SetAnchorMax( Vector2(anchorMaxX, GetAnchorMax().y) );
}

void RectTransform::SetAnchorMaxY(float anchorMaxY)
{
    SetAnchorMax( Vector2(GetAnchorMax().x, anchorMaxY) );
}

void RectTransform::SetAnchorX(const Vector2 &anchorX)
{
    SetAnchorMinX(anchorX[0]);
    SetAnchorMaxX(anchorX[1]);
}

void RectTransform::SetAnchorY(const Vector2 &anchorY)
{
    SetAnchorMinY(anchorY[0]);
    SetAnchorMaxY(anchorY[1]);
}

void RectTransform::SetAnchors(const Vector2 &anchorPoint)
{
    SetAnchors(anchorPoint, anchorPoint);
}

void RectTransform::SetAnchors(const Vector2 &anchorMin,
                               const Vector2 &anchorMax)
{
    SetAnchorMin(anchorMin);
    SetAnchorMax(anchorMax);
}

void RectTransform::SetWidthFromPivot(int width)
{
    SetMarginLeft ( -(GetPivotPosition().x - (-1)) * width / 2 );
    SetMarginRight(  (GetPivotPosition().x - ( 1)) * width / 2 );
}

void RectTransform::SetHeightFromPivot(int height)
{
    SetMarginBot( -(GetPivotPosition().y - (-1)) * height / 2 );
    SetMarginTop(  (GetPivotPosition().y - ( 1)) * height / 2 );
}

int RectTransform::GetMarginLeft() const { return GetMarginLeftBot().x; }
int RectTransform::GetMarginTop() const { return GetMarginRightTop().y; }
int RectTransform::GetMarginRight() const { return GetMarginRightTop().x; }
int RectTransform::GetMarginBot() const { return GetMarginLeftBot().y; }
int RectTransform::GetMarginMin(Axis axis) const { return GetMargins(axis)[0]; }
int RectTransform::GetMarginMax(Axis axis) const { return GetMargins(axis)[1]; }
Vector2 RectTransform::GetMargins(Axis axis) const
{
    return axis == Axis::Horizontal ?
                Vector2(GetMarginLeft(), GetMarginRight()) :
                Vector2(GetMarginBot(),  GetMarginTop());

}

const Vector2i& RectTransform::GetMarginLeftBot() const { return m_marginLeftBot; }
const Vector2i& RectTransform::GetMarginRightTop() const { return m_marginRightTop; }
const Vector2& RectTransform::GetPivotPosition() const { return m_pivotPosition; }
const Vector2& RectTransform::GetAnchorMin() const { return m_anchorMin; }
const Vector2& RectTransform::GetAnchorMax() const { return m_anchorMax; }

Rect RectTransform::GetViewportRectNDC() const
{
    return GetLocalToWorldMatrix() * Rect::NDCRect;
}
Rect RectTransform::GetViewportRect() const
{
    return GL::FromViewportRectNDCToViewportRect( GetViewportRectNDC() );
}

Rect RectTransform::GetParentScreenRectNDC() const
{
    Rect parentViewportRect = Rect::NDCRect;
    if (GetGameObject()->GetParent())
    {
        RectTransform *parentRectTransform =
                GetGameObject()->GetParent()->GetRectTransform();
        if (parentRectTransform)
        {
            parentViewportRect = parentRectTransform->GetViewportRectNDC();
        }
    }
    return parentViewportRect;
}
Rect RectTransform::GetParentViewportRect() const
{
    return GL::FromScreenRectNDCToScreenRect( GetParentScreenRectNDC() );
}


const Matrix4 &RectTransform::GetLocalToParentMatrix() const
{
    if (!IsInvalid()) { return m_localToParentMatrix; }

    Vector2d minMarginedAnchor (GetAnchorMin() +
                                FromViewportAmountToLocalAmountNDC(GetMarginLeftBot()));
    Vector2d maxMarginedAnchor (GetAnchorMax() -
                                FromViewportAmountToLocalAmountNDC(GetMarginRightTop()));
    Vector3d anchorScaling ((maxMarginedAnchor - minMarginedAnchor) * 0.5, 1);

    Vector3d moveToAnchorCenter( (maxMarginedAnchor + minMarginedAnchor) * 0.5,
                                  0);

    Matrix4d rtMatrix = Matrix4d::TranslateMatrix(moveToAnchorCenter) *
                        Matrix4d::ScaleMatrix(anchorScaling);
    Matrix4d transformMatrix ( Transform::GetLocalToParentMatrix() );

    m_localToParentMatrix = Matrix4f( transformMatrix * rtMatrix );
    Validate();

    return m_localToParentMatrix;
}

bool RectTransform::IsMouseOver(bool recursive) const
{
    if (!Input::IsMouseInsideScreen()) { return false; }

    if (!IsWaitingToBeDestroyed() && IsEnabled() &&
        GetViewportRectNDC().Contains( Input::GetMousePositionScreenNDC() ))
    {
        return true;
    }

    if (recursive)
    {
        List<RectTransform*> childrenRTs =
             GetGameObject()->GetComponentsInChildrenOnly<RectTransform>(true);
        for (RectTransform *childRT : childrenRTs)
        {
            if (childRT->IsMouseOver(false)) { return true; }
        }
    }
    return false;
}

void RectTransform::OnRender(RenderPass rp)
{
    Transform::OnRender(rp);
    if (rp != RenderPass::Overlay) { return; }

    /*
    Gizmos::Reset();
    Gizmos::SetThickness(1.0f);

    Rect r = GetViewportRectNDC(); (void)r;

    // Random::SetSeed(GetInstanceId());
    Gizmos::SetColor(Random::GetColorOpaque());
    // Gizmos::RenderFillRect(r);

    Gizmos::SetColor(Color::Green);
    Gizmos::RenderRect(r);

    Gizmos::SetColor(Color::Yellow);
    Gizmos::RenderScreenLine(r.GetMinXMaxY(), r.GetMaxXMinY());
    Gizmos::SetColor(Color::Yellow);
    Gizmos::RenderScreenLine(r.GetMinXMinY(), r.GetMaxXMaxY());
    float size = GL::FromViewportAmountToViewportAmountNDC(Vector2(2)).x;
    Gizmos::SetColor(Color::Red);
    Gizmos::RenderRect(Rect(r.GetCenter() - Vector2(size),
                            r.GetCenter() + Vector2(size)));

    Gizmos::SetColor(Color::Blue);
    Rect anchorRect = FromLocalNDCToViewportNDC(
                                Rect(GetAnchorMin(), GetAnchorMax()));
    Gizmos::RenderRect(anchorRect);
    */
}

void RectTransform::CloneInto(ICloneable *clone) const
{
    Transform::CloneInto(clone);
    RectTransform *rt = Cast<RectTransform*>(clone);

    rt->SetMargins( GetMarginLeftBot(), GetMarginRightTop() );
    rt->SetAnchors( GetAnchorMin(), GetAnchorMax() );
    rt->SetPivotPosition( GetPivotPosition() );
}

void RectTransform::ImportXML(const XMLNode &xmlInfo)
{
    Transform::ImportXML(xmlInfo);

    if (xmlInfo.Contains("MarginLeftBot"))
    {
        SetMargins (xmlInfo.Get<Vector2i>("MarginLeftBot"),
                    xmlInfo.Get<Vector2i>("MarginRightTop"));
    }

    if (xmlInfo.Contains("PivotPosition"))
    { SetPivotPosition( xmlInfo.Get<Vector2>("PivotPosition") ); }

    if (xmlInfo.Contains("AnchorMin"))
    { SetAnchorMin( xmlInfo.Get<Vector2>("AnchorMin") ); }

    if (xmlInfo.Contains("AnchorMax"))
    { SetAnchorMax( xmlInfo.Get<Vector2>("AnchorMax") ); }
}

void RectTransform::ExportXML(XMLNode *xmlInfo) const
{
    Transform::ExportXML(xmlInfo);

    xmlInfo->Set("MarginLeftBot",  GetMarginLeftBot() );
    xmlInfo->Set("MarginRightTop", GetMarginRightTop());

    xmlInfo->Set("PivotPosition",  GetPivotPosition());
    xmlInfo->Set("AnchorMin",      GetAnchorMin()    );
    xmlInfo->Set("AnchorMax",      GetAnchorMax()    );
}

void RectTransform::OnEnabled()  { Invalidate(); }
void RectTransform::OnDisabled() { Invalidate(); }
