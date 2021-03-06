#include "Bang/UICanvas.h"

#include "Bang/Rect.h"
#include "Bang/Input.h"
#include "Bang/Scene.h"
#include "Bang/Cursor.h"
#include "Bang/XMLNode.h"
#include "Bang/GameObject.h"
#include "Bang/IFocusable.h"
#include "Bang/SceneManager.h"
#include "Bang/RectTransform.h"
#include "Bang/UILayoutManager.h"

USING_NAMESPACE_BANG

UICanvas::UICanvas()
{
    m_uiLayoutManager = new UILayoutManager();
}

UICanvas::~UICanvas()
{
    delete m_uiLayoutManager;
}

void UICanvas::OnStart()
{
    Component::OnStart();
}

void UICanvas::OnUpdate()
{
    Component::OnUpdate();

    // Focus
    IFocusable *focusMouseOver = nullptr;

    Array<IFocusable*> focusables;
    GetSortedFocusCandidatesByOcclusionOrder(GetGameObject(), &focusables);
    for (IFocusable *focusable : focusables)
    {
        Component *focusableComp = Cast<Component*>(focusable);
        if (focusableComp->IsEnabled() && focusable->IsFocusEnabled())
        {
            RectTransform *rt = focusableComp->GetGameObject()->GetRectTransform();
            if (rt && rt->IsMouseOver())
            {
                focusMouseOver = focusable;
                if (Input::GetMouseButtonDown(MouseButton::Left))
                {
                    SetFocus(focusable);
                }

                break; // Finished searching!
            }
        }
    }

    // Reset focus when clicking out of everything
    if (Input::GetMouseButtonDown(MouseButton::Left) && !focusMouseOver)
    {
        SetFocus(nullptr);
    }
    SetFocusMouseOver(focusMouseOver);

    // Reset cursor type on mouse up
    if (Input::GetMouseButtonUp(MouseButton::Left))
    {
        Cursor::Set(Cursor::Type::Arrow);
    }

    // Tabbing
    if (Input::GetKeyDownRepeat(Key::Tab))
    {
        IFocusable *currentFocus = GetCurrentFocus();
        if (currentFocus)
        {
            const int n = focusables.Size();
            int indexOfFocus = focusables.IndexOf(currentFocus);
            bool shift = Input::GetKey(Key::LShift) || Input::GetKey(Key::RShift);
            int newFocusIndex = indexOfFocus;

            while (true)
            {
                newFocusIndex = (newFocusIndex + (shift ? 1 : -1) + n) % n;
                if (newFocusIndex == indexOfFocus) { break; } // Complete loop

                IFocusable *newFocus = focusables.At(newFocusIndex);
                Component *newFocusComp = Cast<Component*>(newFocus);
                const bool isValid = newFocus->IsFocusEnabled() &&
                                     (!newFocusComp ||
                                       newFocusComp->IsEnabled(true));
                if (isValid) { break; }
            }
            SetFocus( focusables.At(newFocusIndex) );
        }
    }

    // Update focusables
    for (IFocusable *focusable : focusables)
    {
        focusable->UpdateFromCanvas();
    }
}

void UICanvas::Invalidate()
{
    List<RectTransform*> rts = GetGameObject()->
                               GetComponentsInChildren<RectTransform>(true);
    for (RectTransform *rt : rts)
    {
        rt->Invalidate();
    }
    GetLayoutManager()->RebuildLayout(GetGameObject());
}

void UICanvas::SetFocus(IFocusable *_newFocusable)
{
    IFocusable *newFocusable = _newFocusable;
    Object *obj = newFocusable ? DCAST<Object*>(newFocusable) : nullptr;
    if (obj)
    {
        if (obj->IsWaitingToBeDestroyed()) { newFocusable = nullptr; }
    }

    if (newFocusable != GetCurrentFocus())
    {
        if (GetCurrentFocus())
        {
            Object *focusableObj = Cast<Object*>( GetCurrentFocus() );
            if (GetCurrentFocus() != GetCurrentFocusMouseOver())
            { focusableObj->EventEmitter<IDestroyListener>::UnRegisterListener(this); }

            GetCurrentFocus()->ClearFocus();
        }

        p_currentFocus = newFocusable;
        if (GetCurrentFocus())
        {
            Object *focusableObj = DCAST<Object*>( GetCurrentFocus() );
            focusableObj->EventEmitter<IDestroyListener>::RegisterListener(this);

            GetCurrentFocus()->SetFocus();
        }
    }
}

void UICanvas::SetFocusMouseOver(IFocusable *_newFocusableMO)
{
    IFocusable *newFocusableMO = _newFocusableMO;
    Object *obj = newFocusableMO ? DCAST<Object*>(newFocusableMO) : nullptr;
    if (obj)
    {
        if (obj->IsWaitingToBeDestroyed()) { newFocusableMO = nullptr; }
    }

    if (newFocusableMO != GetCurrentFocusMouseOver())
    {
        Cursor::Set(Cursor::Type::Arrow); // Reset cursor type

        if (GetCurrentFocusMouseOver())
        {
            Object *focusableMOObj = Cast<Object*>( GetCurrentFocusMouseOver() );
            if (GetCurrentFocus() != GetCurrentFocusMouseOver())
            { focusableMOObj->EventEmitter<IDestroyListener>::UnRegisterListener(this); }

            GetCurrentFocusMouseOver()->PropagateMouseOverToListeners(false);
        }

        p_currentFocusMouseOver = newFocusableMO;
        if (GetCurrentFocusMouseOver())
        {
            Object *focusableMOObj = Cast<Object*>( GetCurrentFocusMouseOver() );
            focusableMOObj->EventEmitter<IDestroyListener>::RegisterListener(this);
            GetCurrentFocusMouseOver()->PropagateMouseOverToListeners(true);
        }
    }
}

void UICanvas::OnAfterChildrenUpdate()
{
    Component::OnAfterChildrenUpdate();

    if (GetCurrentFocus()) { GetCurrentFocus()->m_hasJustFocusChanged = false; }
}

void UICanvas::OnBeforeRender()
{
    Component::OnBeforeRender();
    GetLayoutManager()->RebuildLayout( GetGameObject() );
}

void UICanvas::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
}

void UICanvas::ImportXML(const XMLNode &xmlInfo)
{
    Component::ImportXML(xmlInfo);
}

void UICanvas::ExportXML(XMLNode *xmlInfo) const
{
    Component::ExportXML(xmlInfo);
}

void UICanvas::OnDestroyed(EventEmitter<IDestroyListener> *object)
{
    IFocusable *destroyedFocusable = DCAST<IFocusable*>(object);

    if (destroyedFocusable == GetCurrentFocus())
    { SetFocus(nullptr); }

    if (destroyedFocusable == GetCurrentFocusMouseOver())
    { SetFocusMouseOver(nullptr); }
}

bool UICanvas::HasFocus(const Component *comp, bool recursive)
{
    return comp ? UICanvas::HasFocus(comp->GetGameObject(), recursive) : false;
}

bool UICanvas::HasFocus(const GameObject *go, bool recursive)
{
    if (!go) { return false; }

    if (!recursive)
    {
        List<IFocusable*> focusables = go->GetComponents<IFocusable>();
        for (IFocusable *focusable : focusables)
        {
            if (focusable == GetCurrentFocus()) { return true; }
        }
    }
    else
    {
        if (HasFocus(go, false)) { return true; }
        else
        {
            const List<GameObject*>& children = go->GetChildren();
            for (GameObject *child : children)
            {
                if (HasFocus(child, true)) { return true; }
            }
        }
    }
    return false;
}

bool UICanvas::IsMouseOver(const Component *comp, bool recursive)
{
    return comp ? UICanvas::IsMouseOver(comp->GetGameObject(), recursive) : false;
}

bool UICanvas::IsMouseOver(const GameObject *go, bool recursive)
{
    if (!go) { return false; }
    if (!recursive)
    {
        List<IFocusable*> focusables = go->GetComponents<IFocusable>();
        for (IFocusable *focusable : focusables)
        {
            if (focusable == GetCurrentFocusMouseOver()) { return true; }
        }
    }
    else
    {
        if (IsMouseOver(go, false)) { return true; }
        else
        {
            const List<GameObject*>& children = go->GetChildren();
            for (GameObject *child : children)
            {
                if (IsMouseOver(child, true)) { return true; }
            }
        }
    }
    return false;
}

IFocusable *UICanvas::GetCurrentFocus() { return p_currentFocus; }
IFocusable* UICanvas::GetCurrentFocusMouseOver() { return p_currentFocusMouseOver; }

UILayoutManager *UICanvas::GetLayoutManager() const
{
    return m_uiLayoutManager;
}

UICanvas *UICanvas::GetActive(const GameObject *go)
{
    UICanvas *canvasInThis = go->GetComponent<UICanvas>();
    return canvasInThis ? canvasInThis : go->GetComponentInParent<UICanvas>();
}

UICanvas *UICanvas::GetActive(const Component *comp)
{
    return UICanvas::GetActive(comp->GetGameObject());
}

struct GameObjectZComparer
{
    inline bool operator() (const IFocusable * const& lhs,
                            const IFocusable * const& rhs)
    {
        const GameObject *glhs = Cast<const GameObject*>(lhs);
        const GameObject *grhs = Cast<const GameObject*>(rhs);
        if (!glhs) { glhs = Cast<const Component*>(lhs)->GetGameObject(); }
        if (!grhs) { grhs = Cast<const Component*>(rhs)->GetGameObject(); }
        Transform *lt = glhs->GetTransform();
        Transform *rt = grhs->GetTransform();
        if (!lt) { return false; }
        if (!rt) { return true; }
        return lt->GetPosition().z < rt->GetPosition().z;
    }
};


void UICanvas::GetSortedFocusCandidatesByOcclusionOrder(
                                    const GameObject *go,
                                    Array<IFocusable*> *sortedCandidates) const
{
    GetSortedFocusCandidatesByPaintOrder(go, sortedCandidates);
    Containers::StableSort(sortedCandidates->Begin(), sortedCandidates->End(),
                           GameObjectZComparer());
}

void UICanvas::GetSortedFocusCandidatesByPaintOrder(
                                    const GameObject *go,
                                    Array<IFocusable*> *sortedCandidates) const
{
    List<GameObject*> children = go->GetChildren();
    Containers::Reverse(children.Begin(), children.End());
    for (GameObject *child : children)
    {
        if (child->IsEnabled() && !child->IsWaitingToBeDestroyed())
        {
            GetSortedFocusCandidatesByPaintOrder(child, sortedCandidates);

            IFocusable *focusable = child->GetComponent<IFocusable>();
            if (focusable)
            {
                Object *focusableObj = Cast<Object*>(focusable);
                if (!focusableObj ||
                     (focusableObj->IsEnabled() &&
                     !focusableObj->IsWaitingToBeDestroyed())
                   )
                {
                    sortedCandidates->PushBack(focusable);
                }
            }
        }
    }
}
