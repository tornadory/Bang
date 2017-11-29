#include "Bang/UIInputNumber.h"

#include "Bang/Input.h"
#include "Bang/UICanvas.h"
#include "Bang/GameObject.h"
#include "Bang/UIInputText.h"
#include "Bang/RectTransform.h"
#include "Bang/UITextRenderer.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

UIInputNumber::UIInputNumber()
{
}

UIInputNumber::~UIInputNumber()
{
}

void UIInputNumber::OnStart()
{
    UIInputText *inputText = GetGameObject()->GetComponent<UIInputText>();
    inputText->EventEmitter<IValueChangedListener>::RegisterListener(this);
}

void UIInputNumber::OnUpdate()
{
    Component::OnUpdate();

    if (UICanvas::HasFocus(this))
    {
        if (Input::GetKeyDown(Key::Enter)) { UICanvas::ClearFocus(); }
    }
}

void UIInputNumber::SetNumber(float v)
{
    m_value = v;
    String vStr = String::ToString(v, 2);
    GetInputText()->GetText()->SetContent(vStr);
}

float UIInputNumber::GetNumber() const
{
    return m_value;
}

void UIInputNumber::OnFocusTaken(IFocusable *focusable)
{
    IFocusListener::OnFocusTaken(focusable);
}

void UIInputNumber::OnFocusLost(IFocusable *focusable)
{
    IFocusListener::OnFocusLost(focusable);
    SetNumber( GetNumber() );
}

void UIInputNumber::UpdateValueFromText()
{
    const String &content = GetInputText()->GetText()->GetContent();
    if (!content.IsEmpty())
    {
        std::istringstream iss(content);
        iss >> m_value;
    }
    else { m_value = 0; }
}

UIInputText *UIInputNumber::GetInputText() const { return p_inputText; }

void UIInputNumber::OnValueChanged(Object*)
{
    UpdateValueFromText();
    EventEmitter<IValueChangedListener>::
            PropagateToListeners(&IValueChangedListener::OnValueChanged, this);
}

UIInputNumber *UIInputNumber::CreateInto(GameObject *go)
{
    REQUIRE_COMPONENT(go, RectTransform);

    GameObjectFactory::CreateUIInputTextInto(go);

    UIInputNumber *inputNumber = go->AddComponent<UIInputNumber>();

    UIInputText *inputText = go->GetComponent<UIInputText>();
    inputText->SetAllowedCharacters("0123456789.,-+");
    inputText->GetText()->SetHorizontalAlign(HorizontalAlignment::Left);
    inputText->GetText()->SetTextSize(10);

    inputNumber->p_inputText = inputText;

    inputNumber->SetNumber(0.0f);

    return inputNumber;
}

