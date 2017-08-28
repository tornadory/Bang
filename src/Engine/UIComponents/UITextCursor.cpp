#include "Bang/UITextCursor.h"

#include "Bang/Time.h"
#include "Bang/Material.h"
#include "Bang/LineRenderer.h"
#include "Bang/RectTransform.h"
#include "Bang/ComponentFactory.h"

UITextCursor::UITextCursor()
{
    UseMaterialCopy();
    GetMaterial()->SetDiffuseColor(Color::Black);
    SetViewProjMode(GL::ViewProjMode::IgnoreBoth);
    SetRenderPass(RenderPass::Canvas);
    SetStroke(1.5f);

    constexpr float limit = 1.0f;
    SetPoints({
               Vector3(0.0f, -limit, 0.0f),
               Vector3(0.0f,  limit, 0.0f),

               Vector3(-limit, -limit, 0.0f),
               Vector3( limit, -limit, 0.0f),

               Vector3(-limit,  limit, 0.0f),
               Vector3( limit,  limit, 0.0f),
              });
}

UITextCursor::~UITextCursor()
{
}

void UITextCursor::OnUpdate()
{
    Component::OnUpdate();

    m_cursorTime += Time::GetDeltaTime();
    SetVisible( m_cursorTime <= m_cursorTickTime );
    if (m_cursorTime >= m_cursorTickTime * 2) { m_cursorTime = 0.0f; }
}

void UITextCursor::SetStroke(float cursorWidth)
{
    SetLineWidth(cursorWidth);
}

void UITextCursor::SetTickTime(float cursorTickTime)
{
    m_cursorTickTime = cursorTickTime;
}

float UITextCursor::GetStroke() const
{
    return GetLineWidth();
}

float UITextCursor::GetTickTime() const
{
    return m_cursorTickTime;
}
