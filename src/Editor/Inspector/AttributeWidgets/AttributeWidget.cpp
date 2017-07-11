#include "Bang/AttributeWidget.h"

#include <QLabel>
#include <QBoxLayout>
#include "Bang/WinUndef.h"

#include "Bang/XMLNode.h"
#include "Bang/Inspector.h"
#include "Bang/XMLAttribute.h"
#include "Bang/AttrWidgetInt.h"
#include "Bang/AttrWidgetBool.h"
#include "Bang/AttrWidgetEnum.h"
#include "Bang/AttrWidgetFile.h"
#include "Bang/AttrWidgetColor.h"
#include "Bang/AttrWidgetFloat.h"
#include "Bang/AttrWidgetString.h"
#include "Bang/AttrWidgetButton.h"
#include "Bang/AttrWidgetVectorFloat.h"

AttributeWidget::AttributeWidget(const XMLAttribute &xmlAttribute,
                                 bool isSubWidget,
                                 bool createLabel,
                                 bool labelAbove )
{
    setVisible(false);
    Refresh(xmlAttribute);

    m_horizontalLayout.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_horizontalLayout.setContentsMargins(labelAbove ? 0 : 5,0,0,0);

    if (!isSubWidget)
    {
        QString labelString = "";
        if (createLabel)
        {
            String name = xmlAttribute.GetName();
            name = Inspector::FormatInspectorLabel(name);
            labelString = name.ToQString();

            QLabel *attrNameLabel = new QLabel(labelString);
            attrNameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            m_horizontalLayout.addWidget(attrNameLabel);
        }
    }
    Refresh(xmlAttribute);

    setLayout(&m_horizontalLayout);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    SetHeightSizeHint(labelAbove ? 45 : 25);
    setVisible(true);
}

void AttributeWidget::AfterConstructor()
{
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
}

void AttributeWidget::Refresh(const XMLAttribute &attribute)
{
    m_readonly = attribute.HasProperty(XMLProperty::Readonly);
    setEnabled( !attribute.HasProperty(XMLProperty::Disabled) );
}

int AttributeWidget::GetHeightSizeHint() const
{
    if (!IsVisible()) { return 0; }
    return m_heightSizeHint;
}

void AttributeWidget::SetHeightSizeHint(int heightSizeHint)
{
    m_heightSizeHint = heightSizeHint;
}

void AttributeWidget::SetVisible(bool visible)
{
    if (visible && !IsVisible()) { setVisible(true); }
    else if (!visible && IsVisible()) { hide(); }
}

bool AttributeWidget::IsVisible() const
{
    return !isHidden();
}

XMLAttribute AttributeWidget::GetXMLAttribute() const
{
    XMLAttribute empty;
    return empty; // Empty
}

AttributeWidget *AttributeWidget::FromXMLAttribute(const XMLAttribute &xmlAttr)
{
    const XMLAttribute::Type attrType = xmlAttr.GetType();

    if (xmlAttr.HasVectoredType())
    {
        int numberOfFields = xmlAttr.GetNumberOfFieldsOfType();
        if (numberOfFields == 1)
        {
            if (attrType == XMLAttribute::Type::Float)
            {
                return new AttrWidgetFloat(xmlAttr, false);
            }
            else { return new AttrWidgetInt(xmlAttr, false); }
        }
        else { return new AttrWidgetVectorFloat(xmlAttr); }
    }
    else if (attrType == XMLAttribute::Type::File)
    {
        return new AttrWidgetFile(xmlAttr);
    }
    else if (attrType == XMLAttribute::Type::String)
    {
        return new AttrWidgetString(xmlAttr);
    }
    else if (attrType == XMLAttribute::Type::Bool)
    {
        return new AttrWidgetBool(xmlAttr);
    }
    else if (attrType == XMLAttribute::Type::Enum)
    {
        return new AttrWidgetEnum(xmlAttr);
    }
    else if (attrType == XMLAttribute::Type::Color)
    {
        return new AttrWidgetColor(xmlAttr);
    }
    else if (attrType == XMLAttribute::Type::Button)
    {
        return new AttrWidgetButton(xmlAttr);
    }

    return nullptr;
}
