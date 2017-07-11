#include "Bang/EditorGizmo.h"

#include "Bang/Input.h"

EditorGizmo::EditorGizmo(GameObject *attachedGameObject)
{
    m_attachedGameObject = attachedGameObject;

    GetHideFlags()->SetOn(HideFlag::HideAndDontSave);
}

EditorGizmo::~EditorGizmo()
{

}

void EditorGizmo::OnEditorUpdate()
{
    GameObject::OnEditorUpdate();

    if (m_mouseIsOver && Input::GetMouseButtonDown(Input::MouseButton::Left))
    {
        m_grabbed = true;
        Cursor::SetIcon(m_cursorIconWhenGrabbed);
    }

    if (m_grabbed)
    {
        if (Input::GetMouseButtonUp(Input::MouseButton::Left))
        {
            m_grabbed = false;
            Input::SetMouseWrapping(false);
            Cursor::SetIcon(m_mouseIsOver ? m_cursorIconWhenOver
                                          : Cursor::CursorIcon::Arrow);
        }
        else
        {
            Input::SetMouseWrapping(true);
            Cursor::SetIcon(m_cursorIconWhenGrabbed);
        }
    }
}

void EditorGizmo::OnMouseEnter(bool fromChildren)
{
    GameObject::OnMouseEnter(fromChildren);
    if (!m_mouseIsOver)
    {
        m_mouseIsOver = true;
        if (!m_grabbed)
        {
            Cursor::SetIcon(m_cursorIconWhenOver);
        }
    }
}

void EditorGizmo::OnMouseExit(bool fromChildren)
{
    GameObject::OnMouseExit(fromChildren);
    if (m_mouseIsOver)
    {
        m_mouseIsOver = false;
        if (!m_grabbed)
        {
            Cursor::SetIcon(Cursor::CursorIcon::Arrow);
        }
    }
}

bool EditorGizmo::IsGrabbed() const
{
    return m_grabbed;
}
