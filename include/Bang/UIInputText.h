#ifndef UIINPUTTEXT_H
#define UIINPUTTEXT_H

#include "Bang/IEventEmitter.h"
#include "Bang/ILayoutElement.h"
#include "Bang/IFocusListener.h"
#include "Bang/IComponentDriver.h"
#include "Bang/IValueChangedListener.h"

NAMESPACE_BANG_BEGIN

FORWARD class GameObject;
FORWARD class UITextCursor;
FORWARD class UIScrollArea;
FORWARD class RectTransform;
FORWARD class UITextRenderer;
FORWARD class UIImageRenderer;

class UIInputText : public IComponentDriver<UIInputText>,
                    public Component,
                    public EventEmitter<IValueChangedListener>,
                    public IFocusListener,
                    public ILayoutElement
{
    COMPONENT(UIInputText)

public:
    virtual ~UIInputText();

    void OnUpdate() override;

    void SetCursorIndex(int index);
    void SetSelection(int selectionBeginIndex,
                      int selectionEndIndex);

    String GetSelectedText() const;
    void ReplaceSelectedText(const String &replaceStr);

    void ResetSelection();

    void SetAllowedCharacters(const String &allowedCharacters);

    // IFocusListener
    virtual void OnFocusTaken() override;
    virtual void OnFocusLost() override;

    // ILayoutElement
    virtual void CalculateLayout(Axis axis) override;

    UITextCursor *GetCursor() const;
    UITextRenderer *GetText() const;
    UIImageRenderer *GetBackground() const;

private:
    static const Vector2i LookAheadOffsetPx;
    static const int MarginX;
    static const int MarginY;

    UIImageRenderer *p_background = nullptr;

    UILabel *p_label = nullptr;
    UITextCursor *p_cursor = nullptr;
    UIScrollArea *p_scrollArea = nullptr;
    String m_allowedCharacters = "";

    UIInputText();

    void HandleTyping();
    void HandleKeySelection(bool wasSelecting);
    void HandleCursorIndices(bool wasSelecting);
    String FilterAllowedInputText(const String &inputText);

    int GetCursorIndex() const;
    int GetSelectionIndex() const;

    UILabel *GetLabel() const;
    RectTransform *GetLabelRT() const;
    RectTransform *GetTextRT() const;
    RectTransform *GetRT() const;

    bool IsDelimiter(char initialChar, char currentChar) const;
    int GetWordSplitIndex(int startingIndex, bool forward) const;

    void UpdateCursorRenderer();
    void UpdateTextScrolling();
    bool IsShiftPressed() const;

    // IComponentDriver
    static UIInputText *CreateInto(GameObject *go);

    friend class GameObjectFactory;
    friend class IComponentDriver<UIInputText>;
};

NAMESPACE_BANG_END

#endif // UIINPUTTEXT_H
