#include "Bang/UIDirLayout.h"

#include "Bang/Array.h"
#include "Bang/XMLNode.h"
#include "Bang/UIGameObject.h"
#include "Bang/RectTransform.h"

UIDirLayout::UIDirLayout() : UIDirLayout(false)
{
}

UIDirLayout::UIDirLayout(bool vertical) :
    m_vertical(vertical)
{
}

UIDirLayout::~UIDirLayout()
{
}

void UIDirLayout::OnUpdate()
{
    Component::OnUpdate();

    const int numChildren = GetGameObject()->GetChildren().Size();
    float latestAnchor = (m_vertical ? 1.0f : -1.0f);
    const float balancedSize = (1.0f / numChildren);

    float totalStretch = 0.0f;
    for (int i = 0; i < GetGameObject()->GetChildren().Size(); ++i)
    {
        totalStretch += GetStretch(i);
    }

    int i = 0;
    for (GameObject *child : GetGameObject()->GetChildren())
    {
        float sizeNDC;
        if (totalStretch > 0) { sizeNDC  = (GetStretch(i) / totalStretch); }
        else { sizeNDC = balancedSize; }
        sizeNDC *= 2.0f;

        const bool first = (i == 0);
        const bool last  = (i == numChildren-1);
        const int spacing = (first || last) ? m_spacingPx / 1.5 : m_spacingPx;
        RectTransform *rt = child->GetComponent<RectTransform>();
        int marginLeft = 0, marginTop = 0, marginRight = 0, marginBot = 0;
        if (m_vertical)
        {
            rt->SetAnchorMin( Vector2(-1.0f, latestAnchor - sizeNDC) );
            rt->SetAnchorMax( Vector2(1.0f, latestAnchor) );
            marginTop = last ? spacing : ((spacing+1) / 2);
            marginBot = first ? spacing : (spacing / 2);
        }
        else
        {
            rt->SetAnchorMin( Vector2(latestAnchor, -1.0f) );
            rt->SetAnchorMax( Vector2(latestAnchor + sizeNDC, 1.0f) );
            marginLeft  = last ? spacing : ((spacing+1) / 2);
            marginRight = first ? spacing : (spacing / 2);
        }
        rt->SetMargins(marginLeft, marginTop, marginRight, marginBot);

        latestAnchor += sizeNDC * (m_vertical ? -1.0f : 1.0f);
        ++i;
    }
}

void UIDirLayout::SetSpacing(int spacingPx)
{
    m_spacingPx = spacingPx;
}

void UIDirLayout::SetStretch(int index, float stretch)
{
    m_stretches[index] = stretch;
}

float UIDirLayout::GetStretch(int index) const
{
    if (!m_stretches.ContainsKey(index)) { return 1.0f; }
    return m_stretches.At(index);
}

void UIDirLayout::Read(const XMLNode &xmlInfo)
{
    Component::Read(xmlInfo);

    if (xmlInfo.Contains("SpacingPx"))
    { SetSpacing( xmlInfo.Get<int>("SpacingPx") ); }

    int i = 0;
    while (xmlInfo.Contains("Stretch" + String(i)))
    {
        SetStretch(i, xmlInfo.Get<float>("Stretch" + String(i)));
        ++i;
    }
    Debug_Log("STRETCHES: " << m_stretches);
}

void UIDirLayout::Write(XMLNode *xmlInfo) const
{
    Component::Write(xmlInfo);

    xmlInfo->Set("SpacingPx", m_spacingPx);

    const Map<int, float> &stretches = m_stretches;
    for (auto it = stretches.cbegin(); it != stretches.cend(); ++it)
    {
        float stretch = it->second;
        xmlInfo->Set("Stretch" + String(it->first), stretch);
    }
}

